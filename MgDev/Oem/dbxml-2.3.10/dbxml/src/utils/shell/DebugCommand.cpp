//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DebugCommand.cpp,v 1.40 2006/10/30 17:46:07 bostic Exp $
//

#include "DebugCommand.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>

#include <db_cxx.h>
#include "ScopedDbt.hpp"
#include "NameID.hpp"

#include "UTF8.hpp"
#include "QueryContext.hpp"
#include "PrintDataItemTree.hpp"
#include "OperationContext.hpp"
#include "Document.hpp"
#include "TransactedContainer.hpp"
#include "Key.hpp"
#include "DbWrapper.hpp"
#include "QueryExecutionContext.hpp"
#include "optimizer/QueryPlanGenerator.hpp"
#include "optimizer/CostBasedOptimizer.hpp"
#include "optimizer/ASTReplaceOptimizer.hpp"
#include "optimizer/ASTRewriteOptimizer.hpp"
#include "optimizer/QueryPlanOptimizer.hpp"
#include "optimizer/CombineStepsOptimizer.hpp"
#include "dataItem/DbXmlContext.hpp"
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "nodeStore/NsIterator.hpp"

#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XQException.hpp>

using namespace DbXml;
using namespace std;

string DebugCommand::getCommandName() const
{
	return "debug";
}

string DebugCommand::getBriefHelp() const
{
	return "Debug command -- internal use only";
}

string DebugCommand::getMoreHelp() const
{
	return "";
}

#define DB_TEST 0
#define QUERY_TEST 1
#define ALGEBRA_TEST 0

