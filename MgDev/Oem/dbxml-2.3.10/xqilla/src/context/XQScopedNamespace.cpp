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
 * $Id: XQScopedNamespace.cpp,v 1.6 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQScopedNamespace.cpp: implementation of the XQScopedNamespace class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/context/XQScopedNamespace.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQScopedNamespace::XQScopedNamespace(XPath2MemoryManager* memMgr, const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* prevScope) :
    XQillaNSResolverImpl(memMgr, NULL)
{
    _prevScope=prevScope;
    _ctxNode=NULL;
}

XQScopedNamespace::~XQScopedNamespace()
{

}

void XQScopedNamespace::setNodeContext(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* node)
{
    _ctxNode=node;
}

const XMLCh* XQScopedNamespace::lookupNamespaceURI(const XMLCh* prefix) const
{
    const XMLCh* resolvedURI = _namespaceBindings.get((void*)prefix);
    if(resolvedURI != NULL) {
      if(XPath2Utils::equals(resolvedURI, g_nsBlocker))
        return NULL;
      else
        return resolvedURI;
    }
    if(_ctxNode)
    {
        resolvedURI=_ctxNode->lookupNamespaceURI(prefix);
        if(resolvedURI)
            return resolvedURI;
    }
    if(_prevScope)
        return _prevScope->lookupNamespaceURI(prefix);
    return NULL;
}

const XMLCh* XQScopedNamespace::lookupPrefix(const XMLCh* uri) const
{
    const XMLCh* nsPrefix=XQillaNSResolverImpl::lookupPrefix(uri);
    if(nsPrefix!=NULL)
        return nsPrefix;
    if(_ctxNode)
    {
        nsPrefix=_ctxNode->lookupNamespacePrefix(uri, false);
        if(nsPrefix)
            return nsPrefix;
    }
    if(_prevScope)
        return _prevScope->lookupPrefix(uri);
    return NULL;
}
