/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANDOCUMENTIMPL_HPP
#define __PATHANDOCUMENTIMPL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/internal/XPath2MemoryManagerImpl.hpp>
#include <pathan/internal/ProxyMemoryManager.hpp>

#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/util/XMemory.hpp>


class DynamicContext;
class XPath2Result;

XERCES_CPP_NAMESPACE_BEGIN
class XMLGrammarPool;
XERCES_CPP_NAMESPACE_END  

class PathanDocumentImpl : public XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl
{
public:
  PathanDocumentImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  PathanDocumentImpl(const XMLCh *fNamespaceURI, const XMLCh *qualifiedName, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentType *doctype,
                     XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr);
  ~PathanDocumentImpl();
  
  // weak version, need to create a context from scratch inside
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathExpression* createExpression(const XMLCh* expression,
                                                                                    const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver)
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  
  // create an NS Resolver
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* nodeResolver);
  
  // weak version, need to create a context from scratch inside
  virtual void* evaluate(const XMLCh* expression,
                         XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* contextNode,
                         const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* resolver,
                         unsigned short type,
                         void* reuseableResult)
    throw (XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException, XERCES_CPP_NAMESPACE_QUALIFIER DOMException);
  
  virtual void setGrammarPool(XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *xmlGrammarPool);
  virtual XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *getGrammarPool();

protected:
  /// Used for creating objects returned from DOMXPathEvaluator methods
  ProxyMemoryManager _memMgr;

  XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPool *_xmlGrammarPool;

}; //PathanDocumentImpl


#endif //__PATHANDOCUMENTIMPL_HPP
