/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANNSRESOLVERIMPL_HPP
#define __PATHANNSRESOLVERIMPL_HPP

#include <pathan/Pathan.hpp>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/PathanNSResolver.hpp>
#include <xercesc/util/RefHashTableOf.hpp>

class XPath2MemoryManager;

class PATHAN_EXPORT PathanNSResolverImpl : public PathanNSResolver
{
public:
	PathanNSResolverImpl(XPath2MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *resolverNode);
	~PathanNSResolverImpl();

	virtual const XMLCh* lookupNamespaceURI(const XMLCh* prefix) const;
  virtual const XMLCh* lookupPrefix(const XMLCh* uri) const;
  
  virtual void addNamespaceBinding(const XMLCh* prefix, const XMLCh* uri);

  virtual void release();

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getResolverNode(void);

protected:
  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf< XMLCh > _namespaceBindings;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *_resolverNode;
  XPath2MemoryManager* _memMgr;
};//PathanNSResolverImpl


#endif //__PATHANNSRESOLVERIMPL_HPP
