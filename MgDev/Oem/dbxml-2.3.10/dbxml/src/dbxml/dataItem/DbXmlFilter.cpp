//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlFilter.cpp,v 1.21 2006/10/30 17:45:55 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlFilter.hpp"
#include "DbXmlStep.hpp"

#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/context/ContextHelpers.hpp>

using namespace DbXml;
using namespace std;

DbXmlFilter::DbXmlFilter(ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(DBXML_FILTER, memMgr),
	  arg_(arg)
{
	staticTyping(0);
}

ASTNode *DbXmlFilter::staticTyping(StaticContext *context)
{
	_src.clear();

	if(context != 0) {
// 		AutoNodeSetOrderingReset orderReset(context);
		arg_ = arg_->staticTyping(context);
	}
	_src.add(arg_->getStaticResolutionContext());
	_src.contextItemUsed(true);
	_src.setProperties(Join::getJoinTypeProperties(Join::SELF));

	if(context != 0) _src.getStaticType() = context->getContextItemType();
	else _src.getStaticType() = StaticType::NODE_TYPE;

	return this;
}

DbXmlResult DbXmlFilter::createResult(const DbXmlResult &contextItems, unsigned int props, DynamicContext* context) const
{
	DbXmlResult result = contextItems;

	if(result.isNull()) {
		result = new XQContextItem::ContextItemResult(this);
	}

	result = new FilterResult(result, this);

	return result;
}


DbXmlFilter::FilterResult::FilterResult(const DbXmlResult &parent, const DbXmlFilter *filter)
	: DbXmlResultImpl(filter),
	  parent_(parent),
	  arg_(filter->getArgument()),
	  toDo_(true)
{
}

Item::Ptr DbXmlFilter::FilterResult::next(DynamicContext *context)
{
	return filter(parent_->next(context), context);
}

Item::Ptr DbXmlFilter::FilterResult::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	return filter(parent_->seek(did, nid, context), context);
}

Item::Ptr DbXmlFilter::FilterResult::filter(const Item::Ptr &item, DynamicContext *context)
{
	context->testInterrupt();

	AutoContextInfoReset autoReset(context);

	bool contextUsed = arg_->getStaticResolutionContext().isContextItemUsed() ||
		arg_->getStaticResolutionContext().isContextPositionUsed();

	Item::Ptr result = item;
	while(result.notNull()) {
		if(contextUsed || toDo_) {
			toDo_ = false;
			
			context->setContextItem(result);

			// 2) Otherwise, the predicate truth value is the effective boolean value of the
			// predicate expression
			if(arg_->collapseTree(context)->getEffectiveBooleanValue(context, this)) {
				return result;
			} else if(!contextUsed) {
				break;
			}

			autoReset.resetContextInfo();
		}
		result = parent_->next(context);
	}

	parent_ = 0;
	return 0;
}
