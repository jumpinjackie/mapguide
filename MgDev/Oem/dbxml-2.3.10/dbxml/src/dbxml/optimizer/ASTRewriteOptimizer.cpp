//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ASTRewriteOptimizer.cpp,v 1.5 2006/10/30 17:45:58 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "ASTRewriteOptimizer.hpp"
#include "../dataItem/DbXmlNav.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlFilter.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlContext.hpp"

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/Intersect.hpp>

using namespace DbXml;
using namespace std;

ASTNode *ASTRewriteOptimizer::optimize(ASTNode *item)
{
	ancestors_.push_back(ParentInfo(item, false));
	ASTNode *result = NodeVisitingOptimizer::optimize(item);
	ancestors_.pop_back();
	return result;
}

static void optimizePred(ASTNode *&pred, ASTNode *&expr, XPath2MemoryManager *mm)
{
	if(pred->getType() == ASTNode::OPERATOR &&
		((XQOperator*)pred)->getOperatorName() == And::name) {
		And *andOp = (And*)pred;
		unsigned int i = 0;
		while(i < andOp->getNumArgs()) {
			ASTNode *arg = andOp->getArgument(i);
			optimizePred(arg, expr, mm);
			if(arg == 0) {
				andOp->removeArgument(i);
			} else {
				andOp->setArgument(i, arg);
				++i;
			}
		}

		if(andOp->getNumArgs() == 1) {
			pred = andOp->getArgument(0);
		} else if(andOp->getNumArgs() == 0) {
			pred = 0;
		}
	} else {
		DbXmlNav *nav = new (mm) DbXmlNav(mm);
		nav->setLocationInfo(pred);
		if(expr->getType() != ASTNode::CONTEXT_ITEM)
			nav->addStep(expr);

		DbXmlFilter *filter = new (mm) DbXmlFilter(pred, mm);
		filter->setLocationInfo(pred);
		nav->addStep(filter);

		nav->recalculateSRC();

		expr = nav;
		pred = 0;
	}
}

ASTNode *ASTRewriteOptimizer::optimizePredicate(XQPredicate *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	ancestors_.back().second = true;
	item->setPredicate(optimize(const_cast<ASTNode *>(item->getPredicate())));
	ancestors_.back().second = false;

	// Look for predicates that we can change into DbXmlFilter steps.
	// We can do this if the predicate is not (potentially) a single
	// numeric constant, and it doesn't use the context size or context
	// position (as this rewrite messes up the context position and size).

	ASTNode *expr = const_cast<ASTNode*>(item->getExpression());
	ASTNode *pred = const_cast<ASTNode*>(item->getPredicate());

	if(!pred->getStaticResolutionContext().getStaticType().containsType(StaticType::NUMERIC_TYPE) &&
		!pred->getStaticResolutionContext().isContextPositionUsed() &&
		!pred->getStaticResolutionContext().isContextSizeUsed() &&
		expr->getStaticResolutionContext().getStaticType().isType(StaticType::NODE_TYPE)) {
		
		optimizePred(pred, expr, mm);

		if(pred == 0) {
			return expr;
		} else {
			item->setExpression(expr);
			item->setPredicate(pred);
		}
	}

	return item;
}

GeneralComp::ComparisonOperation oppositeOperation(GeneralComp::ComparisonOperation op)
{
	switch(op) {
	case GeneralComp::EQUAL: {
		return GeneralComp::EQUAL;
	}
	case GeneralComp::NOT_EQUAL: {
		return GeneralComp::NOT_EQUAL;
	}
	case GeneralComp::LESS_THAN: {
		return GeneralComp::GREATER_THAN;
	}
	case GeneralComp::LESS_THAN_EQUAL: {
		return GeneralComp::GREATER_THAN_EQUAL;
	}
	case GeneralComp::GREATER_THAN: {
		return GeneralComp::LESS_THAN;
	}
	case GeneralComp::GREATER_THAN_EQUAL: {
		return GeneralComp::LESS_THAN_EQUAL;
	}
	default:
		DBXML_ASSERT(0);
	}
	return (GeneralComp::ComparisonOperation)-1;
}

