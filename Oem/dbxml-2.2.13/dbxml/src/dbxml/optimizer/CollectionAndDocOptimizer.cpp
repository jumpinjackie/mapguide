//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: CollectionAndDocOptimizer.cpp,v 1.25 2005/11/30 17:12:15 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "CollectionAndDocOptimizer.hpp"
#include "../UTF8.hpp"
#include "../dataItem/QueryPlanFunction.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/DbXmlNav.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlFilter.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlContext.hpp"

#include <pathan/XPath2Utils.hpp>
#include <pathan/functions/FunctionDoc.hpp>
#include <pathan/functions/FunctionDocAvailable.hpp>
#include <pathan/functions/FunctionCollection.hpp>
#include <pathan/functions/FunctionExists.hpp>
#include <pathan/functions/FunctionContains.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/operators/And.hpp>
#include <pathan/operators/Or.hpp>
#include <pathan/operators/Equals.hpp>
#include <pathan/operators/NotEquals.hpp>
#include <pathan/operators/LessThan.hpp>
#include <pathan/operators/LessThanEqual.hpp>
#include <pathan/operators/GreaterThan.hpp>
#include <pathan/operators/GreaterThanEqual.hpp>
#include <pathan/operators/Union.hpp>
#include <pathan/operators/Intersect.hpp>
#include <pathan/operators/Except.hpp>
#include <pathan/DynamicContext.hpp>

#include <xercesc/dom/DOMNode.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DataItem *CollectionAndDocOptimizer::optimize(DataItem *item)
{
	ancestors_.push_back(ParentInfo(item, false));
	DataItem *result = NodeVisitingOptimizer::optimize(item);
	ancestors_.pop_back();
	return result;
}

