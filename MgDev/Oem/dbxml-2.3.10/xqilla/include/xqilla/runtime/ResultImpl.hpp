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
 * $Id: ResultImpl.hpp,v 1.9 2006/11/01 16:37:15 jpcs Exp $
 */

#ifndef _RESULTIMPL_HPP
#define _RESULTIMPL_HPP

#include <string>
#include <xercesc/util/XercesDefs.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xqilla/ast/LocationInfo.hpp>

class SequenceType;
class DynamicContext;
class Sequence;

/** A lazily evaluated query result */
class XQILLA_API ResultImpl : public ReferenceCounted, public LocationInfo
{
public:
  virtual ~ResultImpl() {}

  /// Get the next item from the iterator. Returns null if the is no next value.
  virtual Item::Ptr next(DynamicContext *context) = 0;

  /// Performs iterator specific skip of results
  virtual void skip() {}

  /// Returns the effective boolean value of the sequence. Only works properly before next() has been called.
  virtual bool getEffectiveBooleanValue(DynamicContext* context, const LocationInfo *info);
  /// Returns the effective boolean value of the sequence.
  static bool getEffectiveBooleanValue(const Item::Ptr &first, const Item::Ptr &second, DynamicContext* context, const LocationInfo *info);

  /// Cast operator to a Sequence. Only works properly before next() has been called.
  virtual Sequence toSequence(DynamicContext *context);

  /// Debug method to visualise the Result
  virtual std::string asString(DynamicContext *context, int indent) const = 0;

  static std::string getIndent(int indent);

protected:
  /**
   * Do not allocate this object from a memory manager!
   * It must be created using the heap.
   */
  ResultImpl(const LocationInfo *o)
  {
    setLocationInfo(o);
  }

private:
  ResultImpl(const ResultImpl &) {};
  ResultImpl &operator=(const ResultImpl &) { return *this; };
};

#endif
