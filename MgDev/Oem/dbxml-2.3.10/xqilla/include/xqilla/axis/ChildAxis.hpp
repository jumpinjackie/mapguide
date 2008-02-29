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
 * $Id: ChildAxis.hpp,v 1.9 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _CHILDAXIS_HPP
#define _CHILDAXIS_HPP

#include <xqilla/axis/Axis.hpp>

/**
This class implements the ChildAxis axis as defined by the XPath spec
*/
class XQILLA_API ChildAxis : public Axis
{
public:
  ChildAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory = Axis::gNodeImplFactory);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode(DynamicContext *context);
  std::string asString(DynamicContext *context, int indent) const;
private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child_;
};


#endif