void DebugCommand::execute(Args &args, Environment &env)
{
// 	if(args.size() != 2) {
// 		throw CommandException("Wrong number of arguments");
// 	}

	try {
#if DB_TEST
		DbEnv *env = new DbEnv(0);
		env->open("TESTDIR",
			  DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|
			  DB_INIT_TXN, 0);
		Db db(env, 0);
		db.open(0, "testdb", "testdb", DB_RECNO, DB_CREATE, 0);

		u_int32_t idt = 0;
		DbtIn key(&idt, sizeof(idt));

		DbtOut data;
		data.set("frank", 6);

		{
			NameID id;
			id.setThisFromDbt(key, false);
			Buffer tmp(data.get_data(), data.get_size(), true);
			cerr << "id = " << id << ", value = " << tmp.asString() << endl;
		}

		db.put(0, &key, &data, DB_APPEND);

		{
			ID id;
			id.setThisFromDbt(key, false);
			Buffer tmp(data.get_data(), data.get_size(), true);
			cerr << "id = " << id << ", value = " << tmp.asString() << endl;
		}

		db.close(0);
#endif

#if QUERY_TEST
		ReferenceMinder minder;
		((QueryContext&)env.context()).setMinder(&minder);
		Transaction *txn = 0;
		if (env.txn())
			txn = *env.txn();
		((QueryContext&)env.context()).setTransaction(txn);
		QueryExecutionContext qec((QueryContext&)env.context(), false);
		((QueryContext&)env.context()).setQueryExecutionContext(&qec);

		DbXmlContext *context = ((QueryContext&)env.context()).createStaticContext(txn);

		XQilla xqilla;
		XQQuery *query = xqilla.parse(UTF8ToXMLCh(args[1]).str(), XQilla::XQUERY, context, 0,
			XQilla::NO_STATIC_RESOLUTION | XQilla::NO_ADOPT_CONTEXT);

		OutputTree *before = new OutputTree("Initial tree", context);
		StaticResolver *sr = new StaticResolver(context, before);
		OutputTree *afterStatic = new OutputTree("After Static Resolution", context, sr);
		ASTReplaceOptimizer *rep = new ASTReplaceOptimizer(minder, context, afterStatic);
		OutputTree *afterRep = new OutputTree("After AST Replace", context, rep);
		StaticTyper *st = new StaticTyper(context, afterRep);
		OutputTree *afterStatic2 = new OutputTree("After Static Typing", context, st);
		QueryPlanGenerator *qpGen = new QueryPlanGenerator(context, afterStatic2);
		OutputTree *afterQP = new OutputTree("After Query Plan Generation", context, qpGen);
		QueryPlanOptimizer *qpo = new QueryPlanOptimizer(((QueryContext&)env.context()).getManager(), 0, context,
			afterQP);
		OutputTree *afterQPO = new OutputTree("After QueryPlan Optimisation", context, qpo);
		ASTRewriteOptimizer *rew = new ASTRewriteOptimizer(context, afterQPO);
		OutputTree *afterREW = new OutputTree("After AST Rewrite", context, rew);
		CombineStepsOptimizer *com = new CombineStepsOptimizer(context, afterREW);
		OutputTree *afterCOM = new OutputTree("After Combine Steps Optimizer", context, com);
		CostBasedOptimizer *cbo = new CostBasedOptimizer(((QueryContext&)env.context()).getManager(), 0, context,
			afterCOM);
		OutputTree *afterCBO = new OutputTree("After Cost Based Optimisation", context, cbo);
		DocOrderOptimizer *doo = new DocOrderOptimizer(context, afterCBO);
		OutputTree *afterDOO = new OutputTree("After Document Order Optimisation", context, doo);
		StaticTyper *st2 = new StaticTyper(context, afterDOO);
		OutputTree optimizer("After Static Typing (again)", context, st2);

		optimizer.startOptimize(query);

		cout << "Global Query Plan: " << endl;
		if(qpGen->getQueryPlan() != 0) cout << qpGen->getQueryPlan()->toString(false) << endl;

		DynamicContext *context2 = ((QueryContext&)env.context()).createDynamicContext(txn, context);
		Result result(query->execute(context2));

		cout << "Results:\n";
		Item::Ptr item;
		while((item = result->next(context2)) != NULLRCP && !env.sigBlock().isInterrupted()) {
			cout << XMLChToUTF8(item->asString(context2)).str() << endl;
		}
#endif

#if ALGEBRA_TEST
		Transaction *txn = 0;
		if (env.txn())
			txn = *env.txn();

		QueryContext &qc = (QueryContext&)env.context();
		qc.setFlags(DBXML_LAZY_DOCS);

		TransactedContainer *cont = 0;
		if(env.container())
			cont = (TransactedContainer*)((Container*)*env.container());
		else throw CommandException("No default container");

		Index index("node-element-presence");
		Name name1("", args[1]);
		Name name2("", args[2]);

		IndexDatabase *idb = cont->getIndexDB(index.getSyntax())->getIndexDB();

		Key key1(0);
		key1.setIndex(index);
		key1.setIDsFromNames(qc.getOperationContext(),
			*cont, 0, name1.getURIName().c_str());

		NsIterator *ancestors = idb->createIterator(txn, &key1);

		Key key2(0);
		key2.setIndex(index);
		key2.setIDsFromNames(qc.getOperationContext(),
			*cont, 0, name2.getURIName().c_str());

		NsIterator *descendants = idb->createIterator(txn, &key2);

		NsStructuralJoin sJoin(ancestors, descendants);

		env.deleteResults();
		env.results() = new XmlResults(env.db().createResults());

		DbXmlContext *context = qc.createStaticContext(txn);
		DynamicContext *context2 = qc.createDynamicContext(txn, context);
		ReferenceMinder minder;

		NsItem item;
		while(sJoin.next(item)) {
			XmlDocument document;
			item.getDocID().fetchDocument(cont, &qc, document, &minder);
			env.results()->add(Value::create(((DbXmlFactoryImpl*)context2->getItemFactory())->
						   createNode(item.getIndexEntry(), document, context),
						   qc, /*lazyDocs*/true));
		}

		cout << env.results()->size() << " objects returned for algebra" << endl << endl;

#endif
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException &e) {
		throw CommandException(XMLChToUTF8(e.msg).str());
	}
	catch(const XQException &e) {
		throw CommandException(XMLChToUTF8(e.getError()).str());
	}
	catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
		throw CommandException(XMLChToUTF8(e.getMessage()).str());
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

