/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/dataItem/DataItemIf.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

DataItemIf::DataItemIf(DataItem* test, DataItem* whenTrue, DataItem* whenFalse, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
  _test(test),
  _whenTrue(whenTrue),
  _whenFalse(whenFalse)
{
	setType(DataItem::IF);
}

Result DataItemIf::createResult(DynamicContext* context, int flags) const
{
  return new IfResult(this, flags, context);
}

DataItem* DataItemIf::staticResolution(StaticContext *context) {
  _test = _test->staticResolution(context);

  if(_test->isConstant()) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    if(_test->collapseTree(dContext).getEffectiveBooleanValue(dContext)) {
      _whenTrue->addPredicates(getPredicates());
      return _whenTrue->staticResolution(context);
    }
    else {
      _whenFalse->addPredicates(getPredicates());
      return _whenFalse->staticResolution(context);
    }
  }
  else {
    _src.add(_test->getStaticResolutionContext());

    _whenTrue = _whenTrue->staticResolution(context);
    _src.getStaticType() = _whenTrue->getStaticResolutionContext().getStaticType();
    _src.add(_whenTrue->getStaticResolutionContext());

    _whenFalse = _whenFalse->staticResolution(context);
    _src.getStaticType().typeUnion(_whenFalse->getStaticResolutionContext().getStaticType());
    _src.add(_whenFalse->getStaticResolutionContext());

    return resolvePredicates(context);
  }
}

const DataItem *DataItemIf::getTest() const {
  return _test;
}

const DataItem *DataItemIf::getWhenTrue() const {
  return _whenTrue;
}

const DataItem *DataItemIf::getWhenFalse() const {
  return _whenFalse;
}

void DataItemIf::setTest(DataItem *item)
{
  _test = item;
}

void DataItemIf::setWhenTrue(DataItem *item)
{
  _whenTrue = item;
}

void DataItemIf::setWhenFalse(DataItem *item)
{
  _whenFalse = item;
}

DataItemIf::IfResult::IfResult(const DataItemIf *di, int flags, DynamicContext *context)
  : ResultImpl(context),
    _flags(flags),
    _di(di),
    _results(0)
{
}

Item::Ptr DataItemIf::IfResult::next(DynamicContext *context)
{
  if(_results.isNull()) {
    if(_di->getTest()->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO).getEffectiveBooleanValue(context)) {
      _results = _di->getWhenTrue()->collapseTree(context, _flags);
    }
    else {
      _results = _di->getWhenFalse()->collapseTree(context, _flags);
    }
  }

  return _results.next(context);
}

std::string DataItemIf::IfResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<if/>" << std::endl;

  return oss.str();
}
