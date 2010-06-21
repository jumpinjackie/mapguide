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
 * $Id: ProxyMemoryManager.cpp,v 1.7 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/ProxyMemoryManager.hpp>

ProxyMemoryManager::ProxyMemoryManager(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *parent)
  : parent_(parent)
{
  initialise();
}

ProxyMemoryManager::~ProxyMemoryManager() 
{
  releaseAll();
}

void *ProxyMemoryManager::internal_allocate(size_t size)
{
  return parent_->allocate(size);
}

void ProxyMemoryManager::internal_deallocate(void *p)
{
  parent_->deallocate(p);
}

