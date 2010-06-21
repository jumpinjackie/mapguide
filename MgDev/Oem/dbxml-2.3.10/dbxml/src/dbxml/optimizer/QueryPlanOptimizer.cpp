//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanOptimizer.cpp,v 1.3 2006/10/30 17:45:59 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "QueryPlanOptimizer.hpp"
#include "../dataItem/DbXmlContext.hpp"
#include "../dataItem/QueryPlanFunction.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlStep.hpp"

using namespace DbXml;
using namespace std;

ASTNode *QueryPlanOptimizer::optimizeQueryPlanFunction(QueryPlanFunction *item)
{
	NodeVisitingOptimizer::optimizeQueryPlanFunction(item);
	item->generateQueryPlan(mgr_, txn_, xpc_);
	return item;
}

ASTNode *QueryPlanOptimizer::optimizeFunction(XQFunction *item)
{
	NodeVisitingOptimizer::optimizeFunction(item);

	const XMLCh *funUri = item->getFunctionURI();
	const XMLCh *funName = item->getFunctionName();
	if(funUri == XQFunction::XMLChFunctionURI) {
		if(funName == DbXmlDocAvailable::name) {
			((DbXmlDocAvailable*)item)->generateQueryPlan(mgr_, txn_, xpc_);
		} else if(funName == FunctionContains::name) {
			((DbXmlFunContains*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		}
	}

	return item;
}

ASTNode *QueryPlanOptimizer::optimizeOperator(XQOperator *item)
{
	NodeVisitingOptimizer::optimizeOperator(item);

	if(item->getOperatorName() == GeneralComp::name) {
		((DbXmlGeneralComp*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlGeneralComp*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == Equals::name) {
		((DbXmlEquals*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlEquals*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == NotEquals::name) {
		((DbXmlNotEquals*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlNotEquals*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == LessThan::name) {
		((DbXmlLessThan*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlLessThan*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == LessThanEqual::name) {
		((DbXmlLessThanEqual*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlLessThanEqual*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == GreaterThan::name) {
		((DbXmlGreaterThan*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlGreaterThan*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	else if(item->getOperatorName() == GreaterThanEqual::name) {
		((DbXmlGreaterThanEqual*)item)->getArg0QPH().generateQueryPlan(mgr_, txn_, xpc_);
		((DbXmlGreaterThanEqual*)item)->getArg1QPH().generateQueryPlan(mgr_, txn_, xpc_);
	}

	return item;
}

ASTNode *QueryPlanOptimizer::optimizeDbXmlStep(DbXmlStep *item)
{
	item->generateQueryPlan(mgr_, txn_, xpc_);
	return item;
}

ASTNode *QueryPlanOptimizer::optimizeDbXmlCompare(DbXmlCompare *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	item->generateQueryPlan(mgr_, txn_, xpc_);
	return item;
}

ASTNode *QueryPlanOptimizer::optimizeDbXmlContains(DbXmlContains *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	item->generateQueryPlan(mgr_, txn_, xpc_);
	return item;
}
