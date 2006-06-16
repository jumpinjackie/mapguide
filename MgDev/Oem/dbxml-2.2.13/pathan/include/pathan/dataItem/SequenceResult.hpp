/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _SEQUENCERESULT_HPP
#define _SEQUENCERESULT_HPP

#include <pathan/dataItem/ResultImpl.hpp>

#include <pathan/Sequence.hpp>

class PATHAN_EXPORT SequenceResult : public ResultImpl
{
public:
  SequenceResult(const Sequence &seq);

  /// Get the next item from the iterator
  virtual Item::Ptr next(DynamicContext *context);

  /// Cast operator to a Sequence
  virtual Sequence toSequence(DynamicContext *context = 0);

  virtual std::string asString(DynamicContext *context, int indent) const;

private:
  Sequence::iterator it_;
  Sequence seq_;
};

#endif
