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
 * $Id: XQScopedNamespace.hpp,v 1.6 2006/11/01 16:37:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQScopedNamespace.hpp: interface for the XQScopedNamespace class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_)
#define AFXQ_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/dom-api/impl/XQillaNSResolverImpl.hpp>
#include <xqilla/context/StaticContext.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMXPathNSResolver;
class DOMNode;
XERCES_CPP_NAMESPACE_END

class XQILLA_API XQScopedNamespace : public XQillaNSResolverImpl
{
public:
  XQScopedNamespace(XPath2MemoryManager* memMgr, const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* prevScope);
  virtual ~XQScopedNamespace();

  virtual const XMLCh* lookupNamespaceURI(const XMLCh* prefix) const;
  virtual const XMLCh* lookupPrefix(const XMLCh* uri) const;

  void setNodeContext(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* node);
protected:
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* _prevScope;
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* _ctxNode;
};

class XQILLA_API AutoNsScopeReset
{
public:
  AutoNsScopeReset(StaticContext* context, XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* newResolver)
  {
    _context=context;
    _oldNSResolver=_context->getNSResolver();
    _defaultElementAndTypeNS=context->getDefaultElementAndTypeNS();
    _context->setNSResolver(newResolver);
  }

  ~AutoNsScopeReset()
  {
    _context->setNSResolver(_oldNSResolver);
    _context->setDefaultElementAndTypeNS(_defaultElementAndTypeNS);
  }

protected:
  StaticContext* _context;
  const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* _oldNSResolver;
  const XMLCh *_defaultElementAndTypeNS;
  
};

#endif // !defined(AFXQ_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_)
