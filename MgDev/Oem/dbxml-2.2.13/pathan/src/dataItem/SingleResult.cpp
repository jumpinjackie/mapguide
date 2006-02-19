/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/dataItem/SingleResult.hpp>

SingleResult::SingleResult(DynamicContext *context)
  : ResultImpl(context),
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
