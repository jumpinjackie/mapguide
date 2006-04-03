/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/ChildAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/Node.hpp>

ChildAxis::ChildAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    child_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *ChildAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    child_ = getFirstChild(contextNode_);
  }
  else if(child_ != 0) {
    child_ = getNextSibling(child_);
  }

  return child_;
}

std::string ChildAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<child_axis/>";
}
