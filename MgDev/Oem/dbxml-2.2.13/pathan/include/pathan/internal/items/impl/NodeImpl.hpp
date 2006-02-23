/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _PATHANNODEIMPL_HPP
#define _PATHANNODEIMPL_HPP

#include <pathan/Node.hpp>
#include <pathan/internal/DOMutils/XMLChCompare.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

class DynamicContext;
class XPath2MemoryManager;
class Sequence;
class DocumentCache;

XERCES_CPP_NAMESPACE_BEGIN
class DatatypeValidator;
class DOMWriter;
class DOMNode;
XERCES_CPP_NAMESPACE_END

class PATHAN_EXPORT NodeImpl : public Node
{
public:
  typedef RefCountPointer<const NodeImpl> Ptr;

  NodeImpl(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context);
  ~NodeImpl();

  virtual void *getInterface(const XMLCh *name) const;

  virtual bool isNode() const;
  virtual bool isAtomicValue() const;
  virtual const XMLCh* asString(const DynamicContext* context) const;
  virtual bool hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const;

  virtual Sequence dmBaseURI(const DynamicContext* context) const;
  virtual const XMLCh* dmNodeKind() const;
  virtual ATQNameOrDerived::Ptr dmNodeName(const DynamicContext* context) const;
  virtual const XMLCh* dmStringValue(const DynamicContext* context) const;
  virtual Sequence dmTypedValue(const DynamicContext* context) const;

  virtual Sequence dmDocumentURI(const DynamicContext* context) const;
  virtual ATQNameOrDerived::Ptr dmTypeName(const DynamicContext* context) const;

  virtual ATBooleanOrDerived::Ptr dmNilled(const DynamicContext* context) const;

  virtual bool lessThan(const Node::Ptr &other, const DynamicContext *context) const;
  virtual bool equals(const Node::Ptr &other) const;
  virtual bool uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const;

  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* getDOMNode() const;

  virtual Node::Ptr dmParent(const DynamicContext* context) const;
  virtual Result dmAttributes(const DynamicContext* context) const;
  virtual Result dmNamespaceNodes(const DynamicContext* context) const;
  virtual Result dmChildren(const DynamicContext *context) const;

  virtual Result getAxisResult(DataItemStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context) const;

  virtual ATBooleanOrDerived::Ptr dmIsId(const DynamicContext* context) const;
  virtual ATBooleanOrDerived::Ptr dmIsIdRefs(const DynamicContext* context) const;

  virtual const XMLCh* getTypeURI() const;
  virtual const XMLCh* getTypeName() const;

  static const XMLCh ls_string[];

protected:
  NodeImpl(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

  virtual void getTypeUriAndName(const XMLCh*& uri, const XMLCh*& name) const;
  virtual void getMemberTypeUriAndName(const XMLCh*& uri, const XMLCh*& name) const;
  Sequence getListTypeTypedValue(XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *dtv, const DynamicContext* context) const;
  void addStringValueToBuffer(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* node, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& buffer) const;

  XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter *fSerializer;

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* fNode;
  const DynamicContext *context_;
};
#endif
