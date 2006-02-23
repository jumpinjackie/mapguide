//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DebugCommand.cpp,v 1.19 2005/10/12 12:20:21 jsnelson Exp $
//

#include "DebugCommand.hpp"

#include <xquery/XQContext.hpp>
#include <xquery/XQEvaluator.hpp>
#include <xquery/XQQuery.hpp>

#include <db_cxx.h>
#include "ScopedDbt.hpp"
#include "ID.hpp"

#include "UTF8.hpp"
#include "QueryContext.hpp"
#include "PrintDataItemTree.hpp"
#include "OperationContext.hpp"
#include "Document.hpp"
#include "TransactedContainer.hpp"
#include "optimizer/CollectionAndDocOptimizer.hpp"
#include "optimizer/QueryPlanGenerator.hpp"

#include <pathan/internal/XPath2MemoryManagerImpl.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanExpression.hpp>
#include <pathan/internal/dom-extensions/PathanExpressionImpl.hpp>
#include <pathan/PathanEngine.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/exceptions/DSLException.hpp>

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

void DebugCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}

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
			ID id;
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
		if(env.txn()) {
			((QueryContext&)env.context()).setTransaction(*env.txn());
		}
		else {
			((QueryContext&)env.context()).setTransaction(0);
		}

		XQContext *context = ((QueryContext&)env.context()).createXQContext(env.txn() ? *env.txn() : 0);

		XQQuery *query = XQEvaluator::parse(UTF8ToXMLCh(args[1]).str(), context, false);

		OutputTree *before = new OutputTree("Initial tree", context);
		StaticResolver *sr = new StaticResolver(context, before);
		OutputTree *afterStatic = new OutputTree("After Static Resolution", context, sr);
		CollectionAndDocOptimizer *cad = new CollectionAndDocOptimizer(minder, context, afterStatic);
		OutputTree *afterCandD = new OutputTree("After Collection And Doc Optimisation", context, cad);
		QueryPlanGenerator *qpGen = new QueryPlanGenerator(((QueryContext&)env.context()).getManager(), 0, context, afterCandD);
		OutputTree optimizer("After Raw Query Plan Generation", context, qpGen);

		optimizer.startOptimize(query);

		cout << "Global Query Plan: " << endl;
		if(qpGen->getQueryPlan() != 0) cout << qpGen->getQueryPlan()->toString(false) << endl;

		XQContext *context2 = ((QueryContext&)env.context()).createDynamicXQContext(env.txn() ? *env.txn() : 0, context);
		Result result(query->evaluate(context2));

		cout << "Results:\n";
		Item::Ptr item;
		while((item = result.next(context2)) != NULLRCP) {
			cout << XMLChToUTF8(item->asString(context2)).str() << endl;
		}
#endif
	}
	catch (const PathanException &e) {
		throw CommandException(XMLChToUTF8(e.getString()).str());
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException &e) {
		throw CommandException(XMLChToUTF8(e.msg).str());
	}
	catch(const DSLException &e) {
		throw CommandException(XMLChToUTF8(e.getError()).str());
	}
	catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) {
		throw CommandException(XMLChToUTF8(e.getMessage()).str());
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

