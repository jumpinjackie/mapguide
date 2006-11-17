/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/Pathan.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/DynamicErrorException.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

DataItemContextItem::DataItemContextItem(XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr)
{
	setType(DataItem::CONTEXT_ITEM);
}

DataItemContextItem::~DataItemContextItem() {
	//no-op
}

DataItem* DataItemContextItem::staticResolution(StaticContext *context)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE);
  _src.getStaticType() = context->getContextItemType();
  _src.contextItemUsed(true);
  return resolvePredicates(context);
}

Result DataItemContextItem::createResult(DynamicContext* context, int flags) const
{
  return new ContextItemResult(context);
}

DataItemContextItem::ContextItemResult::ContextItemResult(DynamicContext *context)
  : SingleResult(context)
{
}

Item::Ptr DataItemContextItem::ContextItemResult::getSingleResult(DynamicContext *context) const
{
  const Item::Ptr item = context->getContextItem();
  if(item == NULLRCP) {
    DSLthrow(DynamicErrorException,X("DataItemContextItem::ContextItemResult::getSingleResult"),
             X("It is an error for the context item to be undefined when using it [err:XPDY0002]"));
  }
  return item;
}

std::string DataItemContextItem::ContextItemResult::asString(DynamicContext *context, int indent) const
{
  return "<contextitemresult/>";
}
