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
 * $Id: Result.cpp,v 1.11 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <xqilla/runtime/Result.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>

EmptyResult Result::_empty(0);

Result::Result(const Sequence &seq)
  : _impl(new SequenceResult(0, seq))
{
  _impl->incrementRefCount();
}

Result::Result(ResultImpl *impl)
  : _impl(impl)
{
  if(_impl) _impl->incrementRefCount();
}

Result::Result(const Result &o)
  : _impl(o._impl)
{
  if(_impl) _impl->incrementRefCount();
}

Result::~Result()
{
  if(_impl) _impl->decrementRefCount();
}

EmptyResult* Result::getEmpty() const
{
  return &_empty;
}

