/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _PRECEDINGSIBLINGAXIS_HPP
#define _PRECEDINGSIBLINGAXIS_HPP

#include <pathan/internal/navigation/Axis.hpp>

/**
   This class implements the PrecedingSiblingAxis axis as defined by the XPath spec
*/
class PATHAN_EXPORT PrecedingSiblingAxis : public Axis
{
public:
  PrecedingSiblingAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;
private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *sibling_;
};


#endif

