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
 * $Id: XPath2ResultImpl.hpp,v 1.4 2006/11/01 16:37:18 jpcs Exp $
 */

#ifndef __XPATH2RESULTIMPL_HPP
#define __XPATH2RESULTIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/dom-api/XPath2Result.hpp>

#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/runtime/Result.hpp>

#include <xercesc/dom/impl/DOMDocumentImpl.hpp>

class DynamicContext;
class XQQuery;
class XQillaExpressionImpl;

class XQILLA_API XPath2ResultImpl : public XPath2Result, XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfo
{
public:
  XPath2ResultImpl(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                   DynamicContext *staticContext,
                   XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                   XQillaExpressionImpl *exprImpl = 0);

  virtual ~XPath2ResultImpl();

  virtual bool isNode() const;
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMTypeInfo *getTypeInfo() const;


  virtual int asInt() const;
  virtual double asDouble() const;
  virtual const XMLCh* asString() const;
  virtual bool asBoolean() const;
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* asNode() const;

  /// DOMTypeInfo method
  virtual const XMLCh* getName() const;
  /// DOMTypeInfo method
  virtual const XMLCh* getNamespace() const;

protected:
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* _createdWith;

  DynamicContext* _context;
  Item::Ptr _currentItem;

  XQillaExpressionImpl *_exprToDelete;
};

class XQILLA_API XPath2FirstResultImpl : public XPath2ResultImpl
{
public:

  XPath2FirstResultImpl(const XQQuery *expression,
                        XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                        DynamicContext *staticContext,
                        XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                        XQillaExpressionImpl *exprImpl = 0);

  virtual ~XPath2FirstResultImpl();
  virtual void release();

  virtual ResultType getResultType() const;

  virtual bool getInvalidIteratorState() const;
  virtual bool iterateNext();

  virtual unsigned long getSnapshotLength() const;
  virtual bool snapshotItem(unsigned long index);
};

class XQILLA_API XPath2SnapshotResultImpl : public XPath2ResultImpl
{
public:

  XPath2SnapshotResultImpl(const XQQuery *expression,
                           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                           DynamicContext *staticContext,
                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                           XQillaExpressionImpl *exprImpl = 0);

  virtual ~XPath2SnapshotResultImpl();
  virtual void release();

  virtual ResultType getResultType() const;

  virtual bool getInvalidIteratorState() const;
  virtual bool iterateNext();

  virtual unsigned long getSnapshotLength() const;
  virtual bool snapshotItem(unsigned long index);

private:
  Sequence *_sequence;
};

class XQILLA_API XPath2IteratorResultImpl : public XPath2ResultImpl
{
public:
  XPath2IteratorResultImpl(const XQQuery *expression,
                           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                           DynamicContext *staticContext,
                           XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr,
                           XQillaExpressionImpl *exprImpl = 0);

  virtual ~XPath2IteratorResultImpl();
  virtual void release();

  virtual ResultType getResultType() const;

  virtual bool getInvalidIteratorState() const;
  virtual bool iterateNext();

  virtual unsigned long getSnapshotLength() const;
  virtual bool snapshotItem(unsigned long index);

private:
  Result _results;

  const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl* _documentRoot;
  int _changes;
};

#endif
