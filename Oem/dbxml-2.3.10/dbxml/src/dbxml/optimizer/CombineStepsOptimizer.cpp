//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CombineStepsOptimizer.cpp,v 1.4.2.1 2007/01/12 18:56:21 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "../QueryPlan.hpp"
#include "../TransactedContainer.hpp"
#include "CombineStepsOptimizer.hpp"
#include "../dataItem/DbXmlContext.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../dataItem/LookupIndexFunction.hpp"
#include "../dataItem/QueryPlanFunction.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlNav.hpp"
#include "../dataItem/DbXmlFilter.hpp"
#include "../dataItem/DbXmlStep.hpp"

using namespace DbXml;
using namespace std;

void CombineStepsOptimizer::checkForExactCompare(DbXmlCompare *compare, ASTNode *&prev, DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(!compare->isQueryPlanExact()) {
		newNav->addStep(prev);
		DbXmlCompare *cmp = new (mm) DbXmlCompare(Join::SELF, compare, mm);
		cmp->setLocationInfo(compare);
		cmp->clearQueryPlanHolder();
		prev = cmp;
	}
}

void CombineStepsOptimizer::checkForExactContains(DbXmlContains *contains, ASTNode *&prev, DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(!contains->isQueryPlanExact()) {
		newNav->addStep(prev);
		DbXmlContains *cnt = new (mm) DbXmlContains(Join::SELF, contains, mm);
		cnt->setLocationInfo(contains);
		cnt->clearQueryPlanHolder();
		prev = cnt;
	}
}

void CombineStepsOptimizer::combineLookup(LookupIndex *index, ASTNode *&prev, ASTNode *current,
	DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	switch(current->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP): {
		DbXmlStep *step = (DbXmlStep*)current;
		if(step->getJoinType() == Join::SELF && step->isSuitableForLookupIndex()) {
			LookupIndex *combined = new (mm) LookupIndex(step, mm);
			combined->setLocationInfo(step);
			combined->intersectQueryPlan(index, mm);

			prev = combined;
			return;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		DbXmlCompare *compare = (DbXmlCompare*)current;
		if(compare->getJoinType() == Join::SELF && compare->isSuitableForLookupIndex()) {
			LookupIndex *combined = new (mm) LookupIndex(compare, mm);
			combined->setLocationInfo(compare);
			combined->intersectQueryPlan(index, mm);

			prev = combined;
			checkForExactCompare(compare, prev, newNav);
			return;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		DbXmlContains *contains = (DbXmlContains*)current;
		if(contains->getJoinType() == Join::SELF && contains->isSuitableForLookupIndex()) {
			LookupIndex *combined = new (mm) LookupIndex(contains, mm);
			combined->setLocationInfo(contains);
			combined->intersectQueryPlan(index, mm);

			prev = combined;
			checkForExactContains(contains, prev, newNav);
			return;
		}
		break;
	}
	default: break;
	}

	newNav->addStep(index);
	prev = current;
}

void CombineStepsOptimizer::combineStep(DbXmlStep *step, ASTNode *&prev, ASTNode *current,
	DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	switch(current->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		DbXmlCompare *compare = (DbXmlCompare*)current;
		if(compare->getJoinType() == Join::SELF && compare->getNodeTest() == 0 &&
			(compare->isSuitableForQPHCombine() ||
				(compare->getQueryPlan() == 0 && step->getQueryPlan() == 0))) {
			DbXmlCompare *combined = new (mm) DbXmlCompare(step->getJoinType(),
				(DbXmlNodeTest*)step->getNodeTest(), compare, mm);
			combined->setLocationInfo(compare);
			combined->intersectQueryPlan(step, mm);
			prev = combined;
			return;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		DbXmlContains *contains = (DbXmlContains*)current;
		if(contains->getJoinType() == Join::SELF && contains->getNodeTest() == 0 &&
			(contains->isSuitableForQPHCombine() ||
				(contains->getQueryPlan() == 0 && step->getQueryPlan() == 0))) {
			DbXmlContains *combined = new (mm) DbXmlContains(step->getJoinType(),
				(DbXmlNodeTest*)step->getNodeTest(), contains, mm);
			combined->setLocationInfo(contains);
			combined->intersectQueryPlan(step, mm);
			prev = combined;
			return;
		}
		break;
	}
	default:
		break;
	}

	newNav->addStep(step);
	prev = current;
}

void CombineStepsOptimizer::combineCompare(DbXmlCompare *compare, ASTNode *&prev, ASTNode *current,
	DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	switch(current->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		DbXmlCompare *compare2 = (DbXmlCompare*)current;
		if(compare2->getJoinType() == Join::SELF && compare2->isSuitableForLookupIndex() &&
			compare->isSuitableForLookupIndex() && Join::isSupported(compare->getJoinType())) {

			LookupIndex *combined = new (mm) LookupIndex(compare2, mm);
			combined->setLocationInfo(compare2);
			combined->intersectQueryPlan(compare, mm);

			Join *newJoin = new (mm) Join(compare->getJoinType(), /*returnArg*/true, combined, mm);
			newJoin->setLocationInfo(compare2);

			prev = newJoin;
			checkForExactCompare(compare, prev, newNav);
			checkForExactCompare(compare2, prev, newNav);
			return;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		DbXmlContains *contains = (DbXmlContains*)current;
		if(contains->getJoinType() == Join::SELF && contains->isSuitableForLookupIndex() &&
			compare->isSuitableForLookupIndex() && Join::isSupported(compare->getJoinType())) {

			LookupIndex *combined = new (mm) LookupIndex(contains, mm);
			combined->setLocationInfo(contains);
			combined->intersectQueryPlan(compare, mm);

			Join *newJoin = new (mm) Join(compare->getJoinType(), /*returnArg*/true, combined, mm);
			newJoin->setLocationInfo(contains);

			prev = newJoin;
			checkForExactCompare(compare, prev, newNav);
			checkForExactContains(contains, prev, newNav);
			return;
		}
		break;
	}
	default:
		break;
	}

	newNav->addStep(compare);
	prev = current;
}

void CombineStepsOptimizer::combineContains(DbXmlContains *contains, ASTNode *&prev, ASTNode *current,
	DbXmlNav *newNav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	switch(current->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		DbXmlCompare *compare = (DbXmlCompare*)current;
		if(compare->getJoinType() == Join::SELF && compare->isSuitableForLookupIndex() &&
			contains->isSuitableForLookupIndex() && Join::isSupported(contains->getJoinType())) {

			LookupIndex *combined = new (mm) LookupIndex(compare, mm);
			combined->setLocationInfo(compare);
			combined->intersectQueryPlan(contains, mm);

			Join *newJoin = new (mm) Join(contains->getJoinType(), /*returnArg*/true, combined, mm);
			newJoin->setLocationInfo(compare);

			prev = newJoin;
			checkForExactContains(contains, prev, newNav);
			checkForExactCompare(compare, prev, newNav);
			return;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		DbXmlContains *contains2 = (DbXmlContains*)current;
		if(contains2->getJoinType() == Join::SELF && contains2->isSuitableForLookupIndex() &&
			contains->isSuitableForLookupIndex() && Join::isSupported(contains->getJoinType())) {

			LookupIndex *combined = new (mm) LookupIndex(contains2, mm);
			combined->setLocationInfo(contains2);
			combined->intersectQueryPlan(contains, mm);

			Join *newJoin = new (mm) Join(contains->getJoinType(), /*returnArg*/true, combined, mm);
			newJoin->setLocationInfo(contains2);

			prev = newJoin;
			checkForExactContains(contains, prev, newNav);
			checkForExactContains(contains2, prev, newNav);
			return;
		}
		break;
	}
	default:
		break;
	}

	newNav->addStep(contains);
	prev = current;
}

void CombineStepsOptimizer::combine(ASTNode *&prev, ASTNode *current, DbXmlNav *newNav)
{
	if(current->getType() == ASTNode::CONTEXT_ITEM) {
		// Remove context item
		return;
	}

	if(prev == 0) {
		prev = current;
		return;
	}

	switch(prev->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP):
		combineStep((DbXmlStep*)prev, prev, current, newNav);
		break;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE):
		combineCompare((DbXmlCompare*)prev, prev, current, newNav);
		break;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS):
		combineContains((DbXmlContains*)prev, prev, current, newNav);
		break;
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX):
		combineLookup((LookupIndex*)prev, prev, current, newNav);
		break;
	default:
		newNav->addStep(prev);
		prev = current;
		break;
	}
}

