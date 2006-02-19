/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/SelfAxis.hpp>

#include <pathan/Node.hpp>

SelfAxis::SelfAxis(Node::Ptr contextNode, DynamicContext *context)
  : ResultImpl(context),
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
