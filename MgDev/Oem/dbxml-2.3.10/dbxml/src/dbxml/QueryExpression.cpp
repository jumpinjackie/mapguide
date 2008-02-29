//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryExpression.cpp,v 1.68.2.1 2006/12/08 15:27:40 jsnelson Exp $
//

#include <sstream>

#include "DbXmlInternal.hpp"

#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>

#include "PrintDataItemTree.hpp"
#include "QueryExpression.hpp"
#include "QueryContext.hpp"
#include "Results.hpp"
#include "optimizer/QueryPlanGenerator.hpp"
#include "optimizer/CostBasedOptimizer.hpp"
#include "optimizer/ASTReplaceOptimizer.hpp"
#include "optimizer/ASTRewriteOptimizer.hpp"
#include "optimizer/QueryPlanOptimizer.hpp"
#include "optimizer/CombineStepsOptimizer.hpp"
#include "Manager.hpp"
#include "HighResTimer.hpp"
#include "dataItem/DbXmlContext.hpp"

#include "UTF8.hpp"

using namespace DbXml;
using namespace std;

// QueryExpression
QueryExpression::QueryExpression(const std::string &query, XmlQueryContext &context, Transaction *txn)
	: query_(query),
	  context_(context),
	  qec_(context_, /*debugging*/false),
	  xqContext_(((QueryContext &)context).createStaticContext(txn)),
	  expr_(0)
{
	((Manager &)((QueryContext &)getContext()).getManager())
		.log(Log::C_OPTIMIZER, Log::L_INFO, "Started parse");

	HighResTimer t;
	t.start();

	((QueryContext &)getContext()).setTransaction(txn);
	((QueryContext &)getContext()).setMinder(&minder_);
	((QueryContext &)context_).setQueryExecutionContext(&qec_);

	try {
		((QueryContext&)context_).startQuery();
		ScopedPtr<Optimizer> optimizer(createOptimizer(txn));
		expr_ = XQilla::parse(UTF8ToXMLCh(getQuery()).str(), XQilla::XQUERY, xqContext_, 0,
			XQilla::NO_STATIC_RESOLUTION | XQilla::NO_ADOPT_CONTEXT);
		optimizer->startOptimize(expr_);
	}
	catch(const QueryInterruptedException &ie) {
		delete expr_;
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(const QueryTimeoutException &ie) {
		delete expr_;
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		delete expr_;
		throw XmlException(XmlException::XPATH_PARSER_ERROR, e);
	}
	catch(...) {
		delete expr_;
		throw;
	}
	t.stop();

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		ostringstream s;
		s << "Finished parse, time taken = " << (t.durationInSeconds() * 1000) << "ms";
		((Manager &)((QueryContext &)getContext()).getManager())
			.log(Log::C_OPTIMIZER, Log::L_INFO, s);
	}
}

QueryExpression::~QueryExpression()
{
	delete expr_;
}

std::string QueryExpression::getQueryPlan() const
{
	return DbXmlPrintXQTree::print(getCompiledExpression(), xqContext_);
}

DbXmlContext *QueryExpression::getDynamicContext() const
{
	return xqContext_;
}

XQQuery *QueryExpression::getCompiledExpression() const
{
	DBXML_ASSERT(expr_ != 0);
	return expr_;
}

Results *QueryExpression::execute(Transaction *txn, Value *contextItem, XmlQueryContext &context, u_int32_t flags)
{
	if (contextItem && (contextItem->getType() == XmlValue::BINARY))
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlQueryExpression::execute: context item cannot be a binary value");

	// NOTE: LazyDIResults::next() catches all query exceptions.
	// If Eager resulst are ever changed to not be implemented in terms
	// of Lazy results, that code needs to catch exceptions as well
	// The timer, however, is never used for purely lazy queries, as
	// there is no control over iteration.
	Results *ret = 0;
	if(context.getEvaluationType() == XmlQueryContext::Eager) {
		ret = new ValueResults(
			new LazyDIResults(
				context, contextItem,
				*this, txn, flags));
	} else {
		ret = new LazyDIResults(context, contextItem, *this, txn, flags);
	}
	return ret;
}

Optimizer *QueryExpression::createOptimizer(Transaction *txn)
{
	StaticResolver *sr = new StaticResolver(xqContext_);
	ASTReplaceOptimizer *rep = new ASTReplaceOptimizer(minder_, xqContext_, sr);
	StaticTyper *st = new StaticTyper(xqContext_, rep);
	QueryPlanGenerator *qpg = new QueryPlanGenerator(xqContext_, st);
	QueryPlanOptimizer *qpo = new QueryPlanOptimizer(((QueryContext&)getContext()).getManager(), txn, xqContext_, qpg);
	ASTRewriteOptimizer *rew = new ASTRewriteOptimizer(xqContext_, qpo);
	CombineStepsOptimizer *com = new CombineStepsOptimizer(xqContext_, rew);
	CostBasedOptimizer *cbo = new CostBasedOptimizer(((QueryContext&)getContext()).getManager(), txn, xqContext_, com);
	DocOrderOptimizer *doo = new DocOrderOptimizer(xqContext_, cbo);
	StaticTyper *st2 = new StaticTyper(xqContext_, doo);
	return st2;
}
