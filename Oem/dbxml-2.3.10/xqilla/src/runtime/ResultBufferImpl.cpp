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
 * $Id: ResultBufferImpl.cpp,v 1.11 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/runtime/ResultBufferImpl.hpp>

const unsigned int ResultBufferImpl::UNLIMITED_COUNT = (unsigned int)-1;

ResultBufferImpl::ResultBufferImpl(const Result &result, unsigned int readCount)
  : _refCount(0),
    _result(result),
    _readCount(0),
    _maxReadCount(readCount)
{
  // Do nothing
}

ResultBufferImpl::ResultBufferImpl(const Item::Ptr &item, unsigned int readCount)
  : _refCount(0),
    _result(0),
    _readCount(0),
    _maxReadCount(readCount)
{
  _items.push_back(item);
}

ResultBufferImpl::~ResultBufferImpl()
{
  // Do nothing
}

Result ResultBufferImpl::createResult()
{
  return new BufferedResult(this);
}

Item::Ptr ResultBufferImpl::item(unsigned int index, DynamicContext *context)
{
  while(!_result.isNull() && index >= _items.size()) {
    const Item::Ptr item = _result->next(context);
    if(item.isNull()) {
      _result = 0;
    }
    else {
      _items.push_back(item);
    }
  }

  if(index >= _items.size()) {
    return 0;
  }
  else {
    return _items[index];
  }
}

BufferedResult::BufferedResult(ResultBufferImpl *impl)
  : ResultImpl(0),
    _impl(impl),
    _pos(0)
{
}

Item::Ptr BufferedResult::next(DynamicContext *context)
{
  return _impl->item(_pos++, context);
}

std::string BufferedResult::asString(DynamicContext *context, int indent) const
{
  return "bufferedresult";
}