static ASTNode *findNodeProducer(ASTNode *arg, XQCastAs *&cast)
{
	cast = 0;
	while(true) {
		switch(arg->getType()) {
		case ASTNode::TREAT_AS: {
			arg = const_cast<ASTNode*>(((XQTreatAs*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_ANY_URI: {
			arg = const_cast<ASTNode*>(((XQPromoteAnyURI*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_NUMERIC: {
			arg = const_cast<ASTNode*>(((XQPromoteNumeric*)arg)->getExpression());
			break;
		}
		case ASTNode::PROMOTE_UNTYPED: {
			arg = const_cast<ASTNode*>(((XQPromoteUntyped*)arg)->getExpression());
			break;
		}
		case ASTNode::ATOMIZE: {
			arg = const_cast<ASTNode*>(((XQAtomize*)arg)->getExpression());
			break;
		}
		case ASTNode::CAST_AS: {
			if(cast != 0) {
				// Can't handle more than one cast
				cast = 0;
				return 0;
			}
			cast = (XQCastAs*)arg;
			arg = const_cast<ASTNode*>(((XQCastAs*)arg)->getExpression());
			break;
		}
		default: {
			if(arg->getStaticResolutionContext().getStaticType().isType(StaticType::NODE_TYPE))
				return arg;
			return 0;
		}
		}
	}

	return 0;
}

static inline void swapASTNode(ASTNode *&a, ASTNode *&b)
{
	ASTNode *tmp = a;
	a = b;
	b = tmp;
}

static inline void swapCast(XQCastAs *&a, XQCastAs *&b)
{
	XQCastAs *tmp = a;
	a = b;
	b = tmp;
}

static inline void swapQPH(QueryPlanHolder *&a, QueryPlanHolder *&b)
{
	QueryPlanHolder *tmp = a;
	a = b;
	b = tmp;
}

ASTNode *ASTRewriteOptimizer::createDbXmlIntersect(Intersect *item)
{
	ASTNode *arg0 = item->getArgument(0);
	ASTNode *arg1 = item->getArgument(1);

	XQCastAs *arg0Cast, *arg1Cast;
	ASTNode *arg0Nodes = findNodeProducer(arg0, arg0Cast);
	ASTNode *arg1Nodes = findNodeProducer(arg1, arg1Cast);

	if(arg0Nodes == 0 || arg0Cast != 0 ||
		arg1->getStaticResolutionContext().areContextFlagsUsed()) {
		// Swap the arguments
		swapASTNode(arg0, arg1);
		swapASTNode(arg0Nodes, arg1Nodes);
		swapCast(arg0Cast, arg1Cast);
	}

	if(arg0Nodes != 0 && arg0Cast == 0 &&
		!arg1->getStaticResolutionContext().areContextFlagsUsed()) {
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		DbXmlNav *nav = new (mm) DbXmlNav(mm);
		nav->setLocationInfo(item);
		if(arg0Nodes->getType() != ASTNode::CONTEXT_ITEM) {
			nav->addStep(arg0Nodes);
		}

		Join *join = new (mm) Join(Join::SELF, /*returnArg*/false, arg1, mm);
		join->setLocationInfo(item);

		nav->addStep(join);
		nav->recalculateSRC();

		return nav;
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::createDbXmlCompare(XQOperator *item, GeneralComp::ComparisonOperation op,
	bool generalComp, QueryPlanHolder *arg0qph, QueryPlanHolder *arg1qph)
{
	bool insidePredicate = ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second;

	if(!generalComp && !insidePredicate) return item;

	ASTNode *arg0 = item->getArgument(0);
	ASTNode *arg1 = item->getArgument(1);

	XQCastAs *arg0Cast, *arg1Cast;
	ASTNode *arg0Nodes = findNodeProducer(arg0, arg0Cast);
	ASTNode *arg1Nodes = findNodeProducer(arg1, arg1Cast);

	if(arg1qph->getQueryPlan() != 0 && arg1Nodes != 0 &&
		!arg0->getStaticResolutionContext().areContextFlagsUsed()) {

		if(arg0qph->getQueryPlan() == 0 || arg0Nodes == 0 ||
			arg1->getStaticResolutionContext().areContextFlagsUsed()) {

			// Swap the arguments, and the operation
			op = oppositeOperation(op);
			swapASTNode(arg0, arg1);
			swapASTNode(arg0Nodes, arg1Nodes);
			swapCast(arg0Cast, arg1Cast);
			swapQPH(arg0qph, arg1qph);
		} else {			
			// TBD Perform a value join if both arguments are nodes - jpcs

			// Decide between the two ways based on the number of keys returned
			bool ok0, ok1;
			QueryPlan::Cost cost0, cost1;
			ok0 = arg0qph->costQueryPlan(cost0, xpc_);
			ok1 = arg1qph->costQueryPlan(cost1, xpc_);

			if(ok1 && (!ok0 || cost1.keys < cost0.keys)) {
				// Swap the arguments, and the operation
				op = oppositeOperation(op);
				swapASTNode(arg0, arg1);
				swapASTNode(arg0Nodes, arg1Nodes);
				swapCast(arg0Cast, arg1Cast);
				swapQPH(arg0qph, arg1qph);
			}
		}
	}

	if(arg0Nodes != 0 && !arg1->getStaticResolutionContext().areContextFlagsUsed()) {
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		DbXmlNav *nav = new (mm) DbXmlNav(mm);
		nav->setLocationInfo(item);
		if(arg0Nodes->getType() != ASTNode::CONTEXT_ITEM) {
			nav->addStep(arg0Nodes);
		}

		DbXmlCompare *compare = new (mm) DbXmlCompare(op, generalComp, arg1, mm);
		compare->setLocationInfo(item);
		compare->setQueryPlanHolder(arg0qph);

		if(arg0Cast != 0) {
			const XMLCh *uri = arg0Cast->getSequenceType()->getTypeURI(xpc_);
			const XMLCh *name = arg0Cast->getSequenceType()->getConstrainingType()->getName();
			compare->setCastType(uri, name);
		}

		nav->addStep(compare);
		nav->recalculateSRC();

		if(insidePredicate) {
			return nav;
		} else {
			// We aren't directly inside a predicate, so we need to add
			// an fn:exists() function
			VectorOfASTNodes args = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
			args.push_back(nav);
			ASTNode *result = new (mm) FunctionExists(args, mm);
			result->setLocationInfo(item);
			return result;
		}
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::optimizeOperator(XQOperator *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	if(item->getOperatorName() == And::name || item->getOperatorName() == Or::name)
		ancestors_.back().second = true;
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	ancestors_.back().second = false;

	if(item->getOperatorName() == GeneralComp::name) {
		DbXmlGeneralComp *db = (DbXmlGeneralComp*)item;
		return createDbXmlCompare(item, ((GeneralComp*)item)->getOperation(),
			/*generalComp*/true, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == Equals::name) {
		DbXmlEquals *db = (DbXmlEquals*)item;
		return createDbXmlCompare(item, GeneralComp::EQUAL,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == NotEquals::name) {
		DbXmlNotEquals *db = (DbXmlNotEquals*)item;
		return createDbXmlCompare(item, GeneralComp::NOT_EQUAL,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == LessThan::name) {
		DbXmlLessThan *db = (DbXmlLessThan*)item;
		return createDbXmlCompare(item, GeneralComp::LESS_THAN,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == LessThanEqual::name) {
		DbXmlLessThanEqual *db = (DbXmlLessThanEqual*)item;
		return createDbXmlCompare(item, GeneralComp::LESS_THAN_EQUAL,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == GreaterThan::name) {
		DbXmlGreaterThan *db = (DbXmlGreaterThan*)item;
		return createDbXmlCompare(item, GeneralComp::GREATER_THAN,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == GreaterThanEqual::name) {
		DbXmlGreaterThanEqual *db = (DbXmlGreaterThanEqual*)item;
		return createDbXmlCompare(item, GeneralComp::GREATER_THAN_EQUAL,
			/*generalComp*/false, &db->getArg0QPH(), &db->getArg1QPH());
	}

	else if(item->getOperatorName() == Intersect::name) {
		return createDbXmlIntersect((Intersect*)item);
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::optimizeIf(XQIf *item)
{
	ancestors_.back().second = true;
	item->setTest(optimize(const_cast<ASTNode *>(item->getTest())));
	ancestors_.back().second = false;
	item->setWhenTrue(optimize(const_cast<ASTNode *>(item->getWhenTrue())));
	item->setWhenFalse(optimize(const_cast<ASTNode *>(item->getWhenFalse())));
	return item;
}

ASTNode *ASTRewriteOptimizer::optimizeNav(XQNav *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	DbXmlNav *nav = new (mm) DbXmlNav(item, mm);
	nav->setLocationInfo(item);

	return optimizeDbXmlNav(nav);
}

static void optimizeWhere(XQVariableBinding *binding, ASTNode *&where, ASTNodeImpl *&allValues, XPath2MemoryManager *mm)
{
	DbXmlNav *whereNav = 0;
	if(where->getType() == (ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
		whereNav = (DbXmlNav*)where;
	}
	else if(where->getType() == ASTNode::DOCUMENT_ORDER &&
		((XQDocumentOrder*)where)->getExpression()->getType() == (ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
		whereNav = (DbXmlNav*)((XQDocumentOrder*)where)->getExpression();
	}

	if(whereNav != 0 && !whereNav->getStaticResolutionContext().areContextFlagsUsed()) {

		ASTNode *firstStep = whereNav->getSteps().front();
		if(firstStep->getType() == ASTNode::VARIABLE && binding->_variable != 0 &&
			XPath2Utils::equals(((XQVariable*)firstStep)->getName(), binding->_vName) &&
			XPath2Utils::equals(((XQVariable*)firstStep)->getURI(), binding->_vURI)) {

			// Remove the initial variable lookup
			const_cast<DbXmlNav::Steps&>(whereNav->getSteps())
				.erase(const_cast<DbXmlNav::Steps&>(whereNav->getSteps()).begin());
			whereNav->recalculateSRC();

			if(whereNav->getStaticResolutionContext().isVariableUsed(binding->_vURI, binding->_vName)) {
				// It still uses the binding variable, so we can't move it to be a predicate.
				// Put things back the way they were...
				whereNav->addStepFront(firstStep);
				whereNav->recalculateSRC();
			} else {
				DbXmlNav *nav = new (mm) DbXmlNav(mm);
				nav->setLocationInfo(allValues);
				nav->addStep(allValues);

				DbXmlFilter *filter = new (mm) DbXmlFilter(whereNav, mm);
				filter->setLocationInfo(where);
				nav->addStep(filter);

				nav->recalculateSRC();

				allValues = nav;
				where = 0;
			}
		}
	}

	else if(where->getType() == ASTNode::OPERATOR &&
		((XQOperator*)where)->getOperatorName() == And::name) {
		And *andOp = (And*)where;
		unsigned int i = 0;
		while(i < andOp->getNumArgs()) {
			ASTNode *arg = andOp->getArgument(i);
			optimizeWhere(binding, arg, allValues, mm);
			if(arg == 0) {
				andOp->removeArgument(i);
			} else {
				andOp->setArgument(i, arg);
				++i;
			}
		}

		if(andOp->getNumArgs() == 1) {
			where = andOp->getArgument(0);
		} else if(andOp->getNumArgs() == 0) {
			where = 0;
		}
	}
}

ASTNode *ASTRewriteOptimizer::optimizeFLWOR(XQFLWOR *item)
{
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		if((*i)->_where) {
			ancestors_.back().second = true;
			(*i)->_where = optimize((*i)->_where);
			ancestors_.back().second = false;

			// If the where clause is a navigation that starts at the binding variable,
			// and this is a FOR binding, we can move the where clause to be a DbXmlFilter
			// step of the binding expression.
			if((*i)->_bindingType == XQVariableBinding::forBinding &&
				(*i)->_allValues->getStaticResolutionContext().getStaticType().isType(StaticType::NODE_TYPE)) {
				optimizeWhere(*i, (*i)->_where, (ASTNodeImpl*&)(*i)->_allValues, xpc_->getMemoryManager());
			}
		}

		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// sort specs
	const XQSort *sort = item->getSort();
	if(sort) {
		XQSort::VectorOfSortSpec *specs = const_cast<XQSort::VectorOfSortSpec *>(sort->getSortSpecs());
		for(XQSort::VectorOfSortSpec::iterator j = specs->begin(); j != specs->end(); ++j) {
			(*j)->setExpression(optimize(const_cast<ASTNode *>((*j)->getExpression())));
		}
	}

	// where and return
	if(item->getWhereExpr()) {
		ancestors_.back().second = true;
		item->setWhereExpr(optimize(const_cast<ASTNode *>(item->getWhereExpr())));
		ancestors_.back().second = false;
	}
	item->setReturnExpr(optimize(const_cast<ASTNode *>(item->getReturnExpr())));

	if(item->getSort() == 0 && item->getWhereExpr() == 0) {
		return optimizeFLOWRToNav(bindings, item, /*quantified*/false);
	} else {
		return item;
	}
}

ASTNode *ASTRewriteOptimizer::optimizeFLOWRToNav(VectorOfVariableBinding *bindings, XQFLWOR *item, bool quantified)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	//
	// We want to rewrite flwor expressions that look like this:
	//
	// for $a in foo/bar return $a/baz
	//
	// into:
	//
	// foo/bar/baz
	//

	while(!bindings->empty()) {
		XQVariableBinding *binding = bindings->back();

		if(binding->_bindingType == XQVariableBinding::forBinding && binding->_where == 0 &&
			binding->_allValues->getStaticResolutionContext().getStaticType().isType(StaticType::NODE_TYPE) &&
			!item->getReturnExpr()->getStaticResolutionContext().areContextFlagsUsed()) {

			DbXmlNav *returnNav = 0;
			if(item->getReturnExpr()->getType() == (ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
				returnNav = (DbXmlNav*)item->getReturnExpr();
			}
			else if(item->getReturnExpr()->getType() == ASTNode::DOCUMENT_ORDER &&
				((XQDocumentOrder*)item->getReturnExpr())->getExpression()->getType() ==
				(ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
				returnNav = (DbXmlNav*)((XQDocumentOrder*)item->getReturnExpr())->getExpression();
			}

			if(returnNav != 0) {
				ASTNode *firstStep = returnNav->getSteps().front();
				if(firstStep->getType() == ASTNode::VARIABLE && binding->_variable != 0 &&
					XPath2Utils::equals(((XQVariable*)firstStep)->getName(), binding->_vName) &&
					XPath2Utils::equals(((XQVariable*)firstStep)->getURI(), binding->_vURI)) {

					// Remove the initial variable lookup
					const_cast<DbXmlNav::Steps&>(returnNav->getSteps())
						.erase(const_cast<DbXmlNav::Steps&>(returnNav->getSteps()).begin());
					// TBD this probably gets things wrong  - jpcs
 					returnNav->recalculateSRC();

					if(returnNav->getStaticResolutionContext().isVariableUsed(binding->_vURI, binding->_vName) ||
						(binding->_positionalVariable != 0 &&
							returnNav->getStaticResolutionContext().isVariableUsed(binding->_pURI, binding->_pName))) {
						// It still uses the binding variable, so we can't move it.
						// Put things back the way they were...
						returnNav->addStepFront(firstStep);
						returnNav->recalculateSRC();
						break;
					} else {
						DbXmlNav *nav = new (mm) DbXmlNav(mm);
						nav->setLocationInfo(binding->_allValues);
						nav->addStep(binding->_allValues);

						// Add the return expression's steps to the binding expression
						for(DbXmlNav::Steps::const_iterator s = returnNav->getSteps().begin();
						    s != returnNav->getSteps().end(); ++s) {
							nav->addStep(*s);
						}

						nav->recalculateSRC();

						// Set the binding expression as the return expression,
						// and remove the old binding
						item->setReturnExpr(nav);
						bindings->pop_back();
					}
				} else break;
			} else if(item->getReturnExpr()->getType() == ASTNode::VARIABLE) {
				XQVariable *returnVar = (XQVariable*)item->getReturnExpr();
				if(XPath2Utils::equals(returnVar->getName(), binding->_vName) &&
					XPath2Utils::equals(returnVar->getURI(), binding->_vURI)) {
					item->setReturnExpr(binding->_allValues);
					bindings->pop_back();
				} else break;
			} else break;
		} else break;
	}

	if(bindings->empty()) {
		if(!quantified) {
			return (ASTNode*)item->getReturnExpr();
		} else if(ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second) {
			return (ASTNode*)item->getReturnExpr();
		} else {
			// We aren't directly inside a predicate, so we need to add
			// an fn:exists() function
			VectorOfASTNodes args = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
			args.push_back((ASTNode*)item->getReturnExpr());
			ASTNode *result = new (mm) FunctionExists(args, mm);
			result->setLocationInfo(item->getReturnExpr());
			return result;
		}
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::optimizeFLWORQuantified(XQQuantified *item)
{
	// bindings
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// no where or sort specs

	// return
	ancestors_.back().second = true;
	item->setReturnExpr(optimize(const_cast<ASTNode *>(item->getReturnExpr())));
	ancestors_.back().second = false;

	if(item->getQuantifierType() == XQQuantified::some) {
		return optimizeFLOWRToNav(bindings, item, /*quantified*/true);
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::createDbXmlContains(DbXmlFunContains *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes&>(item->getArguments());

	// Make sure there is no collation argument
	if(args.size() == 2) {
		XQCastAs *arg0Cast;
		ASTNode *arg0 = findNodeProducer(args[0], arg0Cast);
		ASTNode *arg1 = args[1];

		if(arg0 != 0 && arg0Cast == 0 &&
			!arg1->getStaticResolutionContext().areContextFlagsUsed()) {
			XPath2MemoryManager *mm = xpc_->getMemoryManager();

			DbXmlNav *nav = new (mm) DbXmlNav(mm);
			nav->setLocationInfo(item);
			if(arg0->getType() != ASTNode::CONTEXT_ITEM) {
				nav->addStep(arg0);
			}

			DbXmlContains *contains = new (mm) DbXmlContains(arg1, mm);
			contains->setLocationInfo(item);
			contains->setQueryPlanHolder(&item->getArg0QPH());
			nav->addStep(contains);
			nav->recalculateSRC();

			if(ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second) {
				return nav;
			} else {
				// We aren't directly inside a predicate, so we need to add
				// an fn:exists() function
				VectorOfASTNodes args = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
				args.push_back(nav);
				ASTNode *result = new (mm) FunctionExists(args, mm);
				result->setLocationInfo(item);
				return result;
			}
		}
	}

	return item;
}

ASTNode *ASTRewriteOptimizer::optimizeFunction(XQFunction *item)
{
	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	NodeVisitingOptimizer::optimizeFunction(item);

	if(uri == XQFunction::XMLChFunctionURI) {
		if(name == FunctionContains::name) {
			return createDbXmlContains((DbXmlFunContains*)item);
		}
	}

	return item;
}
