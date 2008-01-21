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
 * $Id: SingleResult.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/runtime/SingleResult.hpp>

SingleResult::SingleResult(const LocationInfo *o)
  : ResultImpl(o),
    toDo_(true)
{
}

Item::Ptr SingleResult::next(DynamicContext *context)
{
  if(toDo_) {
    toDo_ = false;
    return getSingleResult(context);
  }
  else {
    return 0;
  }
}
