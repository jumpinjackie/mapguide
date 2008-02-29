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
 * $Id: LazySequenceResult.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>
#include <xqilla/runtime/LazySequenceResult.hpp>
#include <xqilla/context/DynamicContext.hpp>

LazySequenceResult::LazySequenceResult(const LocationInfo *o, DynamicContext *context)
  : ResultImpl(o),
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
    getResult(seq_, context);
    it_ = seq_.begin();
  }

  return seq_;
}
