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
 * $Id: SequenceResult.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>
#include <xqilla/runtime/SequenceResult.hpp>

SequenceResult::SequenceResult(const LocationInfo *o, const Sequence &seq)
  : ResultImpl(o),
    seq_(seq)
{
  it_ = seq_.begin();
}

Item::Ptr SequenceResult::next(DynamicContext *context)
{
  if(it_ == seq_.end()) {
    return 0;
  }
  else {
    return *(it_++);
  }
}

Sequence SequenceResult::toSequence(DynamicContext *context)
{
  return seq_;
}

std::string SequenceResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<sequence/>" << std::endl;

  return oss.str();
}
