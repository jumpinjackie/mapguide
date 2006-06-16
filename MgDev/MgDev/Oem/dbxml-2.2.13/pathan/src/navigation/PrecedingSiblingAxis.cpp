/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/PrecedingSiblingAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>

PrecedingSiblingAxis::PrecedingSiblingAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    sibling_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *PrecedingSiblingAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    sibling_ = getPreviousSibling(contextNode_);
  }
  else if(sibling_ != 0) {
    sibling_ = getPreviousSibling(sibling_);
  }

  return sibling_;
}

std::string PrecedingSiblingAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<preceding_sibling_axis/>";
}
