/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/Pathan.hpp>
#include <pathan/DateOrTimeType.hpp>

/* is this type date or time based?  Return true by default */
bool DateOrTimeType::isDateOrTimeTypeValue() const
{
  return true;
}

