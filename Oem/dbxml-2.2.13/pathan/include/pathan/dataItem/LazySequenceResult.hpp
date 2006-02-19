/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _LAZYSEQUENCERESULT_HPP
#define _LAZYSEQUENCERESULT_HPP

#include <pathan/dataItem/ResultImpl.hpp>

#include <pathan/Sequence.hpp>

class PATHAN_EXPORT LazySequenceResult : public ResultImpl
{
public:
  LazySequenceResult(DynamicContext *context);

  /// Get the next item from the iterator
  virtual Item::Ptr next(DynamicContext *context);

  /// Called lazily to get the Sequence to iterate over
  virtual void getResult(Sequence &toFill, DynamicContext *context) const = 0;

  /// Cast operator to a Sequence
  virtual Sequence toSequence(DynamicContext *context = 0);

private:
  bool toInit_;
  Sequence::iterator it_;
  Sequence seq_;
};

#endif
