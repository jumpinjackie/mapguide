//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQScopedNamespace.cpp,v 1.3 2005/04/05 16:44:58 bostic Exp $
//

//////////////////////////////////////////////////////////////////////
// XQScopedNamespace.cpp: implementation of the XQScopedNamespace class.
//////////////////////////////////////////////////////////////////////

#include "xquery/dom-extensions/XQScopedNamespace.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQScopedNamespace::XQScopedNamespace(XPath2MemoryManager* memMgr, const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* prevScope) :
    PathanNSResolverImpl(memMgr, NULL)
{
    _prevScope=prevScope;
}

XQScopedNamespace::~XQScopedNamespace()
{

}

const XMLCh* XQScopedNamespace::lookupNamespaceURI(const XMLCh* prefix) const
{
    const XMLCh* nsURI=PathanNSResolverImpl::lookupNamespaceURI(prefix);
    if(nsURI!=NULL)
        return nsURI;
    if(_prevScope)
        return _prevScope->lookupNamespaceURI(prefix);
    return NULL;
}

const XMLCh* XQScopedNamespace::lookupPrefix(const XMLCh* uri) const
{
    const XMLCh* nsPrefix=PathanNSResolverImpl::lookupPrefix(uri);
    if(nsPrefix!=NULL)
        return nsPrefix;
    if(_prevScope)
        return _prevScope->lookupPrefix(uri);
    return NULL;
}
