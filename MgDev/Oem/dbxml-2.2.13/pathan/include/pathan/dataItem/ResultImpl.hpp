/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _RESULTIMPL_HPP
#define _RESULTIMPL_HPP

#include <string>
#include <xercesc/util/XercesDefs.hpp>

#include "../Pathan.hpp"
#include "../Item.hpp"
#include "../ReferenceCounted.hpp"

class SequenceType;
class DynamicContext;
class Sequence;

/** A lazily evaluated query result */
class PATHAN_EXPORT ResultImpl : public ReferenceCounted
{
public:
  /**
   * Do not allocate this object from a memory manager!
   * It must be created using the heap.
   * @param context Is used for toSequence only
   */
  ResultImpl(DynamicContext *context);
  virtual ~ResultImpl();

  /// Get the next item from the iterator. Returns null if the is no next value.
  virtual Item::Ptr next(DynamicContext *context) = 0;

  /// Performs iterator specific skip of results
  virtual void skip();

  /// Returns the effective boolean value of the sequence. Only works properly before next() has been called.
  virtual bool getEffectiveBooleanValue(DynamicContext* context);
  /// Returns the effective boolean value of the sequence.
  static bool getEffectiveBooleanValue(const Item::Ptr &first, const Item::Ptr &second, DynamicContext* context);

  /// Cast operator to a Sequence. Only works properly before next() has been called.
  virtual Sequence toSequence(DynamicContext *context = 0);

  /// Debug method to visualise the Result
  virtual std::string asString(DynamicContext *context, int indent) const = 0;

  static std::string getIndent(int indent);

protected:
  ResultImpl(const ResultImpl &) {};
  ResultImpl &operator=(const ResultImpl &) { return *this; };

  DynamicContext *context_; ///< Not to be used by anything other than toSequence()!!
};

#endif
