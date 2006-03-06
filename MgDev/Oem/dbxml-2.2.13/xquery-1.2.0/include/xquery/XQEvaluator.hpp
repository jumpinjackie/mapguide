//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQEvaluator.hpp,v 1.7 2005/04/05 16:44:44 bostic Exp $
//

//////////////////////////////////////////////////////////////////////
// XQEvaluator.h: interface for the XQEvaluator class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQEVALUATOR_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
#define AFX_XQEVALUATOR_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_

#include "XQEngine.hpp"
#include "XQQuery.hpp"

class XQContext;

XERCES_CPP_NAMESPACE_BEGIN
    class InputSource;
    class XMLBuffer;
XERCES_CPP_NAMESPACE_END

class XQENGINE_API XQEvaluator
{
public:
    static XQQuery* parse(const XMLCh* query, XQContext* context, bool staticallyResolve = true, const XMLCh* queryFile=NULL);
    static XQQuery* parse(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource& querySrc, XQContext* context, bool staticallyResolve = true);
    static XQQuery* parseFromURI(const XMLCh* queryFile, XQContext* context, bool staticallyResolve = true);
    static bool readQuery(const XMLCh* queryFile, 
                          XPath2MemoryManager* memMgr, 
                          XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& queryText);
    static bool readQuery(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource& querySrc, 
                          XPath2MemoryManager* memMgr, 
                          XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& queryText);
};

#endif // !defined(AFX_XQEVALUATOR_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
