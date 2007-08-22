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
 * $Id: SelfAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/SelfAxis.hpp>

#include <xqilla/items/Node.hpp>

SelfAxis::SelfAxis(const LocationInfo *info, Node::Ptr contextNode)
  : ResultImpl(info),
    toDo_(true),
    contextNode_(contextNode)
{
}

Item::Ptr SelfAxis::next(DynamicContext *context)
{
  if(toDo_) {
    toDo_ = false;
    return contextNode_;
  }
  else {
    return 0;
  }
}

std::string SelfAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<self_axis/>";
}
