/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/Pathan.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>

DataItemLiteral::DataItemLiteral(ItemConstructor *ic, XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
    _itemConstructor(ic)
{
	setType(DataItem::LITERAL);
}

/** Returns true if this DataItem has no predicates, and is an instance of
    DataItemSequence or DataItemLiteral. If the literal value of this DataItem
    is a single DateOrTimeType, then hasTimezone() on it must return true,
    otherwise this method will return false. */
bool DataItemLiteral::isConstantAndHasTimezone(StaticContext *context) const
{
  if(isConstant()) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item = _itemConstructor->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isDateOrTimeTypeValue() &&
       ((const DateOrTimeType::Ptr)item)->hasTimezone()) {
      return true;
    }
  }
  return false;
}

bool DataItemLiteral::isSingleNumericConstant(StaticContext *context) const
{
  if(isConstant()) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item = _itemConstructor->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isNumericValue()) {
      return true;
    }
  }
  return false;
}

DataItem* DataItemLiteral::staticResolution(StaticContext *context) {
  DataItem *result = new (getMemoryManager()) DataItemSequence(_itemConstructor, getMemoryManager());
  result->addPredicates(getPredicates());
  return result->staticResolution(context);
}

Result DataItemLiteral::createResult(DynamicContext* context, int flags) const
{
  return Sequence(_itemConstructor->createItem(context), context->getMemoryManager());
}
