/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>
#include <pathan/dataItem/SequenceResult.hpp>

SequenceResult::SequenceResult(const Sequence &seq)
  : ResultImpl(0),
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
