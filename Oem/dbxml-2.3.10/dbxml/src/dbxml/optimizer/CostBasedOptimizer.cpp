//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CostBasedOptimizer.cpp,v 1.17.2.1 2007/02/02 18:59:58 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "../QueryPlan.hpp"
#include "../TransactedContainer.hpp"
#include "CostBasedOptimizer.hpp"
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

bool CostBasedOptimizer::reverseDbXmlStep(DbXmlStep *step, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	DbXmlNodeTest *nt = (DbXmlNodeTest*)step->getNodeTest();

	if(reverse != 0) {
		// Skip the step if it's self::node(), which is a noop
		if(Join::inverse(axis) != Join::SELF || nt != 0) {
			// Reverse the step, and add it to the reverse navigation,
			DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), step, mm);
			newStep->setLocationInfo(step);
			reverse->addStep(newStep);
		}
		axis = step->getJoinType();
	}

	return true;
}

bool CostBasedOptimizer::reverseDbXmlCompare(DbXmlCompare *compare, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(reverse != 0) {
		// Reverse the step, and add it to the reverse navigation
		DbXmlCompare *newCompare = new (mm) DbXmlCompare(Join::inverse(axis), compare, mm);
		newCompare->setLocationInfo(compare);
		reverse->addStep(newCompare);

		axis = compare->getJoinType();
	}

	return true;
}

bool CostBasedOptimizer::reverseDbXmlContains(DbXmlContains *contains, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(reverse != 0) {
		// Reverse the step, and add it to the reverse navigation
		DbXmlContains *newContains = new (mm) DbXmlContains(Join::inverse(axis), contains, mm);
		newContains->setLocationInfo(contains);
		reverse->addStep(newContains);

		axis = contains->getJoinType();
	}

	return true;
}

bool CostBasedOptimizer::reverseDbXmlFilter(DbXmlFilter *filter, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(reverse != 0) {
		// Reverse the step, and add it to the reverse navigation
		if(Join::inverse(axis) != Join::SELF) {
			DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
			newStep->setLocationInfo(filter);
			reverse->addStep(newStep);
		}
		reverse->addStep(filter);

		axis = Join::SELF;
	}

	return true;
}

bool CostBasedOptimizer::reverseJoin(Join *join, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(reverse != 0) {
		if(join->getReturnArg()) {
			// Reverse the step, and add it to the reverse navigation
			Join *newJoin = new (mm) Join(Join::inverse(axis), join, mm);
			newJoin->setLocationInfo(join);
			reverse->addStep(newJoin);

			axis = join->getJoinType();
		} else {
			// Add a step to provide our inverse axis
			if(Join::inverse(axis) != Join::SELF) {
				DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
				newStep->setLocationInfo(join);
				reverse->addStep(newStep);
			}
			reverse->addStep(join);

			axis = Join::SELF;
		}
	}

	return true;
}

bool CostBasedOptimizer::reverseLookupIndex(LookupIndex *index, Join::Type &axis, DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(reverse != 0) {
		// Reverse the step, and add it to the reverse navigation
		if(!Join::isSupported(Join::inverse(axis))) {
			// Fake the join with a DbXmlStep and a self join
			DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
			newStep->setLocationInfo(index);
			reverse->addStep(newStep);

			axis = Join::SELF;
		}

		Join *newJoin = new (mm) Join(Join::inverse(axis), /*returnArg*/true, index, mm);
		newJoin->setLocationInfo(index);
		reverse->addStep(newJoin);

		axis = Join::NONE;
	}

	return true;
}

bool CostBasedOptimizer::addQPFToLookup(QueryPlanFunction *qpf, LookupIndex *index)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	if(qpf->getQueryPlan() == 0 || qpf->getContainer() == 0) {
		return false;
	} else if(qpf->getFunctionType() == QueryPlanFunction::COLLECTION) {
		// We don't need to add a DocumentQP
	} else if(qpf->getQueryPlan()->getType() != QueryPlan::DOCUMENT) {
		return false;
	} else if(index != 0) {
		DocumentQP *qp = (DocumentQP*)qpf->getQueryPlan();

		// Add a DocumentQP to the QueryPlan for the LookupIndex
		DocumentQP *docQP = new (mm) DocumentQP(qp->getValue().getValue(),
			index->getQueryPlan()->copy(mm), mm);
		index->setQueryPlan(docQP);
	}

	qpf->removeNonConstant(mgr_, txn_, xpc_);
	return true;

	// TBD Can we use this? It's faster but it breaks XQTS - jpcs

