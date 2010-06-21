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
 * $Id: PrecedingSiblingAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/PrecedingSiblingAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>

PrecedingSiblingAxis::PrecedingSiblingAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    sibling_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *PrecedingSiblingAxis::nextNode(DynamicContext *context)
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
