/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _SELFAXIS_HPP
#define _SELFAXIS_HPP

#include <pathan/Pathan.hpp>
#include <pathan/dataItem/ResultImpl.hpp>
#include <pathan/Node.hpp>

class PATHAN_EXPORT SelfAxis : public ResultImpl
{
public:
  SelfAxis(Node::Ptr contextNode, DynamicContext *context);
  Item::Ptr next(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;
private:
  bool toDo_;
  Node::Ptr contextNode_;
};

#endif

