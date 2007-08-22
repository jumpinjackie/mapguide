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
 * $Id: FollowingAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/FollowingAxis.hpp>

#include <xqilla/items/Node.hpp>

#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>

FollowingAxis::FollowingAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    node_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *FollowingAxis::nextNode(DynamicContext *context)
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    node_ = contextNode_;
  }
  
  if(node_ != 0) {
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *result = 0;

    if(node_ != contextNode_) {
      result = getFirstChild(node_);
    }

    while(result == 0 && node_ != 0) {
      result = getNextSibling(node_);
      if(result == 0) {
        node_ = getParent(node_);
      }
    }

    node_ = result;
  }

  return node_;
}

std::string FollowingAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<following_axis/>";
}