// 	if(qpf->getContainer() != 0 && qpf->getQueryPlan() != 0 && index != 0 &&
// 		index->getQueryPlan()->getType() != QueryPlan::INTERSECT_DOCS) {

// 		index->intersectDocQueryPlan(qpf, mm);
// 		return true;
// 	}

// 	return false;
}

bool CostBasedOptimizer::reverseQueryPlanFunction(QueryPlanFunction *qpf, Join::Type &axis, LookupIndex *index,
	DbXmlNav *&reverse)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	bool docQueryPlanAdded = reverse == 0 ? false : addQPFToLookup(qpf, index);

	// Everything is a descendant of the root node, so we don't
	// need an extra step in that case
	bool stepAdded = false;
	if(axis == Join::DESCENDANT || axis == Join::DESCENDANT_OR_SELF) {
		stepAdded = true;
	} else if(reverse != 0 && !Join::isSupported(Join::inverse(axis))) {
		DbXmlNodeTest *nt = new (mm) DbXmlNodeTest(Node::document_string);
		DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), nt, mm);
		newStep->setLocationInfo(qpf);
		reverse->addStep(newStep);

		stepAdded = true;
		axis = Join::SELF;
	}

	if(reverse != 0 && (!docQueryPlanAdded || !stepAdded)) {
		Join *newJoin = new (mm) Join(Join::inverse(axis), /*returnArg*/true, qpf, mm);
		newJoin->setLocationInfo(qpf);
		reverse->addStep(newJoin);
	}

	axis = Join::NONE;
	return true;
}

bool CostBasedOptimizer::reverseVariable(XQVariable *var, Join::Type &axis, DbXmlNav *&reverse)
{
	// TBD enable variable reversing - jpcs

	return false;

// 	XPath2MemoryManager *mm = xpc_->getMemoryManager();

// 	if(reverse != 0) {
// 		// Reverse the step, and add it to the reverse navigation
// 		if(!Join::isSupported(Join::inverse(axis))) {
// 			// Fake the join with a DbXmlStep and a self join
// 			DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
// 			newStep->setLocationInfo(var);
// 			reverse->addStep(newStep);

// 			axis = Join::SELF;
// 		}

// 		Join *newJoin = new (mm) Join(Join::inverse(axis), /*returnArg*/true, var, mm);
// 		newJoin->setLocationInfo(var);
// 		reverse->addStep(newJoin);

// 		axis = Join::NONE;
// 	}

// 	return true;
}

