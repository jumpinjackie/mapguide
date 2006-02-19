/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/DescendantAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/Node.hpp>

DescendantAxis::DescendantAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    descendant_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *DescendantAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    descendant_ = getFirstChild(contextNode_);
  }
  else if(descendant_ != 0) {
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *result = getFirstChild(descendant_);

    while(result == 0) {
      result = getNextSibling(descendant_);
      if(result == 0) {
        descendant_ = getParent(descendant_);
        if(descendant_ == contextNode_) break;
      }
    }

    descendant_ = result;
  }

  return descendant_;
}

std::string DescendantAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<descendant_axis/>";
}
