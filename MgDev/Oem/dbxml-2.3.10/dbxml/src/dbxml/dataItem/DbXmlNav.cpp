//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlNav.cpp,v 1.35 2006/10/30 17:45:55 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "DbXmlNav.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlFilter.hpp"
#include "DbXmlCompare.hpp"
#include "DbXmlContains.hpp"
#include "DbXmlStep.hpp"
#include "LookupIndexFunction.hpp"
#include "QueryPlanFunction.hpp"
#include "../Document.hpp"
#include "../QueryExecutionContext.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

DbXmlNav::DbXmlNav(XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_NAV, memMgr),
	  steps_(XQillaAllocator<ASTNode*>(memMgr)),
	  sortAdded_(false)
{
}

DbXmlNav::DbXmlNav(XQNav *nav, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_NAV, memMgr),
	  steps_(XQillaAllocator<ASTNode*>(memMgr)),
	  sortAdded_(false)
{
	for(XQNav::Steps::const_iterator i = nav->getSteps().begin();
	    i != nav->getSteps().end(); ++i) {
		steps_.push_back(i->step);
	}

	staticTyping(0);
}

DbXmlNav::DbXmlNav(ASTNode *step, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_NAV, memMgr),
	  steps_(XQillaAllocator<ASTNode*>(memMgr)),
	  sortAdded_(false)
{
	steps_.push_back(step);
	staticTyping(0);
}

void DbXmlNav::addStep(ASTNode *step)
{
  if(step->getType() == (ASTNode::whichType)DBXML_NAV) {
    Steps &navSteps = ((DbXmlNav*)step)->steps_;
    for(Steps::iterator it2 = navSteps.begin();
        it2 != navSteps.end(); ++it2) {
      steps_.push_back(*it2);
    }
  }
  else {
    steps_.push_back(step);
  }
}

void DbXmlNav::addStepFront(ASTNode* step)
{
  if(step->getType() == (ASTNode::whichType)DBXML_NAV) {
    Steps &navSteps = ((DbXmlNav*)step)->steps_;
    for(Steps::reverse_iterator it2 = navSteps.rbegin();
        it2 != navSteps.rend(); ++it2) {
      steps_.insert(steps_.begin(), *it2);
    }
  }
  else {
    steps_.insert(steps_.begin(), step);
  }
}

ASTNode *DbXmlNav::staticTyping(StaticContext *context)
{
	_src.clear();

	unsigned int props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
		StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE |
		StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE;

	StaticType oldContextItemType = context != 0 ? context->getContextItemType() : 0;

	Steps::iterator begin = steps_.begin();
	Steps::iterator end = steps_.end();
	Steps::iterator it = begin;
	for(; it != end; ++it) {
		if(context != 0)
			*it = (*it)->staticTyping(context);
		const StaticResolutionContext &stepSrc = (*it)->getStaticResolutionContext();
		if(context != 0)
			context->setContextItemType(stepSrc.getStaticType());

		props = XQNav::combineProperties(props, stepSrc.getProperties());

		if((*it)->getType() == (ASTNode::whichType)JOIN) {
			props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED;
		}

		if(it != begin) {
			// Remove context item usage
			_src.addExceptContextFlags(stepSrc);
		}
		else {
			_src.add(stepSrc);
		}
	}

	if(!steps_.empty()) {
		_src.getStaticType() = steps_.back()->getStaticResolutionContext().getStaticType();
	}

	_src.setProperties(props);

	if(context != 0)
		context->setContextItemType(oldContextItemType);

	return this;
}

void DbXmlNav::recalculateSRC()
{
	staticTyping(0);
}

DbXmlResult DbXmlNav::sortIfPossible(const DbXmlResult &result, unsigned int &props, const LocationInfo *location,
	DynamicContext *context)
{
	if(props & StaticResolutionContext::DOCORDER) {
		// It's already sorted
		return result;
	} else if(props & StaticResolutionContext::GROUPED) {
		// If the result is not in document order, but it is
		// grouped, we can efficiently sort it using the GroupedSort
		// iterator
		props |= StaticResolutionContext::DOCORDER;
		return new GroupedSort(result, location);
	} else {
		// We can't sort it efficiently
		return result;
	}
}

