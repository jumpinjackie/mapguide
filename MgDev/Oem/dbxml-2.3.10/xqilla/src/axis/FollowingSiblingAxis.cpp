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
 * $Id: FollowingSiblingAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/FollowingSiblingAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xqilla/items/Node.hpp>

FollowingSiblingAxis::FollowingSiblingAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    node_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *FollowingSiblingAxis::nextNode(DynamicContext *context)
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    node_ = getNextSibling(contextNode_);
  }
  else if(node_ != 0) {
    node_ = getNextSibling(node_);
  }

  return node_;
}

std::string FollowingSiblingAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<following_sibling_axis/>";
}
