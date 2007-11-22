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
 * $Id: XQillaXMLGrammarPoolImpl.hpp,v 1.5 2006/11/01 16:37:19 jpcs Exp $
 */

#ifndef __XQILLAXMLGRAMMARPOOLIMPL_HPP
#define __XQILLAXMLGRAMMARPOOLIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>


XERCES_CPP_NAMESPACE_BEGIN
class Grammar;
class MemoryManager;
XERCES_CPP_NAMESPACE_END  

class XQillaXMLGrammarPoolImpl : public XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPoolImpl
{
public:

    XQillaXMLGrammarPoolImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr);

    /**
     * This method is overriden to not throw exceptions when a repeated grammar is found, but just ignore them
     **/
    virtual bool cacheGrammar(XERCES_CPP_NAMESPACE_QUALIFIER Grammar* const  gramToCache);
};

#endif
