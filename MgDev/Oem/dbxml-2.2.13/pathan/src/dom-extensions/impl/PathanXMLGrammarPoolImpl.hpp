/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANXMLGRAMMARPOOLIMPL_HPP
#define __PATHANXMLGRAMMARPOOLIMPL_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>


XERCES_CPP_NAMESPACE_BEGIN
class Grammar;
class MemoryManager;
XERCES_CPP_NAMESPACE_END  

class PathanXMLGrammarPoolImpl : public XERCES_CPP_NAMESPACE_QUALIFIER XMLGrammarPoolImpl
{
public:

    PathanXMLGrammarPoolImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr);

    /**
     * This method is overriden to not throw exceptions when a repeated grammar is found, but just ignore them
     **/
    virtual bool cacheGrammar(XERCES_CPP_NAMESPACE_QUALIFIER Grammar* const  gramToCache);
};

#endif
