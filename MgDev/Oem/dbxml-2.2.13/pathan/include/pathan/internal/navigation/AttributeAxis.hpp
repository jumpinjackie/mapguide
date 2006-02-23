/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATTRIBUTEAXIS_HPP
#define _ATTRIBUTEAXIS_HPP

#include <pathan/internal/navigation/Axis.hpp>

#include <xercesc/dom/DOMNamedNodeMap.hpp>

class PATHAN_EXPORT AttributeAxis : public Axis
{
public:
  AttributeAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;

private:
  XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *nodeMap_;
  unsigned int i_;
};

#endif

