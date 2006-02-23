//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlNav.cpp,v 1.20 2005/12/16 11:35:44 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "DbXmlNav.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlContext.hpp"
#include "DbXmlFilter.hpp"
#include "DbXmlCompare.hpp"
#include "DbXmlContains.hpp"
#include "../Document.hpp"
#include "../QueryExecutionContext.hpp"

#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/SequenceResult.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

DbXmlNav::DbXmlNav(XPath2MemoryManager* memMgr)
	: DataItemNav(memMgr)
{
}

DbXmlNav::DbXmlNav(DataItemNav *nav, XPath2MemoryManager* memMgr)
	: DataItemNav(memMgr)
{
	addPredicates(nav->getPredicates());
	_src.copy(nav->getStaticResolutionContext());
	_gotoRoot = nav->getGotoRootFirst();
	_steps = nav->getSteps();
}

DbXmlNav::DbXmlNav(DataItem *step, XPath2MemoryManager* memMgr)
	: DataItemNav(memMgr)
{
	_src.copy(step->getStaticResolutionContext());
	_steps.push_back(step);
}

void DbXmlNav::recalculateSRC()
{
	_src.clear();

	if(_gotoRoot) {
		_src.contextItemUsed(true);
	}

	Steps::iterator begin = _steps.begin();
	Steps::iterator end = _steps.end();
	Steps::iterator it = begin;
	for(; it != end; ++it) {
		if(it != begin || _gotoRoot) {
			// Remove context item usage
			_src.addExceptContextFlags(it->step->getStaticResolutionContext());
		}
		else {
			_src.add(it->step->getStaticResolutionContext());
		}
	}

	if(!_steps.empty()) {
		_src.getStaticType() = _steps.back().step->getStaticResolutionContext().getStaticType();
	}
	else if(_gotoRoot) {
		_src.getStaticType().flags = StaticType::NODE_TYPE;
	}

	// Force recalculation of the properties
	_isSorted = -1;
	getIsSorted();
}

Result DbXmlNav::sortIfPossible(const Result &result, unsigned int &props, DynamicContext *context)
{
	if(props & StaticResolutionContext::DOCORDER) {
		// It's already sorted
		return result;
	}
	else if(props & StaticResolutionContext::GROUPED) {
		// If the result is not in document order, but it is
		// grouped, we can efficiently sort it using the GroupedSort
		// iterator
		props |= StaticResolutionContext::DOCORDER;
		return new GroupedSort(result, context);
	} else {
		// We can't sort it efficiently
		return result;
	}
}

Result DbXmlNav::makeStepResult(Result &parent, unsigned int &props, const Steps::const_iterator &it,
	const Steps::const_iterator &end, int flags, DynamicContext *context)
{
	Result result(0);

	if(it->usesContextSize) {
		// We need the context size, so convert to a Sequence to work it out
		Sequence seq(parent.toSequence(context));
		result =  new StepResult(new SequenceResult(seq), it->step, seq.getLength(), flags, context);
	}
	else if(it->step->getType() == DataItem::STEP) {
		const DataItemStep *distep = (const DataItemStep*)it->step;
		const DbXmlNodeTest *nodeTest = (const DbXmlNodeTest*)distep->getNodeTest();

		if(nodeTest->getQueryPlan() != 0) {
			// Only try to sort if we've got a query plan
			parent = sortIfPossible(parent, props, context);
		}
		if(nodeTest->getQueryPlan() != 0 && (props & StaticResolutionContext::DOCORDER)) {
			result = new QueryPlanStepResult(parent, distep, flags, context);
		}
		else {
			result = new StepResult(parent, it->step, 0, flags, context);
		}
	}
	else if(it->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_COMPARE) {
		result = ((const DbXmlCompare*)it->step)->createResult(parent, props, context);
	}
	else if(it->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_FILTER) {
		result = new DbXmlFilter::FilterResult(parent, props, ((const DbXmlFilter*)it->step)->getFilterExpr(), context);
	}
	else if(it->step->getType() == (DataItem::whichType)DbXmlContext::DBXML_CONTAINS) {
		result = ((const DbXmlContains*)it->step)->createResult(parent, props, context);
	}
	else if(!it->step->getStaticResolutionContext().areContextFlagsUsed()) {
		result = it->step->collapseTree(context, flags);
	}
	else {
		result = new StepResult(parent, it->step, 0, flags, context);
	}
	props = combineProperties(props, it->step->getStaticResolutionContext().getProperties());

	const StaticType &st = it->step->getStaticResolutionContext().getStaticType();
	if(it == (end-1)) {
		// the last step allows either nodes or atomic items
		if((st.flags & StaticType::NODE_TYPE) &&
			((st.flags & StaticType::NUMERIC_TYPE) ||
				(st.flags & StaticType::OTHER_TYPE))) {
			result = new LastStepCheckResult(result, context);
		}
	}
	else {
		if((st.flags & StaticType::NUMERIC_TYPE) ||
			(st.flags & StaticType::OTHER_TYPE)) {
			result = new IntermediateStepCheckResult(result, context);
		}
	}

	return result;
}

