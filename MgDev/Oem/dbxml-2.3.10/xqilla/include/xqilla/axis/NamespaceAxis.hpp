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
 * $Id: NamespaceAxis.hpp,v 1.11 2006/11/27 17:51:17 amassari Exp $
 */

#ifndef _NAMESPACEAXIS_HPP
#define _NAMESPACEAXIS_HPP

#include <set>

#include <xqilla/axis/Axis.hpp>

#include <xercesc/dom/DOMNamedNodeMap.hpp>

class XQILLA_API NamespaceAxis : public Axis
{
public:
  NamespaceAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory = Axis::gNodeImplFactory);

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;

private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node_, *originalNode_;
  XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *nodeMap_;
  unsigned int i_;

  typedef std::set<const XMLCh*, XMLChSort> DoneSet;
  DoneSet done_;
  bool defNsTested_, defXmlNs_;
};

#endif

