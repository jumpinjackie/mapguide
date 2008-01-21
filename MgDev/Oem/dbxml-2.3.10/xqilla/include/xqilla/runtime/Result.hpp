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
 * $Id: Result.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _RESULT_HPP
#define _RESULT_HPP

#include <string>
#include <xercesc/util/XercesDefs.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/runtime/EmptyResult.hpp>

class Sequence;
class SequenceType;
class DynamicContext;
class StaticType;

/** A reference counting wrapper for the lazily evaluated query result */
class XQILLA_API Result
{
public:
  Result(const Sequence &seq);
  Result(ResultImpl *impl);
  Result(const Result &o);
  Result &operator=(const Result &o);
  ~Result();

  /// Returns the underlying ResultImpl object
  ResultImpl *operator->();

  /// Returns the underlying ResultImpl object
  const ResultImpl *operator->() const;

  /// Returns the underlying ResultImpl object
  ResultImpl *get();

  /// Returns the underlying ResultImpl object
  const ResultImpl *get() const;

  /// Returns true if the underlying pointer is null
  bool isNull() const;

  EmptyResult* getEmpty() const;
  
private:
  ResultImpl *_impl;

  static EmptyResult _empty;
};

inline bool Result::isNull() const
{
  return _impl == 0;
}

inline ResultImpl *Result::get()
{
  if(_impl) return _impl;
  return getEmpty();
}

inline const ResultImpl *Result::get() const
{
  if(_impl) return _impl;
  return getEmpty();
}

inline ResultImpl *Result::operator->()
{
  return get();
}

inline const ResultImpl *Result::operator->() const
{
  return get();
}

inline Result &Result::operator=(const Result &o)
{
  if(o._impl != _impl) {
    if(_impl) _impl->decrementRefCount();
    _impl = o._impl;
    if(_impl) _impl->incrementRefCount();
  }
  return *this;
}

#endif
