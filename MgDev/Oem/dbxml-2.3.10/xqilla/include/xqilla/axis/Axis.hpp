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
 * $Id: Axis.hpp,v 1.10 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _AXIS_HPP
#define _AXIS_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/items/impl/NodeImpl.hpp>

#include <xercesc/dom/DOMNode.hpp>

class NodeTest;

class AxisNodeFactory
{
public:
  virtual ~AxisNodeFactory() {}
  virtual const Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context) const = 0;
};

template<class TYPE>
class AxisNodeFactoryTemplate : public AxisNodeFactory
{
public:
  virtual const Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context) const
  {
    return new TYPE(node, context);
  }
};


class XQILLA_API Axis : public ResultImpl
{
public:
  static AxisNodeFactoryTemplate<NodeImpl> gNodeImplFactory;

  Axis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory);
  Item::Ptr next(DynamicContext *context);

  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nextNode(DynamicContext *context) = 0;

  static const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getParent(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
  static const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFirstChild(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
  static const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getLastChild(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
  static const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getNextSibling(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
  static const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getPreviousSibling(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

protected:
  Node::Ptr nodeObj_;
  const AxisNodeFactory &factory_;

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode_;
  const NodeTest *nodeTest_;
  bool toDo_;
};

#endif

