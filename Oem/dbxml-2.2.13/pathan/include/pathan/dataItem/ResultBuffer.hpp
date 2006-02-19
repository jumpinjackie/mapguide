/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _RESULTBUFFER_HPP
#define _RESULTBUFFER_HPP

#include "../Pathan.hpp"
#include "ResultImpl.hpp"

class Result;
class ResultBufferImpl;
class Item;

/** A reference counting wrapper for the result buffer */
class PATHAN_EXPORT ResultBuffer
{
public:
  ResultBuffer(const Result &result);
  ResultBuffer(ResultBufferImpl *impl);
  ResultBuffer(const ResultBuffer &o);
  ResultBuffer &operator=(const ResultBuffer &o);
  ~ResultBuffer();

  /// Creates a result that returns the same items as the one used to construct this buffer
  Result createResult(DynamicContext *context) const;

  bool isNull() const
  {
    return _impl == 0;
  }

private:
  class BufferedResult : public ResultImpl
  {
  public:
    BufferedResult(ResultBufferImpl *impl, DynamicContext *context);
    ~BufferedResult();

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    ResultBufferImpl *_impl;
    unsigned int _pos;
  };

  ResultBufferImpl *_impl;
};

#endif
