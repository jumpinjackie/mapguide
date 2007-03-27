/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */


#include "../../config/pathan_config.h"
#include "PathanXMLGrammarPoolImpl.hpp"
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/RuntimeException.hpp>

PathanXMLGrammarPoolImpl::PathanXMLGrammarPoolImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr) : XMLGrammarPoolImpl(memMgr)
{};

bool PathanXMLGrammarPoolImpl::cacheGrammar(XERCES_CPP_NAMESPACE_QUALIFIER Grammar* const  gramToCache)
{
  try {
    return XMLGrammarPoolImpl::cacheGrammar(gramToCache);
  }
  catch(XERCES_CPP_NAMESPACE_QUALIFIER RuntimeException& e) {
    //do nothing - return false because the grammar was not accepted
    return false;
  }
  return false;
}
