/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>
#include <pathan/dataItem/EmptyResult.hpp>

EmptyResult::EmptyResult(DynamicContext *context)
  : ResultImpl(context)
{
}

Item::Ptr EmptyResult::next(DynamicContext *context)
{
  return 0;
}

std::string EmptyResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<empty/>" << std::endl;

  return oss.str();
}
