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
 * $Id: XQillaDocumentImpl.hpp,v 1.5.2.1 2007/01/19 18:52:17 jpcs Exp $
 */

#ifndef __XQILLADOCUMENTIMPL_HPP
#define __XQILLADOCUMENTIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>
#include <xqilla/framework/ProxyMemoryManager.hpp>

#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/util/XMemory.hpp>


class DynamicContext;
class XPath2Result;

XERCES_CPP_NAMESPACE_BEGIN
class XMLGrammarPool;
XERCES_CPP_NAMESPACE_END  

class XQillaDocumentImpl : public XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl
{
public:
  XQillaDocumentImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  XQillaDocumentImpl(const XMLCh *fNamespaceURI, const XMLCh *qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *doctype,
                     XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr);
  ~XQillaDocumentImpl();
  
  // weak version, need to create a context from scratch inside
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathExpression*
  createExpression(const XMLCh* expression, const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver);
  
  // create an NS Resolver
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver*
  createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* nodeResolver);
  
  // weak version, need to create a context from scratch inside
  virtual void* evaluate(const XMLCh* expression,
                         XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                         const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver,
                         unsigned short type,
                         void* reuseableResult);
  
  virtual void setGrammarPool(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGrammarPool);
  virtual XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *getGrammarPool();

protected:
  /// Used for creating objects returned from DOMXPathEvaluator methods
  ProxyMemoryManager _memMgr;

  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *_xmlGrammarPool;

}; //XQillaDocumentImpl


#endif //__XQILLADOCUMENTIMPL_HPP
