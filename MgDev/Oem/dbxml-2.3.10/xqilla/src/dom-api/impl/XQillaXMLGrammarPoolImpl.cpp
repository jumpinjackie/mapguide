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
 * $Id: XQillaXMLGrammarPoolImpl.cpp,v 1.6 2006/11/01 16:37:19 jpcs Exp $
 */


#include "../../config/xqilla_config.h"
#include "XQillaXMLGrammarPoolImpl.hpp"
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/RuntimeException.hpp>

XQillaXMLGrammarPoolImpl::XQillaXMLGrammarPoolImpl(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const memMgr) : XMLGrammarPoolImpl(memMgr)
{};

bool XQillaXMLGrammarPoolImpl::cacheGrammar(XERCES_CPP_NAMESPACE_QUALIFIER Grammar* const  gramToCache)
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
