/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _PRECEDINGAXIS_HPP
#define _PRECEDINGAXIS_HPP

#include <pathan/internal/navigation/Axis.hpp>

/**
This class implements the PrecedingAxis axis as defined by the XPath spec
*/
class PATHAN_EXPORT PrecedingAxis : public Axis
{
public:
  PrecedingAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;
private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *parent_;
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node_;
};


#endif

