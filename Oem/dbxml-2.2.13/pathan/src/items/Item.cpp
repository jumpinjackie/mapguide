/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/Item.hpp>
#include <pathan/Pathan.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

const XMLCh Item::gPathan[] =   // Points to "Pathan"
{
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_P,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
  XERCES_CPP_NAMESPACE_QUALIFIER chNull
};
