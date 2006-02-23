/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/ParentAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>
#include <pathan/Node.hpp>

ParentAxis::ParentAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *ParentAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    return getParent(contextNode_);
  }
  else {
    return 0;
  }
}

std::string ParentAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<parent_axis/>";
}