Result DbXmlNav::createResultForDbXmlFilter(const Result &contextItems, unsigned int props, DynamicContext* context, int flags) const
{
	Result result = contextItems;
	flags &= ~(RETURN_ONE | RETURN_TWO);

	Steps::const_iterator it = _steps.begin();
	Steps::const_iterator end = _steps.end();

	if(_gotoRoot) {
		result = new DbXmlGotoRootResult(result, context);
		if(props & StaticResolutionContext::ONENODE) {
			props = StaticResolutionContext::DOCORDER;
		} else {
			props = 0;
		}
		props |= StaticResolutionContext::GROUPED | StaticResolutionContext::PEER;
	} else if(it != end) {
		// Remove the GROUPED property, because DbXmlFilter
		// doesn't work with buffered iterators like
		// GroupedSort - jpcs
		props &= ~StaticResolutionContext::GROUPED;
		result = makeStepResult(result, props, it, end, flags, context);
		++it;
	}

	for(; it != end; ++it) {
		// Remove the GROUPED property, because DbXmlFilter
		// doesn't work with buffered iterators like
		// GroupedSort - jpcs
		props &= ~StaticResolutionContext::GROUPED;
		result = makeStepResult(result, props, it, end, flags, context);
	}

	return result;
}

Result DbXmlNav::createResult(DynamicContext* context, int flags) const
{
	Result result(0);
	flags &= ~(RETURN_ONE | RETURN_TWO);

	Steps::const_iterator it = _steps.begin();
	Steps::const_iterator end = _steps.end();

	// The properties of the context item
	unsigned int props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER |
		StaticResolutionContext::SUBTREE | StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE;

	if(_gotoRoot) {
		result = new GotoRootResult(context);
		props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER;
	} else if(it != end) {
		result = makeStepResult(result, props, it, end, flags, context);
		props = it->step->getStaticResolutionContext().getProperties();
		++it;
	}

	for(; it != end; ++it) {
		result = makeStepResult(result, props, it, end, flags, context);
	}

	if(props & StaticResolutionContext::DOCORDER) {
		return result;
	}
	else if(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED ||
		flags & DataItem::UNORDERED) {
		return new UniqueNodesResult(result, context);
	}
	else if(props & StaticResolutionContext::GROUPED) {
		return new GroupedSort(result, context);
	}
	else {
		return result.sortIntoDocumentOrder(context);
	}
}

/////////////////////////////////////
// GroupedSort
/////////////////////////////////////

DbXmlNav::GroupedSort::GroupedSort(const Result &parent, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  toDo_(true),
	  next_(0),
	  sorted_(0)
{
}

static inline ID getDocID(const Item::Ptr &item)
{
	const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(node != 0);
	return ((Document&)node->getXmlDocument()).getID();
}

Item::Ptr DbXmlNav::GroupedSort::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;
		next_ = parent_.next(context);
	}

	Item::Ptr result = sorted_.next(context);
	if(result.isNull() && next_.notNull()) {
		ID docID = getDocID(next_);

		Sequence seq(next_, context->getMemoryManager());
		while((next_ = parent_.next(context)).notNull() && getDocID(next_) == docID) {
			seq.addItem(next_);
		}

		if(next_.isNull()) {
			parent_ = 0;
		}

		seq.sortIntoDocumentOrder(context);
		sorted_ = seq;
		result = sorted_.next(context);
	}

	return result;
}

string DbXmlNav::GroupedSort::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlNav_GroupedSort/>";
}

/////////////////////////////////////
// DbXmlGotoRootResult
/////////////////////////////////////

DbXmlNav::DbXmlGotoRootResult::DbXmlGotoRootResult(const Result &parent, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent)
{
}

Item::Ptr DbXmlNav::DbXmlGotoRootResult::next(DynamicContext *context)
{
	Item::Ptr item = parent_.next(context);

	if(item != NULLRCP) {
		// Do equivalent of root()
		if(item->isNode()) {
			return FunctionRoot::root((const Node::Ptr)item, context);
		} else {
			DSLthrow(TypeErrorException,X("DbXmlNav::DbXmlGotoRootResult::next"),
				X("An attempt to navigate when the Context Item was not a node was made [err:XPTY0020]"));
		}
	} else
		parent_ = 0;
	return 0;
}

string DbXmlNav::DbXmlGotoRootResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlNav_DbXmlGotoRootResult/>";
}

/////////////////////////////////////
// QueryPlanStepResult
/////////////////////////////////////

DbXmlNav::QueryPlanStepResult::QueryPlanStepResult(const Result &parent, const DataItemStep *step,
	int flags, DynamicContext *context)
	: QueryPlanResultImpl(parent, (const DbXmlNodeTest*)step->getNodeTest(), step->getAxis(), context),
	  step_(step),
	  flags_(flags)
{
	DBXML_ASSERT(step->getNodeTest()->getInterface(DbXmlNodeTest::gDbXml) != 0);
}

void DbXmlNav::QueryPlanStepResult::setResult(const DbXmlNodeImpl *node, DynamicContext *context)
{
	if(result_.isNull()) {
		result_ = node->getAxisResult(step_->getAxis(), step_->getNodeTest(), context);
	}

	Predicates::const_iterator it = step_->getPredicates().begin();
	Predicates::const_iterator end2 = step_->getPredicates().end();
	for(; it != end2; ++it) {
		result_ = step_->createPredicateResult(result_, *it, context);
	}
	result_ = step_->postPredicateResultHook(result_, context, flags_);
}

string DbXmlNav::QueryPlanStepResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlNav_QueryPlanStepResult/>";
}