DataItem *CollectionAndDocOptimizer::optimizePredicate(DataItem *item)
{
	ancestors_.back().second = true;
	DataItem *result = optimize(item);
	ancestors_.back().second = false;
	return result;
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

DataItem *CollectionAndDocOptimizer::createDbXmlCompare(DataItemOperator *item, GeneralComp::ComparisonOperation op, bool generalComp)
{
	bool insidePredicate = ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second;

	if(!generalComp && !insidePredicate) return item;

	DataItem *arg0 = item->getArgument(0);
	DataItem *arg1 = item->getArgument(1);
	const StaticType *arg0_st = &arg0->getStaticResolutionContext().getStaticType();
	const StaticType *arg1_st = &arg1->getStaticResolutionContext().getStaticType();

	if(arg1_st->isNodesOnly() && !arg0->getStaticResolutionContext().areContextFlagsUsed()) {
		// Swap the arguments, and the operation
		op = oppositeOperation(op);
		arg0 = arg1;
		arg0_st = arg1_st;
		arg1 = item->getArgument(0);
	}

	if(arg0_st->isNodesOnly() && !arg1->getStaticResolutionContext().areContextFlagsUsed()) {
		XPath2MemoryManager *mm = xpc_->getMemoryManager();

		DbXmlNav *nav = 0;
		if(arg0->getType() == DataItem::NAVIGATION &&
			((DataItemImpl*)arg0)->getPredicates().empty()) {
			nav = (DbXmlNav*)arg0;
		} else {
			nav = new (mm) DbXmlNav(mm);
			if(arg0->getType() != DataItem::CONTEXT_ITEM) {
				nav->addStep(DataItemNav::StepInfo(arg0, arg0->getStaticResolutionContext().isContextSizeUsed()));
			}
		}

		DbXmlCompare *compare = new (mm) DbXmlCompare(op, generalComp, arg1, mm);
		nav->addStep(DataItemNav::StepInfo(compare, compare->getStaticResolutionContext().isContextSizeUsed()));
		nav->recalculateSRC();

		if(insidePredicate) {
			return nav;
		} else {
			// We aren't directly inside a predicate, so we need to add
			// an fn:exists() function
			VectorOfDataItems args = VectorOfDataItems(PathanAllocator<DataItem*>(mm));
			args.push_back(nav);
			return new (mm) FunctionExists(args, mm);
		}
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::optimizeOperator(DataItemOperator *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	if(item->getOperatorName() == And::name || item->getOperatorName() == Or::name)
		ancestors_.back().second = true;
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	ancestors_.back().second = false;
	optimizePredicates(item);

	if(item->getPredicates().empty()) {
		if(item->getOperatorName() == GeneralComp::name) {
			return createDbXmlCompare(item, ((GeneralComp*)item)->getOperation(),
				/*generelComp*/true);
		}

		else if(item->getOperatorName() == Equals::name) {
			return createDbXmlCompare(item, GeneralComp::EQUAL,
				/*generelComp*/false);
		}

		else if(item->getOperatorName() == NotEquals::name) {
			return createDbXmlCompare(item, GeneralComp::NOT_EQUAL,
				/*generelComp*/false);
		}

		else if(item->getOperatorName() == LessThan::name) {
			return createDbXmlCompare(item, GeneralComp::LESS_THAN,
				/*generelComp*/false);
		}

		else if(item->getOperatorName() == LessThanEqual::name) {
			return createDbXmlCompare(item, GeneralComp::LESS_THAN_EQUAL,
				/*generelComp*/false);
		}

		else if(item->getOperatorName() == GreaterThan::name) {
			return createDbXmlCompare(item, GeneralComp::GREATER_THAN,
				/*generelComp*/false);
		}

		else if(item->getOperatorName() == GreaterThanEqual::name) {
			return createDbXmlCompare(item, GeneralComp::GREATER_THAN_EQUAL,
				/*generelComp*/false);
		}
	}

	if(item->getOperatorName() == Union::name ||
		item->getOperatorName() == Intersect::name ||
		item->getOperatorName() == Except::name) {
		return optimizeNodeReturningDI(item);
	}

	return item;
}

static void filterRewrite(DataItemNav::Steps &newSteps, DataItem::Predicates &newPreds,
	DataItem::Predicates::const_iterator it, DataItem::Predicates::const_iterator end,
	XPath2MemoryManager *mm)
{
	for(; it != end; ++it) {
		if(it->pred->getType() == DataItem::NAVIGATION &&
			it->pred->getStaticResolutionContext().getStaticType().isNodesOnly()) {
			DbXmlNav *predNav = (DbXmlNav*)it->pred;

			// We can optimise it
			DataItemImpl *stepToAdd = 0;
			if(!predNav->getGotoRootFirst() && predNav->getSteps().size() == 1) {
				DataItemImpl *step = (DataItemImpl*)predNav->getSteps().front().step;
				if(step->getType() == (DataItem::whichType)DbXmlContext::DBXML_COMPARE ||
					step->getType() == (DataItem::whichType)DbXmlContext::DBXML_CONTAINS) {
					stepToAdd = step;
				}
			}

			if(stepToAdd == 0) {
				stepToAdd = new (mm) DbXmlFilter(predNav, mm);
			}

			newSteps.push_back(DataItemNav::StepInfo(stepToAdd, predNav->getStaticResolutionContext().isContextSizeUsed()));
			filterRewrite(newSteps, const_cast<DataItem::Predicates&>(stepToAdd->getPredicates()), it + 1, end, mm);
			break;
		} else {
			newPreds.push_back(*it);
		}
	}
}

DataItem *CollectionAndDocOptimizer::optimizeNav(DataItemNav *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	return optimizeDbXmlNav(new (mm) DbXmlNav(item, mm));
}

DataItem *CollectionAndDocOptimizer::optimizeDbXmlNav(DbXmlNav *nav)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	NodeVisitingOptimizer::optimizeNav(nav);

	// Look for predicates that we can change into DbXmlFilter steps.
	// We can do this if the predicate is a DbXmlNav object that
	// returns nodes, and it doesn't have any predicates after it that
	// use the context size or context position (as this rewrite messes
	// up the context position and size).

	DataItemNav::Steps newSteps = DataItemNav::Steps(PathanAllocator<DataItemNav::StepInfo>(mm));

	DataItemNav::Steps &args = const_cast<DataItemNav::Steps &>(nav->getSteps());
	DataItemNav::Steps::iterator s = args.begin();
	for(; s != args.end(); ++s) {
		newSteps.push_back(*s);

		DataItem::Predicates newPreds = DataItem::Predicates(PathanAllocator<DataItem::PredInfo>(mm));

		DataItem::Predicates::const_iterator begin = ((const DataItemImpl*)s->step)->getPredicates().begin();
		DataItem::Predicates::const_iterator end = ((const DataItemImpl*)s->step)->getPredicates().end();

		for(DataItem::Predicates::const_iterator p = begin; p != end; ++p) {
			if(p->pred->getStaticResolutionContext().isContextPositionUsed() ||
				p->pred->getStaticResolutionContext().isContextSizeUsed() ||
				p->pred->getStaticResolutionContext().getStaticType().flags & StaticType::NUMERIC_TYPE) {
				for(; begin != p; ++begin) {
					newPreds.push_back(*begin);
				}
			}
		}

		// Now the range [begin, end) contains predicates that are potentially
		// replaced by DbXmlFilter.
		filterRewrite(newSteps, newPreds, begin, end, mm);
		const_cast<DataItem::Predicates&>(((const DataItemImpl*)s->step)->getPredicates()) = newPreds;
	}

	nav->setSteps(newSteps);
	nav->recalculateSRC();

	return nav;
}

static void optimizeWhere(XQVariableBinding *binding, DataItem *&where, DataItemImpl *&allValues, XPath2MemoryManager *mm)
{
	if(where->getType() == DataItem::NAVIGATION &&
		!where->getStaticResolutionContext().areContextFlagsUsed()) {

		DbXmlNav *whereNav = (DbXmlNav*)where;
		DataItem *firstStep = whereNav->getSteps().front().step;
		if(firstStep->getType() == DataItem::VARIABLE && binding->_variable != 0 &&
			XPath2Utils::equals(((DataItemVariable*)firstStep)->getName(), binding->_vName) &&
			XPath2Utils::equals(((DataItemVariable*)firstStep)->getURI(), binding->_vURI)) {

			// Remove the initial variable lookup
			const_cast<DataItemNav::Steps&>(whereNav->getSteps())
				.erase(const_cast<DataItemNav::Steps&>(whereNav->getSteps()).begin());
			whereNav->recalculateSRC();

			if(whereNav->getStaticResolutionContext().isVariableUsed(binding->_vURI, binding->_vName)) {
				// It still uses the binding variable, so we can't move it to be a predicate.
				// Put things back the way they were...
				whereNav->addStepFront(firstStep);
				whereNav->recalculateSRC();
			} else {
				DbXmlNav *nav = 0;
				if(allValues->getType() == DataItem::NAVIGATION && allValues->getPredicates().empty()) {
					nav = (DbXmlNav*)allValues;
				} else {
					nav = new (mm) DbXmlNav(mm);
					nav->addStep(DataItemNav::StepInfo(allValues,
							     allValues->getStaticResolutionContext().isContextSizeUsed()));
				}

				DataItem *stepToAdd = 0;
				if(!whereNav->getGotoRootFirst() && whereNav->getSteps().size() == 1) {

					DataItem *step = whereNav->getSteps().front().step;
					if(step->getType() == (DataItem::whichType)DbXmlContext::DBXML_COMPARE ||
						step->getType() == (DataItem::whichType)DbXmlContext::DBXML_CONTAINS) {
						stepToAdd = step;
					}
				}

				if(stepToAdd == 0) {
					stepToAdd = new (mm) DbXmlFilter(whereNav, mm);
				}

				nav->addStep(stepToAdd);
				nav->recalculateSRC();

				allValues = nav;
				where = 0;
			}
		}
	}

	else if(where->getType() == DataItem::OPERATOR &&
		((DataItemOperator*)where)->getOperatorName() == And::name) {
		And *andOp = (And*)where;
		unsigned int i = 0;
		while(i < andOp->getNumArgs()) {
			DataItem *arg = andOp->getArgument(i);
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

DataItem *CollectionAndDocOptimizer::optimizeFLWOR(XQFLWOR *item)
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
				(*i)->_allValues->getStaticResolutionContext().getStaticType().isNodesOnly()) {
				optimizeWhere(*i, (*i)->_where, (DataItemImpl*&)(*i)->_allValues, xpc_->getMemoryManager());
			}
		}

		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// sort specs
	const XQSort *sort = item->getSort();
	if(sort) {
		XQSort::VectorOfSortSpec *specs = const_cast<XQSort::VectorOfSortSpec *>(sort->getSortSpecs());
		for(XQSort::VectorOfSortSpec::iterator j = specs->begin(); j != specs->end(); ++j) {
			(*j)->setExpression(optimize(const_cast<DataItem *>((*j)->getExpression())));
		}
	}

	// where and return
	if(item->getWhereExpr()) {
		ancestors_.back().second = true;
		item->setWhereExpr(optimize(const_cast<DataItem *>(item->getWhereExpr())));
		ancestors_.back().second = false;
	}
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));

	optimizePredicates(item);

	if(item->getSort() == 0 && item->getWhereExpr() == 0) {
		return optimizeFLOWRToNav(bindings, item, /*quantified*/false);
	} else {
		return item;
	}
}