ASTNode *CombineStepsOptimizer::optimizeDbXmlNav(DbXmlNav *nav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	ASTNode *prev = 0;
	DbXmlNav *newNav = new (mm) DbXmlNav(mm);

	DbXmlNav::Steps &steps = const_cast<DbXmlNav::Steps &>(nav->getSteps());
	for(DbXmlNav::Steps::iterator i = steps.begin(); i != steps.end(); ++i) {
		combine(prev, optimize(*i), newNav);
	}

	if(prev != 0) newNav->addStep(prev);

	if(newNav->getSteps().size() == 1) {
		return newNav->getSteps()[0];
	}

	newNav->recalculateSRC();
	return newNav;
}

ASTNode *CombineStepsOptimizer::dissolveFilter(DbXmlFilter *item, XPath2MemoryManager *mm)
{
	ASTNode *expr = const_cast<ASTNode*>(item->getArgument());

	switch(expr->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP):
		if(((DbXmlStep*)expr)->getJoinType() == Join::SELF)
			return expr;
		break;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE):
		if(((DbXmlCompare*)expr)->getJoinType() == Join::SELF)
			return expr;
		break;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS):
		if(((DbXmlContains*)expr)->getJoinType() == Join::SELF)
			return expr;
		break;
	case ((ASTNode::whichType)DbXmlASTNode::JOIN): {
		Join *join = (Join*)expr;
		if(join->getReturnArg() == false)
			return expr;
		else {
			Join *newJoin = new (mm) Join(join->getJoinType(), /*returnArg*/false,
				const_cast<ASTNode*>(join->getArgument()), mm);
			newJoin->setLocationInfo(join);
			return newJoin;
		}
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER):
		return expr;
	default: break;
	}

	return item;
}

ASTNode *CombineStepsOptimizer::optimizeDbXmlFilter(DbXmlFilter *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return dissolveFilter(item, mm);
}

