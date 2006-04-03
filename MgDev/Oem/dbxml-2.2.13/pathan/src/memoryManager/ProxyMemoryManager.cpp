/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/ProxyMemoryManager.hpp>

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

