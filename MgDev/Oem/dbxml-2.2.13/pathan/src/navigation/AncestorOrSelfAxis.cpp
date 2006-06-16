/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/AncestorOrSelfAxis.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/XPath2NSUtils.hpp>

AncestorOrSelfAxis::AncestorOrSelfAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    ancestor_(0)
{
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *AncestorOrSelfAxis::nextNode()
{
  if(toDo_) {
    // initialise
    toDo_ = false;
    ancestor_ = contextNode_;
  }
  else if(ancestor_ != 0) {
    ancestor_ = getParent(ancestor_);
  }

  return ancestor_;
}

std::string AncestorOrSelfAxis::asString(DynamicContext *context, int indent) const
{
  return getIndent(indent) + "<ancestor_or_self_axis/>";
}
