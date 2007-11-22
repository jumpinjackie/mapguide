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
 * $Id: SequenceResult.hpp,v 1.8 2006/11/01 16:37:15 jpcs Exp $
 */

#ifndef _SEQUENCERESULT_HPP
#define _SEQUENCERESULT_HPP

#include <xqilla/runtime/ResultImpl.hpp>

#include <xqilla/runtime/Sequence.hpp>

class XQILLA_API SequenceResult : public ResultImpl
{
public:
  SequenceResult(const LocationInfo *o, const Sequence &seq);

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
