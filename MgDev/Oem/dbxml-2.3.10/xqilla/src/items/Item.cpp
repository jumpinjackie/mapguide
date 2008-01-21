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
 * $Id: Item.cpp,v 1.8 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/items/Item.hpp>
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

const XMLCh Item::gXQilla[] =   // Points to "XQilla"
{
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_X,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Q,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
  XERCES_CPP_NAMESPACE_QUALIFIER chNull
};