DataItem *CollectionAndDocOptimizer::optimizeFLOWRToNav(VectorOfVariableBinding *bindings, XQFLWOR *item, bool quantified)
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
			binding->_allValues->getStaticResolutionContext().getStaticType().isNodesOnly() &&
			!item->getReturnExpr()->getStaticResolutionContext().areContextFlagsUsed() &&
			((DataItemImpl*)item->getReturnExpr())->getPredicates().empty()) {

			if(item->getReturnExpr()->getType() == DataItem::NAVIGATION) {
				DbXmlNav *returnNav = (DbXmlNav*)item->getReturnExpr();
				DataItem *firstStep = returnNav->getSteps().front().step;
				if(firstStep->getType() == DataItem::VARIABLE && binding->_variable != 0 &&
					XPath2Utils::equals(((DataItemVariable*)firstStep)->getName(), binding->_vName) &&
					XPath2Utils::equals(((DataItemVariable*)firstStep)->getURI(), binding->_vURI)) {

					// Remove the initial variable lookup
					const_cast<DataItemNav::Steps&>(returnNav->getSteps())
						.erase(const_cast<DataItemNav::Steps&>(returnNav->getSteps()).begin());
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
						DbXmlNav *nav = 0;
						if(binding->_allValues->getType() == DataItem::NAVIGATION &&
							((DataItemImpl*)binding->_allValues)->getPredicates().empty()) {
							nav = (DbXmlNav*)binding->_allValues;
						} else {
							nav = new (mm) DbXmlNav(mm);
							nav->addStep(DataItemNav::StepInfo(binding->_allValues,
									     binding->_allValues->getStaticResolutionContext().isContextSizeUsed()));
						}

						// Add the return expression's steps to the binding expression
						for(DataItemNav::Steps::const_iterator s = returnNav->getSteps().begin();
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
			} else if(item->getReturnExpr()->getType() == DataItem::VARIABLE) {
				DataItemVariable *returnVar = (DataItemVariable*)item->getReturnExpr();
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
			return (DataItem*)item->getReturnExpr();
		} else if(ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second) {
			return (DataItem*)item->getReturnExpr();
		} else {
			// We aren't directly inside a predicate, so we need to add
			// an fn:exists() function
			VectorOfDataItems args = VectorOfDataItems(PathanAllocator<DataItem*>(mm));
			args.push_back((DataItem*)item->getReturnExpr());
			return new (mm) FunctionExists(args, mm);
		}
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::optimizeFLWORQuantified(XQQuantified *item)
{
	// bindings
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// no where or sort specs

	// return
	ancestors_.back().second = true;
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));
	ancestors_.back().second = false;

	optimizePredicates(item);

	if(item->getQuantifierType() == XQQuantified::some) {
		return optimizeFLOWRToNav(bindings, item, /*quantified*/true);
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::createDbXmlContains(DataItemFunction *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems&>(item->getArguments());

	// Make sure there is no collation argument
	if(args.size() == 2) {
		DataItem *arg0 = args[0];
		DataItem *arg1 = args[1];

		if(arg0->getStaticResolutionContext().getStaticType().isNodesOnly() &&
			!arg1->getStaticResolutionContext().areContextFlagsUsed()) {

			XPath2MemoryManager *mm = xpc_->getMemoryManager();

			DbXmlNav *nav = 0;
			if(arg0->getType() == DataItem::NAVIGATION &&
				((DataItemImpl*)arg0)->getPredicates().empty()) {
				nav = (DbXmlNav*)arg0;
			} else {
				nav = new (mm) DbXmlNav(mm);
				if(arg0->getType() != DataItem::CONTEXT_ITEM) {
					nav->addStep(DataItemNav::StepInfo(arg0, arg0->getStaticResolutionContext().isContextSizeUsed()));
				}
			}

			DbXmlContains *contains = new (mm) DbXmlContains(arg1, mm);
			nav->addStep(DataItemNav::StepInfo(contains, contains->getStaticResolutionContext().isContextSizeUsed()));
			nav->recalculateSRC();

			if(ancestors_.size() > 1 && ancestors_[ancestors_.size() - 2].second) {
				return nav;
			} else {
				// We aren't directly inside a predicate, so we need to add
				// an fn:exists() function
				VectorOfDataItems args = VectorOfDataItems(PathanAllocator<DataItem*>(mm));
				args.push_back(nav);
				return new (mm) FunctionExists(args, mm);
			}
		}
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::optimizeNodeReturningDI(DataItemImpl *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	bool insideNav = ancestors_.size() > 1 &&
		!ancestors_[ancestors_.size() - 2].second &&
		ancestors_[ancestors_.size() - 2].first->getType() == DataItem::NAVIGATION;

	if(!insideNav && !item->getPredicates().empty()) {
		// Put it inside a DbXmlNav, where it will be optimised better
		DbXmlNav *nav = new (mm) DbXmlNav(item, mm);
		ancestors_.push_back(ParentInfo(nav, false));
		DataItem *result = optimizeDbXmlNav(nav);
		ancestors_.pop_back();
		return result;
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::optimizeFunction(DataItemFunction *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	NodeVisitingOptimizer::optimizeFunction(item);

	// Replace fn:doc() or fn:collection() with our fn:QueryPlanFunction()
	if(uri == DataItemFunction::XMLChFunctionURI) {
		if(name == FunctionDoc::name || name == FunctionCollection::name) {
			QueryPlanFunction::Type type = 
				(name == FunctionDoc::name ? QueryPlanFunction::DOCUMENT : QueryPlanFunction::COLLECTION);
			QueryPlanFunction *result = new (mm)
				QueryPlanFunction(minder_, type, item->getArguments(), mm);
			result->addPredicates(item->getPredicates());

			return optimizeNodeReturningDI(result);
		}

		else if(name == FunctionDocAvailable::name) {
			DbXmlDocAvailable *result = new (mm)
				DbXmlDocAvailable(minder_, item->getArguments(), mm);
			result->addPredicates(item->getPredicates());
			return result;
		}

		else if(name == FunctionContains::name &&
			item->getPredicates().empty()) {
			return createDbXmlContains(item);
		}

		else if(name == FunctionRoot::name) {
			return optimizeNodeReturningDI(item);
		}
	}

	return item;
}

DataItem *CollectionAndDocOptimizer::optimizeStep(DataItemStep *item)
{
	NodeVisitingOptimizer::optimizeStep(item);
	return optimizeNodeReturningDI(item);
}
