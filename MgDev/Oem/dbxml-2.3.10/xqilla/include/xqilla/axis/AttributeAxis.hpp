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
 * $Id: AttributeAxis.hpp,v 1.9 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _ATTRIBUTEAXIS_HPP
#define _ATTRIBUTEAXIS_HPP

#include <xqilla/axis/Axis.hpp>

#include <xercesc/dom/DOMNamedNodeMap.hpp>

class XQILLA_API AttributeAxis : public Axis
{
public:
  AttributeAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory = Axis::gNodeImplFactory);

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;

private:
  XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *nodeMap_;
  unsigned int i_;
};

#endif

