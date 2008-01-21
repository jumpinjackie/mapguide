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
 * $Id: ResultBuffer.hpp,v 1.9 2006/11/01 16:37:15 jpcs Exp $
 */

#ifndef _RESULTBUFFER_HPP
#define _RESULTBUFFER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/ResultBufferImpl.hpp>

/** A reference counting wrapper for the result buffer */
class XQILLA_API ResultBuffer
{
public:
  ResultBuffer(const Result &result, unsigned int readCount = ResultBufferImpl::UNLIMITED_COUNT);
  ResultBuffer(const Item::Ptr &item, unsigned int readCount = ResultBufferImpl::UNLIMITED_COUNT);
  ResultBuffer(ResultBufferImpl *impl);

  /// Creates a result that returns the same items as the one used to construct this buffer
  Result createResult();

  bool isNull() const
  {
    return _impl.isNull();
  }

private:
  ResultBufferImpl::Ptr _impl;
};

#endif
