/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DESCENDANTORSELFAXIS_HPP
#define _DESCENDANTORSELFAXIS_HPP

#include <pathan/internal/navigation/Axis.hpp>

class PATHAN_EXPORT DescendantOrSelfAxis : public Axis
{
public:
  DescendantOrSelfAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context,
       const AxisNodeFactory &factory = Axis::gNodeImplFactory);
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode();
  std::string asString(DynamicContext *context, int indent) const;
private:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *descendant_;
};


#endif

