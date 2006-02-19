//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQScopedNamespace.hpp,v 1.5 2005/04/20 18:31:52 bostic Exp $
//

//////////////////////////////////////////////////////////////////////
// XQScopedNamespace.hpp: interface for the XQScopedNamespace class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_)
#define AFX_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMXPathNSResolver;
XERCES_CPP_NAMESPACE_END

class XQENGINE_API XQScopedNamespace : public PathanNSResolverImpl
{
public:
	XQScopedNamespace(XPath2MemoryManager* memMgr, const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* prevScope);
	virtual ~XQScopedNamespace();

    virtual const XMLCh* lookupNamespaceURI(const XMLCh* prefix) const;
    virtual const XMLCh* lookupPrefix(const XMLCh* uri) const;

protected:
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver* _prevScope;
};

#endif // !defined(AFX_XQSCOPEDNAMESPACE_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_)
