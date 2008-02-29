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
 * $Id: XQLiteral.cpp,v 1.11 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

XQLiteral::XQLiteral(ItemConstructor *ic, XPath2MemoryManager* memMgr)
	: ASTNodeImpl(memMgr),
    _itemConstructor(ic)
{
	setType(ASTNode::LITERAL);
}

/** Returns true if this XQ has no predicates, and is an instance of
    XQSequence or XQLiteral. If the literal value of this XQ
    is a single DateOrTimeType, then !hasTimezone() on it must return true,
    otherwise this method will return false. */
bool XQLiteral::isDateOrTimeAndHasNoTimezone(StaticContext *context) const
{
  AutoDelete<DynamicContext> dContext(context->createDynamicContext());
  dContext->setMemoryManager(context->getMemoryManager());

  Item::Ptr item = _itemConstructor->createItem(dContext);
  if(item->isAtomicValue() &&
     ((const AnyAtomicType::Ptr)item)->isDateOrTimeTypeValue() &&
     !((const DateOrTimeType::Ptr)item)->hasTimezone()) {
    return true;
  }
  return false;
}

bool XQLiteral::isSingleNumericConstant(StaticContext *context) const
{
  AutoDelete<DynamicContext> dContext(context->createDynamicContext());
  dContext->setMemoryManager(context->getMemoryManager());

  Item::Ptr item = _itemConstructor->createItem(dContext);
  if(item->isAtomicValue() &&
     ((const AnyAtomicType::Ptr)item)->isNumericValue()) {
    return true;
  }
  return false;
}

ASTNode* XQLiteral::staticResolution(StaticContext *context) {
  XPath2MemoryManager *mm = context->getMemoryManager();
  ASTNode *result = new (mm) XQSequence(_itemConstructor, mm);
  result->setLocationInfo(this);
  return result->staticResolution(context);
}

ASTNode *XQLiteral::staticTyping(StaticContext *context)
{
  // Should never happen
  return this;
}

Result XQLiteral::createResult(DynamicContext* context, int flags) const
{
  return Sequence(_itemConstructor->createItem(context), context->getMemoryManager());
}
