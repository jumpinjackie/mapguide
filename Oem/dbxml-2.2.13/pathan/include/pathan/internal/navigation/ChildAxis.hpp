/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _CHILDAXIS_HPP
#define _CHILDAXIS_HPP

#include <pathan/internal/navigation/Axis.hpp>

/**
This class implements the ChildAxis axis as defined by the XPath spec
*/
class PATHAN_EXPORT ChildAxis : public Axis
{
public:
  ChildAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;
private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child_;
};


#endif

