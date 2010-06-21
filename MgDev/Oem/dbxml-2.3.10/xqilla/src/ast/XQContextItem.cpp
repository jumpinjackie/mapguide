/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XQContextItem.cpp,v 1.11 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/DynamicErrorException.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

XQContextItem::XQContextItem(XPath2MemoryManager* memMgr)
	: ASTNodeImpl(memMgr)
{
	setType(ASTNode::CONTEXT_ITEM);
}

XQContextItem::~XQContextItem() {
	//no-op
}

ASTNode* XQContextItem::staticResolution(StaticContext *context)
{
  return this;
}

ASTNode *XQContextItem::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
	  StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::SAMEDOC |
	  StaticResolutionContext::ONENODE | StaticResolutionContext::SELF);
  _src.getStaticType() = context->getContextItemType();
  _src.contextItemUsed(true);
  return this;
}

Result XQContextItem::createResult(DynamicContext* context, int flags) const
{
  return new ContextItemResult(this);
}

XQContextItem::ContextItemResult::ContextItemResult(const LocationInfo *location)
  : SingleResult(location)
{
}

Item::Ptr XQContextItem::ContextItemResult::getSingleResult(DynamicContext *context) const
{
  const Item::Ptr item = context->getContextItem();
  if(item == NULLRCP) {
    XQThrow(DynamicErrorException,X("XQContextItem::ContextItemResult::getSingleResult"),
             X("It is an error for the context item to be undefined when using it [err:XPDY0002]"));
  }
  return item;
}

std::string XQContextItem::ContextItemResult::asString(DynamicContext *context, int indent) const
{
  return "<contextitemresult/>";
}
