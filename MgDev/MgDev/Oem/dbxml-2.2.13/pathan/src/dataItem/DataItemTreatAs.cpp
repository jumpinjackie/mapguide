/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <sstream>

#include <pathan/dataItem/DataItemTreatAs.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DataItemTreatAs::DataItemTreatAs(DataItem* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
  _expr(expr),
  _exprType(exprType)
{
	setType(DataItem::TREAT_AS);
}

Result DataItemTreatAs::createResult(DynamicContext* context, int flags) const
{
  return _expr->collapseTree(context, flags & ~(DataItem::RETURN_ONE|DataItem::RETURN_TWO)).matches(_exprType, context);
}

DataItem* DataItemTreatAs::staticResolution(StaticContext *context) {
  return resolveDataItem(_expr, context, true);
}

const DataItem *DataItemTreatAs::getExpression() const {
  return _expr;
}

const SequenceType *DataItemTreatAs::getSequenceType() const {
  return _exprType;
}

void DataItemTreatAs::setExpression(DataItem *item) {
  _expr = item;
}
