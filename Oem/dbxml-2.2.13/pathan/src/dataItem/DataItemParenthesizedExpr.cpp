/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/dataItem/DataItemParenthesizedExpr.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>

DataItemParenthesizedExpr::DataItemParenthesizedExpr(XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr), _dataItems(PathanAllocator<DataItem*>(memMgr))
{
  setType(DataItem::PARENTHESIZED);
}

Result DataItemParenthesizedExpr::createResult(DynamicContext* context, int flags) const
{
  return new ParenthesizedResult(this, flags, context);
}

void DataItemParenthesizedExpr::addItem(DataItem* di) {

	_dataItems.push_back(di);
}

DataItem* DataItemParenthesizedExpr::staticResolution(StaticContext *context) {
  // Return a blank DataItemSequence if we have no children
  if(_dataItems.empty()) {
    return new (getMemoryManager()) DataItemSequence(getMemoryManager());
  }

  // Dissolve ourselves if we have only one child
  if(_dataItems.size() == 1) {
    DataItem *result = _dataItems.front();
    result->addPredicates(getPredicates());
    return result->staticResolution(context);
  }

  _src.getStaticType().flags = 0;

  bool allConstant = true;
  for(VectorOfDataItems::iterator i = _dataItems.begin(); i != _dataItems.end(); ++i) {
    *i = (*i)->staticResolution(context);
    _src.getStaticType().typeUnion((*i)->getStaticResolutionContext().getStaticType());
    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstant()) {
      allConstant = false;
    }
  }

  if(allConstant) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

const VectorOfDataItems &DataItemParenthesizedExpr::getChildren() const {
  return _dataItems;
}

DataItemParenthesizedExpr::ParenthesizedResult::ParenthesizedResult(const DataItemParenthesizedExpr *di, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _di(di),
    _i(di->getChildren().begin()),
    _result(0)
{
}

Item::Ptr DataItemParenthesizedExpr::ParenthesizedResult::next(DynamicContext *context)
{
  Item::Ptr item = _result.next(context);

  while(item == NULLRCP) {
    if(_i != _di->getChildren().end()) {
      _result = (*_i++)->collapseTree(context, _flags);
      item = _result.next(context);
    }
    else {
      _result  = 0;
      break;
    }
  }

  return item;
}

std::string DataItemParenthesizedExpr::ParenthesizedResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<parenthesized/>" << std::endl;

  return oss.str();
}
