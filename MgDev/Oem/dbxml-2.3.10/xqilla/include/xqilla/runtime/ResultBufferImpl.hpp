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
 * $Id: ResultBufferImpl.hpp,v 1.3 2006/11/01 16:37:15 jpcs Exp $
 */

#ifndef _RESULTBUFFERIMPL_HPP
#define _RESULTBUFFERIMPL_HPP

#include <vector>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>

/** A buffer for lazily evaluated query results. This object is not thread safe */
class XQILLA_API ResultBufferImpl : public ReferenceCounted
{
public:
  typedef RefCountPointer<ResultBufferImpl> Ptr;

  static const unsigned int UNLIMITED_COUNT;

  /**
   * Do not allocate this object from a memory manager!
   * It must be created using the heap.
   */
  ResultBufferImpl(const Result &result, unsigned int readCount = UNLIMITED_COUNT);
  ResultBufferImpl(const Item::Ptr &item, unsigned int readCount = UNLIMITED_COUNT);
  ~ResultBufferImpl();

  /// Creates a result that returns the same items as the one used to construct this buffer
  Result createResult();

  unsigned int getMaxReadCount() const { return _maxReadCount; }
  unsigned int incrementReadCount() { return ++_readCount; }

private:
  ResultBufferImpl(const ResultBufferImpl &);
  ResultBufferImpl &operator=(const ResultBufferImpl &);

  Item::Ptr item(unsigned int index, DynamicContext *context);

  friend class BufferedResult;

  typedef std::vector<Item::Ptr> ItemList;

  unsigned int _refCount;
  Result _result;
  ItemList _items;

  unsigned int _readCount;
  unsigned int _maxReadCount;
};

class BufferedResult : public ResultImpl
{
public:
  BufferedResult(ResultBufferImpl *impl);
  Item::Ptr next(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;
private:
  ResultBufferImpl::Ptr _impl;
  unsigned int _pos;
};


#endif