bool CostBasedOptimizer::reverseASTNode(ASTNode *ast, Join::Type &axis, LookupIndex *index,
	DbXmlNav *&reverse)
{
	switch(ast->getType()) {
	case ASTNode::VARIABLE:
		return reverseVariable((XQVariable*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP):
		return reverseDbXmlStep((DbXmlStep*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE):
		return reverseDbXmlCompare((DbXmlCompare*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS):
		return reverseDbXmlContains((DbXmlContains*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER):
		return reverseDbXmlFilter((DbXmlFilter*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX):
		return reverseLookupIndex((LookupIndex*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION):
		return reverseQueryPlanFunction((QueryPlanFunction*)ast, axis, index, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::JOIN):
		return reverseJoin((Join*)ast, axis, reverse);
	default: break;
	}

	return false;
}

bool CostBasedOptimizer::isASTNodeReversible(ASTNode *ast)
{
	Join::Type axis = Join::NONE;
	DbXmlNav *reverse = 0;

	switch(ast->getType()) {
	case ASTNode::VARIABLE:
		return reverseVariable((XQVariable*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP):
		return reverseDbXmlStep((DbXmlStep*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE):
		return reverseDbXmlCompare((DbXmlCompare*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS):
		return reverseDbXmlContains((DbXmlContains*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER):
		return reverseDbXmlFilter((DbXmlFilter*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX):
		return reverseLookupIndex((LookupIndex*)ast, axis, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION):
		return reverseQueryPlanFunction((QueryPlanFunction*)ast, axis, 0, reverse);
	case ((ASTNode::whichType)DbXmlASTNode::JOIN):
		return reverseJoin((Join*)ast, axis, reverse);
	default: break;
	}

	return false;
}

static inline QueryPlanHolder *getQueryPlanHolder(ASTNode *ast, Join::Type &axis)
{
	switch(ast->getType()) {
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP):
		axis = ((DbXmlStep*)ast)->getJoinType();
		return (DbXmlStep*)ast;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE):
		axis = ((DbXmlCompare*)ast)->getJoinType();
		return (DbXmlCompare*)ast;
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS):
		axis = ((DbXmlContains*)ast)->getJoinType();
		return (DbXmlContains*)ast;
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX):
		axis = Join::NONE;
		return (LookupIndex*)ast;
	case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION):
		axis = Join::NONE;
		return (QueryPlanFunction*)ast;
	default: break;
	}

	axis = Join::NONE;
	return 0;
}

static inline LookupIndex *findLookupIndex(ASTNode *arg)
{
	ASTNode *result = arg;

	while(true) {
		switch(result->getType()) {
// 		case ASTNode::DOCUMENT_ORDER: {
// 			result = const_cast<ASTNode*>(((XQDocumentOrder*)result)->getExpression());
// 			break;
// 		}
		case ((ASTNode::whichType)DbXmlASTNode::DBXML_NAV): {
			result = const_cast<ASTNode*>(((DbXmlNav*)result)->getSteps()[0]);
			break;
		}
		case ((ASTNode::whichType)DbXmlASTNode::JOIN): {
			result = const_cast<ASTNode*>(((Join*)arg)->getArgument());
			break;
		}
		case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX): {
			return (LookupIndex*)result;
		}
		default: {
			return 0;
		}
		}
	}

	return 0;
}

DbXmlNav::Steps::reverse_iterator CostBasedOptimizer::findBestIndex(DbXmlNav::Steps::reverse_iterator start,
	DbXmlNav::Steps::reverse_iterator end, bool findJoin)
{
	DbXmlNav::Steps::reverse_iterator found_it = end;
	QueryPlan::Cost found_cost;

	for(DbXmlNav::Steps::reverse_iterator i = start; i != end; ++i) {
		Join::Type qph_axis;
		QueryPlan::Cost cost;
		QueryPlanHolder *qph = getQueryPlanHolder(*i, qph_axis);
		if(qph != 0 && qph->isSuitableForLookupIndex()) {

			if(qph->costQueryPlan(cost, xpc_)) {
				if((found_it == end || cost.compare(found_cost) < 0)) {
					if(qph_axis != Join::NONE) {
						found_it = i;
						found_cost = cost;
					} else {
						// Act like we didn't find a best index
						return end;
					}
				}
			}
		} else if(findJoin && (*i)->getType() == (ASTNode::whichType)DbXmlASTNode::JOIN) {
			LookupIndex *index = findLookupIndex(*i);

			if(index != 0 && index->isSuitableForLookupIndex()) {

				if(index->costQueryPlan(cost, xpc_)) {
					if((found_it == end || cost.compare(found_cost) < 0)) {
						found_it = i;
						found_cost = cost;
					}
				}
			}
		}

		if(found_it != end &&
			(*found_it)->getType() != (ASTNode::whichType)DbXmlASTNode::JOIN &&
			!isASTNodeReversible(*i)) {
			// Abort the reversal, we don't know how to
			// reverse this step
			found_it = end;
			found_cost = QueryPlan::Cost();
		}
	}

	return found_it;
}

static LookupIndex *createLookupIndex(ASTNode *item, DbXmlNav *forward, XPath2MemoryManager *mm)
{
	Join::Type axis;
	QueryPlanHolder *qph = getQueryPlanHolder(item, axis);

	// Create the new index lookup
	LookupIndex *index = new (mm) LookupIndex(qph, mm);
	index->setLocationInfo(item);
	forward->addStep(index);

	if(!qph->isQueryPlanExact()) {
		switch(item->getType()) {
		case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
			DbXmlCompare *oldCmp = (DbXmlCompare*)item;
			DbXmlCompare *cmp = new (mm) DbXmlCompare(oldCmp->getOperation(),
				oldCmp->isGeneralComp(), const_cast<ASTNode*>(oldCmp->getArgument()), mm);
			cmp->setLocationInfo(oldCmp);
			forward->addStep(cmp);
			break;
		}
		case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
			DbXmlContains *oldCnt = (DbXmlContains*)item;
			DbXmlContains *cnt = new (mm)
				DbXmlContains(const_cast<ASTNode*>(oldCnt->getArgument()), mm);
			cnt->setLocationInfo(oldCnt);
			forward->addStep(cnt);
			break;
		}
		default:
			DBXML_ASSERT(false);
			break;
		}
	}

	return index;
}

ASTNode *CostBasedOptimizer::optimizeDbXmlNav(DbXmlNav *nav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	NodeVisitingOptimizer::optimizeDbXmlNav(nav);
	if(nav->getSteps().size() == 1) {
		return nav->getSteps()[0];
	}

	DbXmlNav::Steps &args = const_cast<DbXmlNav::Steps &>(nav->getSteps());
	DbXmlNav::Steps::reverse_iterator found_it = findBestIndex(args.rbegin(), args.rend(), /*findJoin*/false);

	if(found_it != args.rend()) {
		// Create a navigation for the forward steps,
		// and add them
		DbXmlNav *forward = new (mm) DbXmlNav(mm);
		forward->setLocationInfo(nav);

		Join::Type axis;
		QueryPlanHolder *qph = getQueryPlanHolder(*found_it, axis);
		if(qph != 0) {
			LookupIndex *index = createLookupIndex(*found_it, forward, mm);

			// Create a filter with the reverse steps
			DbXmlNav::Steps::reverse_iterator i = found_it;
			++i;
			if(i != args.rend()) {
				DbXmlNav *reverse = new (mm) DbXmlNav(mm);
				reverse->setLocationInfo(nav);

				for(; i != args.rend(); ++i) {
					if(reverseASTNode(*i, axis, index, reverse)) {
						if(axis == Join::NONE) break;
					} else {
						DBXML_ASSERT(false);
					}
				}

				if(!reverse->getSteps().empty()) {
					reverse->recalculateSRC();
					DbXmlFilter *filter = new (mm) DbXmlFilter(reverse, mm);
					filter->setLocationInfo(nav);
					forward->addStep(filter);
				}
			}

			if(axis != Join::NONE) {
				// TBD Add a join against the context item - jpcs
			}
		} else {
			Join *join = (Join*)*found_it;
			forward->addStep(const_cast<ASTNode*>(join->getArgument()));

			// Create a new join with the steps before found_it
			ASTNode *joinArg;
			DbXmlNav::Steps::reverse_iterator i = found_it;
			++i;
			if(i != args.rend()) {
				DbXmlNav *joinNav = new (mm) DbXmlNav(mm);
				joinNav->setLocationInfo(join);

				for(; i != args.rend(); ++i) {
					joinNav->addStepFront(*i);

					if((*i)->getType() == (ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION) {
						addQPFToLookup((QueryPlanFunction*)*i, findLookupIndex(join));
					}
				}

				joinNav->recalculateSRC();
				joinArg = joinNav;
			} else {
				XQContextItem *ci = new (mm) XQContextItem(mm);
				ci->setLocationInfo(join);
				joinArg = ci;
			}

			Join *newJoin = new (mm) Join(Join::inverse(join->getJoinType()),
				!join->getReturnArg(), joinArg, mm);
			newJoin->setLocationInfo(join);
			forward->addStep(newJoin);
		}

		if(axis == Join::NONE) {
			// Add the forward steps
			DbXmlNav::Steps::reverse_iterator i = found_it;
			while(i != args.rbegin()) {
				--i;
				forward->addStep(*i);
			}

			forward->recalculateSRC();

			NodeVisitingOptimizer::optimizeDbXmlNav(forward);
			if(forward->getSteps().size() == 1) {
				return forward->getSteps()[0];
			}

			return forward;
		}
	}

	return nav;
}

static DbXmlNav::Steps::reverse_iterator findLastJoin(DbXmlNav::Steps::reverse_iterator start,
	DbXmlNav::Steps::reverse_iterator end)
{
	for(DbXmlNav::Steps::reverse_iterator i = start; i != end; ++i) {
		if((*i)->getType() == (ASTNode::whichType)DbXmlASTNode::JOIN) {
			return i;
		}
	}

	return end;
}

ASTNode *CostBasedOptimizer::optimizeDbXmlFilter(DbXmlFilter *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	DbXmlNav::Steps args = DbXmlNav::Steps(XQillaAllocator<ASTNode*>(mm));
	if(item->getArgument()->getType() == (ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
		DbXmlNav *nav = (DbXmlNav*)item->getArgument();
		NodeVisitingOptimizer::optimizeDbXmlNav(nav);
		args = nav->getSteps();
	} else {
		args.push_back(const_cast<ASTNode*>(item->getArgument()));
	}

	DbXmlNav::Steps::reverse_iterator found_it = findBestIndex(args.rbegin(), args.rend(), /*findJoin*/true);

	if(found_it == args.rend()) {
		found_it = findLastJoin(args.rbegin(), args.rend());
	}

	if(found_it != args.rend()) {
		// Create a navigation to hold the reversed steps
		DbXmlNav *reverse = new (mm) DbXmlNav(mm);
		reverse->setLocationInfo(item);

		DbXmlNav::Steps::reverse_iterator rev_begin = found_it;

		Join::Type axis;
		LookupIndex *index;
		QueryPlanHolder *qph = getQueryPlanHolder(*found_it, axis);
		if(qph != 0) {
			index = createLookupIndex(*found_it, reverse, mm);

			// Create a filter with the forward steps
			DbXmlNav::Steps::reverse_iterator i;
			if(found_it != args.rbegin()) {
				DbXmlNav *forward = new (mm) DbXmlNav(mm);
				forward->setLocationInfo(item);

				i = found_it;
				while(i != args.rbegin()) {
					--i;
					forward->addStep(*i);
				}

				forward->recalculateSRC();
				DbXmlFilter *filter = new (mm) DbXmlFilter(forward, mm);
				filter->setLocationInfo(item);
				reverse->addStep(filter);
			}
		} else {
			Join *join = (Join*)*found_it;
			index = findLookupIndex(join);
			reverse->addStep(const_cast<ASTNode*>(join->getArgument()));
			axis = join->getJoinType();

			DbXmlNav::Steps::reverse_iterator i;
			if(!join->getReturnArg()) {
				// Add a single step as the inverse of the join
				i = found_it;
				++i;
				if(i != args.rend()) {
					if(!reverseASTNode(*i, axis, index, reverse)) {
						DBXML_ASSERT(false);
					}
					rev_begin = i;
				} else {
					DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
					newStep->setLocationInfo(item);
					reverse->addStep(newStep);
					axis = Join::SELF;
				}
			}

			// Create a filter with the forward steps
			if(found_it != args.rbegin()) {
				DbXmlNav *forward = new (mm) DbXmlNav(mm);
				forward->setLocationInfo(item);

				i = found_it;
				while(i != args.rbegin()) {
					--i;
					forward->addStep(*i);
				}

				forward->recalculateSRC();
				DbXmlFilter *filter = new (mm) DbXmlFilter(forward, mm);
				filter->setLocationInfo(item);
				reverse->addStep(filter);
			}
		}

		// Add the reverse steps
		if(rev_begin != args.rend()) {
			DbXmlNav::Steps::reverse_iterator i = rev_begin;
			for(++i; i != args.rend() && axis != Join::NONE; ++i) {
				if(reverseASTNode(*i, axis, index, reverse)) {
					if(axis == Join::NONE) break;
				} else {
					DBXML_ASSERT(false);
				}
			}
		}

		if(!Join::isSupported(axis)) {
			// Fake the join with a DbXmlStep and a self join
			DbXmlStep *newStep = new (mm) DbXmlStep(Join::inverse(axis), mm);
			newStep->setLocationInfo(item);
			reverse->addStep(newStep);

			axis = Join::SELF;
		}

		Join *join = new (mm) Join(axis, /*returnArg*/false, reverse, mm);
		join->setLocationInfo(item);

		return NodeVisitingOptimizer::optimizeJoin(join);
	}

	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return CombineStepsOptimizer::dissolveFilter(item, mm);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ASTNode *DocOrderOptimizer::optimizeDbXmlNav(DbXmlNav *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	NodeVisitingOptimizer::optimizeDbXmlNav(item);

	if(!item->getSortAdded()) {
		item->setSortAdded(true);
		ASTNode *result = new (mm) XQDocumentOrder(item, mm);
		result->setLocationInfo(item->getSteps().back());
		return result;
	}

	return item;
}

ASTNode *DocOrderOptimizer::optimizeDbXmlStep(DbXmlStep *item)
{
	// TBD enable this? - jpcs

// 	if(Join::isSupported(item->getJoinType()) && item->isSuitableForLookupIndex()) {
// 		XPath2MemoryManager *mm = xpc_->getMemoryManager();

// 		LookupIndex *index = new (mm) LookupIndex(item, mm);
// 		index->setLocationInfo(item);

// 		DbXmlNav *joinNav = new (mm) DbXmlNav(mm);
// 		joinNav->setLocationInfo(item);
// 		joinNav->addStep(index);
// 		joinNav->recalculateSRC();

// 		Join *join = new (mm) Join(item->getJoinType(), /*returnArg*/true, joinNav, mm);
// 		join->setLocationInfo(item);

// 		return optimize(join);
// 	}

	return item;
}
