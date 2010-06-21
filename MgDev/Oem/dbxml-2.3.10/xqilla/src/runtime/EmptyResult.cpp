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
 * $Id: EmptyResult.cpp,v 1.9 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>
#include <xqilla/runtime/EmptyResult.hpp>

EmptyResult::EmptyResult(const LocationInfo *o)
  : ResultImpl(o)
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
