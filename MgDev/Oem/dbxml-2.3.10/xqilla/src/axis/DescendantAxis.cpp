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
 * $Id: DescendantAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/DescendantAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xqilla/items/Node.hpp>

DescendantAxis::DescendantAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    descendant_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *DescendantAxis::nextNode(DynamicContext *context)
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