DbXmlResult DbXmlNav::sort(const DbXmlResult &result, unsigned int &props, const LocationInfo *location,
	DynamicContext *context)
{
	if(props & StaticResolutionContext::DOCORDER) {
		// It's already sorted
		return result;
// 	} else if(props & StaticResolutionContext::GROUPED) {
// 		// If the result is not in document order, but it is
// 		// grouped, we can efficiently sort it using the GroupedSort
// 		// iterator
// 		props |= StaticResolutionContext::DOCORDER;
// 		return new GroupedSort(result, location);
	} else {
		props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED;
		return new DocumentOrderResult(location, result, context);
	}
}

DbXmlResult DbXmlNav::makeStepResult(DbXmlResult &parent, unsigned int &props, const Steps::const_iterator &it,
	const Steps::const_iterator &end, DynamicContext *context, const LocationInfo *location)
{
	DbXmlResult result(0);

	if(parent.isNull()) {
		result = createDbXmlResult(*it, context);
	} else {
		DbXmlASTNode *dbxmlast = DbXmlASTNode::getDbXmlASTNode(*it);
		if((*it)->getStaticResolutionContext().isContextSizeUsed()) {
			// We need the context size, so convert to a Sequence to work it out
			Sequence seq(parent->toSequence(context));
			result =  new NavStepResult(new SequenceResult(location, seq), *it, seq.getLength());
		} else if(dbxmlast != 0) {
			result = dbxmlast->createResult(parent, props, context);
		} else {
			result = new NavStepResult(parent, *it, 0);
		}
	}

	props = XQNav::combineProperties(props, (*it)->getStaticResolutionContext().getProperties());

	if((*it)->getType() == (ASTNode::whichType)JOIN) {
		props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED;
	}

	const StaticType &st = (*it)->getStaticResolutionContext().getStaticType();
	if(it == (end-1)) {
		// the last step allows either nodes or atomic items
		if(st.containsType(StaticType::NODE_TYPE) &&
			st.containsType(StaticType::ANY_ATOMIC_TYPE)) {
			result = new LastStepCheckResult(location, result);
		}
	}
	else {
		if(st.containsType(StaticType::ANY_ATOMIC_TYPE)) {
			result = new IntermediateStepCheckResult(location, result);
		}
	}

	return result;
}

DbXmlResult DbXmlNav::createResult(const DbXmlResult &contextItems, unsigned int props,
	DynamicContext *context) const
{
	DbXmlResult result = contextItems;

	Steps::const_iterator it = steps_.begin();
	Steps::const_iterator end = steps_.end();

	for(; it != end; ++it) {
		result = makeStepResult(result, props, it, end, context, this);
	}

	return result;
}

/////////////////////////////////////
// GroupedSort
/////////////////////////////////////

DbXmlNav::GroupedSort::GroupedSort(const Result &parent, const LocationInfo *location)
	: ResultImpl(location),
	  parent_(parent),
	  toDo_(true),
	  next_(0),
	  sorted_(0)
{
}

static inline DocID getDocID(const Item::Ptr &item)
{
	const DbXmlNodeImpl *node = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(node != 0);
	return node->getDocID();
}

Item::Ptr DbXmlNav::GroupedSort::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;
		next_ = parent_->next(context);
	}

	Item::Ptr result = sorted_->next(context);
	if(result.isNull() && next_.notNull()) {
		DocID docID = getDocID(next_);

		Sequence seq(next_, context->getMemoryManager());
		while((next_ = parent_->next(context)).notNull() && getDocID(next_) == docID) {
			seq.addItem(next_);
		}

		if(next_.isNull()) {
			parent_ = 0;
		}

		seq.sortIntoDocumentOrder(context);
		sorted_ = seq;
		result = sorted_->next(context);
	}

	return result;
}

string DbXmlNav::GroupedSort::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlNav_GroupedSort/>";
}
