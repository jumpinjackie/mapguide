//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryExpression.cpp,v 1.46 2005/10/17 10:08:51 jsnelson Exp $
//

#include <sstream>

#include "DbXmlInternal.hpp"

#include <xquery/XQContext.hpp>
#include <xquery/XQEvaluator.hpp>
#include <xquery/XQQuery.hpp>
#include <xquery/XQException.hpp>

#include "QueryExpression.hpp"
#include "QueryContext.hpp"
#include "Results.hpp"
#include "optimizer/CollectionAndDocOptimizer.hpp"
#include "optimizer/QueryPlanGenerator.hpp"
#include "PrintDataItemTree.hpp"
#include "Manager.hpp"
#include "HighResTimer.hpp"

#include "UTF8.hpp"

using namespace DbXml;
using namespace std;

// QueryExpression
QueryExpression::QueryExpression(const std::string &query, XmlQueryContext &context, Transaction *txn)
	: query_(query),
	  context_(((QueryContext &)context).copy()),
	  xqContext_(((QueryContext &)getContext()).createXQContext(txn)),
	  expr_(0)
{
	((Manager &)((QueryContext &)getContext()).getManager())
		.log(Log::C_OPTIMIZER, Log::L_INFO, "Started parse");

	HighResTimer t;
	t.start();

	((QueryContext &)getContext()).setMinder(&minder_);

	try {
		scoped_ptr<Optimizer> optimizer(createOptimizer(txn));
		expr_ = XQEvaluator::parse(UTF8ToXMLCh(getQuery()).str(), getXQContext(), false);
		optimizer->startOptimize(expr_);
	}
	catch(const XQException &e) {
		ostringstream s;
		XMLChToUTF8 reason(e.getError());
		// TBD: see if xquery has module or file name for error
		s << "Error in XQuery expression";
		//if (file.str() && *(file.str()))
		// s << " in file " << file.str();
		s <<":\n\t";
		s << reason.str() << ", line " <<
			e.m_nLine << ", column " << e.m_nColumn;
		throw XmlException(XmlException::XPATH_PARSER_ERROR,
				   s.str());
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
}

std::string QueryExpression::getQueryPlan() const
{
	return DbXmlPrintDataItemTree::print(getCompiledExpression(), getXQContext());
}

XQContext *QueryExpression::getXQContext() const
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

	if(context.getEvaluationType() == XmlQueryContext::Eager) {
		return new ValueResults(new LazyDIResults(context, contextItem, *this, txn, flags));
	}
	else {
		return new LazyDIResults(context, contextItem, *this, txn, flags);
	}
}

Optimizer *QueryExpression::createOptimizer(Transaction *txn)
{
// 	OutputTree *before = new OutputTree("Initial tree", getXQContext());
// 	StaticResolver *sr = new StaticResolver(getXQContext(), before);
// 	OutputTree *afterStatic = new OutputTree("After Static Resolution", getXQContext(), sr);
// 	CollectionAndDocOptimizer *cad = new CollectionAndDocOptimizer(minder_, getXQContext(), afterStatic);
// 	OutputTree *afterCandD = new OutputTree("After Collection And Doc Optimisation", getXQContext(), cad);
// 	QueryPlanGenerator *qpGen = new QueryPlanGenerator(((QueryContext&)getContext()).getManager(), txn, getXQContext(), afterCandD);
// 	return new OutputTree("After Raw Query Plan Generation", getXQContext(), qpGen);

	StaticResolver *sr = new StaticResolver(getXQContext());
	CollectionAndDocOptimizer *cdo = new CollectionAndDocOptimizer(minder_, getXQContext(), sr);
	QueryPlanGenerator *qpg = new QueryPlanGenerator(((QueryContext&)getContext()).getManager(), txn, getXQContext(), cdo);
	return qpg;
}
