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
 * $Id: ResultBuffer.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/runtime/ResultBuffer.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/context/DynamicContext.hpp>

ResultBuffer::ResultBuffer(const Result &result, unsigned int readCount)
  : _impl(new ResultBufferImpl(result, readCount))
{
}

ResultBuffer::ResultBuffer(const Item::Ptr &item, unsigned int readCount)
  : _impl(new ResultBufferImpl(item, readCount))
{
}

ResultBuffer::ResultBuffer(ResultBufferImpl *impl)
  : _impl(impl)
{
}

Result ResultBuffer::createResult()
{
  if(_impl.isNull()) return 0;

  Result result = _impl->createResult();

  if(_impl->getMaxReadCount() != ResultBufferImpl::UNLIMITED_COUNT &&
     _impl->incrementReadCount() >= _impl->getMaxReadCount()) {
    // We've reached the maximum read count, so noone
    // else will want to read from this ResultBuffer
    _impl = 0;
  }

  return result;
}
