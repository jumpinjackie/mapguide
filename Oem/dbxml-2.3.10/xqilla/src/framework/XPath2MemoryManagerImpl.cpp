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
 * $Id: XPath2MemoryManagerImpl.cpp,v 1.7 2006/11/01 16:37:19 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

XPath2MemoryManagerImpl::XPath2MemoryManagerImpl()
{
  initialise();
}

XPath2MemoryManagerImpl::~XPath2MemoryManagerImpl() 
{
  releaseAll();
}

#ifdef WIN_USE_HEAP
void *XPath2MemoryManagerImpl::allocate(size_t amount)
{
  ++objectsAllocated_;
  totalMemoryAllocated_ += amount;
  return HeapAlloc(fHeap,HEAP_NO_SERIALIZE|HEAP_ZERO_MEMORY,amount);
}

void XPath2MemoryManagerImpl::deallocate(void* p)
{
  --objectsAllocated_;
  totalMemoryAllocated_ -= HeapSize(fHeap,HEAP_NO_SERIALIZE,p);
  HeapFree(fHeap,HEAP_NO_SERIALIZE,p);
}

void XPath2MemoryManagerImpl::initialise()
{
  fHeap=HeapCreate(HEAP_NO_SERIALIZE,128*1024,0);
  BaseMemoryManager::initialise();
}

void XPath2MemoryManagerImpl::releaseAll()
{
  BaseMemoryManager::releaseAll();
  HeapDestroy(fHeap);
}

#else

void *XPath2MemoryManagerImpl::internal_allocate(size_t size)
{
  return new char[size];
}

void XPath2MemoryManagerImpl::internal_deallocate(void *p)
{
  delete [] ((char *)p);
}

#endif

