/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/PrecedingAxis.hpp>

#include <pathan/Node.hpp>

#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>

PrecedingAxis::PrecedingAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    parent_(0),
    node_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *PrecedingAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    node_ = contextNode_;
    parent_ = getParent(node_);
  }
  
  if(node_ != 0) {
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *result =
      getPreviousSibling(node_);

    while(result == 0) {
      node_ = getParent(node_);
      if(node_ != 0 && node_ == parent_) {
        parent_ = getParent(parent_);
        result = getPreviousSibling(node_);
      }
      else {
        break;
      }
    }

    while(result != 0) {
      node_ = result;
      result = getLastChild(node_);
    }
  }

  return node_;
}

std::string PrecedingAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<preceding_axis/>";
}
