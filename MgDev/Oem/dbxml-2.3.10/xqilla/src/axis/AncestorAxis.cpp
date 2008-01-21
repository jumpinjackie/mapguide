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
 * $Id: AncestorAxis.cpp,v 1.9 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/AncestorAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>

AncestorAxis::AncestorAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    ancestor_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *AncestorAxis::nextNode(DynamicContext *context)
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    ancestor_ = contextNode_;
  }

  if(ancestor_ != 0) {
    ancestor_ = getParent(ancestor_);
  }

  return ancestor_;
}

std::string AncestorAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<ancestor_axis/>";
}
