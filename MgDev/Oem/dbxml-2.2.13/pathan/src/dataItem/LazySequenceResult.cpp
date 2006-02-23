/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>
#include <pathan/dataItem/LazySequenceResult.hpp>
#include <pathan/DynamicContext.hpp>

LazySequenceResult::LazySequenceResult(DynamicContext *context)
  : ResultImpl(context),
    toInit_(true),
    seq_(context->getMemoryManager())
{
}

Item::Ptr LazySequenceResult::next(DynamicContext *context)
{
  if(toInit_) {
    toInit_ = false;
    getResult(seq_, context);
    it_ = seq_.begin();
  }

  if(it_ == seq_.end()) {
    return 0;
  }
  else {
    return *(it_++);
  }
}

Sequence LazySequenceResult::toSequence(DynamicContext *context)
{
  if(toInit_) {
    toInit_ = false;
    if(context == 0) context = context_;
    getResult(seq_, context);
    it_ = seq_.begin();
  }

  return seq_;
}
