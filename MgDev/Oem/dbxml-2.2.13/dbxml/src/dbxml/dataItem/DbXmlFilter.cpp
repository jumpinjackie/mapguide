//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlFilter.cpp,v 1.7 2005/12/16 11:35:44 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlFilter.hpp"
#include "DbXmlNav.hpp"
#include "DbXmlContext.hpp"

#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>

using namespace DbXml;
using namespace std;

DbXmlFilter::DbXmlFilter(DbXmlNav *filterExpr, XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
	  filterExpr_(filterExpr)
{
	setType((DataItem::whichType)DbXmlContext::DBXML_FILTER);
	_src.copy(filterExpr->getStaticResolutionContext());
	_src.getStaticType().flags = StaticType::NODE_TYPE;
	_src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::PEER |
		StaticResolutionContext::SUBTREE | StaticResolutionContext::GROUPED |
		StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE);
}

DataItem* DbXmlFilter::staticResolution(StaticContext *context)
{
	DBXML_ASSERT(0);
	return this;
}

Result DbXmlFilter::createResult(DynamicContext* context, int flags) const
{
	DBXML_ASSERT(0);
	return 0;
}

DbXmlFilter::FilterResult::FilterResult(const Result &parent, unsigned int props,
	const DbXmlNav *filterExpr, DynamicContext *context)
	: ResultImpl(context),
	  parent_(parent),
	  props_(props),
	  filterExpr_(filterExpr),
	  toDo_(true),
	  item_(0),
	  result_(0)
{
}

Item::Ptr DbXmlFilter::FilterResult::next(DynamicContext *context)
{
	if(toDo_) {
		toDo_ = false;
		result_ = filterExpr_->createResultForDbXmlFilter(new Stub(this, context), props_, context, DataItem::UNORDERED);

		Predicates::const_iterator it = filterExpr_->getPredicates().begin();
		Predicates::const_iterator end = filterExpr_->getPredicates().end();
		for(; it != end; ++it) {
			result_ = filterExpr_->createPredicateResult(result_, *it, context);
		}
	} else {
		// "fast forward" the result_ iterator to the next result
		// we care about
		result_.skip();
	}

	Item::Ptr tmp = result_.next(context);
	while(tmp != NULLRCP && item_ == NULLRCP) {
		tmp = result_.next(context);
	}

	if(tmp == NULLRCP) {
		result_ = 0;
		return 0;
	}

	tmp = item_;
	item_ = 0;
	return tmp;
}

void DbXmlFilter::FilterResult::skip()
{
	parent_.skip();
}

string DbXmlFilter::FilterResult::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlFilter_FilterResult/>";
}

DbXmlFilter::FilterResult::Stub::Stub(FilterResult *owner, DynamicContext *context)
	: ResultImpl(context),
	  owner_(owner)
{
}

Item::Ptr DbXmlFilter::FilterResult::Stub::next(DynamicContext *context)
{
	owner_->item_ = owner_->parent_.next(context);

	if(owner_->item_ == NULLRCP) {
		owner_->parent_ = 0;
	}

	return owner_->item_;
}

string DbXmlFilter::FilterResult::Stub::asString(DynamicContext *context, int indent) const
{
	return "<DbXmlFilter_FilterResult_Stub/>";
}

