/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/DescendantOrSelfAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/Node.hpp>

DescendantOrSelfAxis::DescendantOrSelfAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    descendant_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *DescendantOrSelfAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    descendant_ = contextNode_;
  }
  else if(descendant_ != 0) {
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *result = getFirstChild(descendant_);

    while(result == 0 && descendant_ != contextNode_) {
      result = getNextSibling(descendant_);
      if(result == 0) {
        descendant_ = getParent(descendant_);
      }
    }

    descendant_ = result;
  }

  return descendant_;
}

std::string DescendantOrSelfAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<descendant_or_self_axis/>";
}
