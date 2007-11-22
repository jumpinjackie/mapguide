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
 * $Id: XQillaNSResolverImpl.hpp,v 1.6 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef __XQILLANSRESOLVERIMPL_HPP
#define __XQILLANSRESOLVERIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xqilla/dom-api/XQillaNSResolver.hpp>
#include <xercesc/util/RefHashTableOf.hpp>

class XPath2MemoryManager;

class XQILLA_API XQillaNSResolverImpl : public XQillaNSResolver
{
public:
	XQillaNSResolverImpl(XPath2MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *resolverNode);
	~XQillaNSResolverImpl();

	virtual const XMLCh* lookupNamespaceURI(const XMLCh* prefix) const;
  virtual const XMLCh* lookupPrefix(const XMLCh* uri) const;
  
  virtual void addNamespaceBinding(const XMLCh* prefix, const XMLCh* uri);

  virtual void release();

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getResolverNode(void);

protected:
  static const XMLCh g_nsBlocker[];
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XMLCh > _namespaceBindings;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *_resolverNode;
  XPath2MemoryManager* _memMgr;
};//XQillaNSResolverImpl


#endif //__XQILLANSRESOLVERIMPL_HPP
