/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _NAMESPACEAXIS_HPP
#define _NAMESPACEAXIS_HPP

#include <set>

#include <pathan/internal/navigation/Axis.hpp>

#include <xercesc/dom/DOMNamedNodeMap.hpp>

class PATHAN_EXPORT NamespaceAxis : public Axis
{
public:
  NamespaceAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;

private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node_;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *nodeMap_;
  unsigned int i_;

  typedef std::set<const XMLCh*, XMLChSort> DoneSet;
  DoneSet done_;
};

#endif

