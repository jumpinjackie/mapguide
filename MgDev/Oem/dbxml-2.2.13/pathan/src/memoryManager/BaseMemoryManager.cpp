/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/BaseMemoryManager.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/util/HashPtr.hpp>
#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

#include <pathan/internal/StringPool.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>
#include <pathan/internal/collations/CollationImpl.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include "../simpleVariables/VarStoreImpl.hpp"
#include "../simpleVariables/VarTypeStoreImpl.hpp"
#include "../functionAPI/FunctionLookupImpl.hpp"
#include <pathan/internal/items/impl/ATDecimalOrDerivedImpl.hpp>

#include <iostream>

static const unsigned int CHUNK_SIZE = 32 * 1024;

BaseMemoryManager::~BaseMemoryManager() 
{
}

void BaseMemoryManager::reset()
{
  releaseAll();
  initialise();
}

void BaseMemoryManager::initialise()
{
  fCurrentBlock = 0;
  objectsAllocated_ = 0;
  totalMemoryAllocated_ = 0;
  fStringPool = new (this) StringPool(this);
  fIntegerPool = 0;

}

void BaseMemoryManager::releaseAll()
{
  if (fIntegerPool)
    fIntegerPool->cleanup();
  // Run backwards through the linked list, deleting the blocks of memory
  while(fCurrentBlock) {
    MemList *prev = fCurrentBlock->prev;
    internal_deallocate((char *)fCurrentBlock);
    fCurrentBlock = prev;
  }
}

void *BaseMemoryManager::allocate(size_t amount)
{
#if ALLOCATE_IN_CHUNKS
  size_t memSize = amount + sizeof(MemAlloc);

  if(fCurrentBlock == 0 || fCurrentBlock->remaining < memSize) {
    size_t chunkSize = sizeof(MemList);
	if(memSize > CHUNK_SIZE) {
      chunkSize += memSize;
    }
	else {
      chunkSize += CHUNK_SIZE;
	}

    char *newBlock = 0;
    try {
      newBlock = (char*)internal_allocate(chunkSize);
    }
    catch (...) {}
    if(!newBlock) {
      throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::allocate(): Out of memory"));
    }

    ++objectsAllocated_;
    totalMemoryAllocated_ += chunkSize;

    MemList *newMemList = (MemList*)newBlock;
    newMemList->prev = fCurrentBlock;
    newMemList->next = 0;
    newMemList->size = chunkSize;
    newMemList->remaining = chunkSize - sizeof(MemList);
    newMemList->allocCount = 0;

    if(fCurrentBlock) {
      fCurrentBlock->next = newMemList;
    }
    fCurrentBlock = newMemList;
  }

  fCurrentBlock->remaining -= memSize;
  ++fCurrentBlock->allocCount;

  // Carve memory from the end of the current chunk
  char *mem = ((char*)fCurrentBlock) + sizeof(MemList)
    + fCurrentBlock->remaining;

  MemAlloc *alloc = (MemAlloc*)mem;
  alloc->list = fCurrentBlock;

  return (void*)(mem + sizeof(MemAlloc));
#else
  char *newBlock = 0;
  try {
    newBlock = (char*)internal_allocate(amount + sizeof(MemList));
  }
  catch (...) {}
  if(!newBlock) {
    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::allocate(): Out of memory"));
  }

  ++objectsAllocated_;
  totalMemoryAllocated_ += amount;

  MemList *newMemList = (MemList *)newBlock;
  newMemList->prev = fCurrentBlock;
  newMemList->next = 0;
  newMemList->size = amount;
#if DEBUG_MEMORY
  newMemList->magic = DEBUG_MEMORY_ALLOCD;
#endif

  if(fCurrentBlock) {
#if DEBUG_MEMORY
	  if (fCurrentBlock->magic != DEBUG_MEMORY_ALLOCD)
		  throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::allocate(): Corrupt block list"));		  
#endif
    fCurrentBlock->next = newMemList;
  }
  fCurrentBlock = newMemList;

  return (void *)(newBlock + sizeof(MemList));
#endif
}

void BaseMemoryManager::deallocate(void* p)
{
  if(p) {
#if ALLOCATE_IN_CHUNKS
    MemAlloc *alloc = (MemAlloc*)(((char*)p) - sizeof(MemAlloc));
    MemList *oldMemList = alloc->list;

    --oldMemList->allocCount;
    if(oldMemList->allocCount == 0) {
      // Decouple the old block from the list
      if(oldMemList->prev) {
        oldMemList->prev->next = oldMemList->next;
      }
      if(oldMemList->next) {
        oldMemList->next->prev = oldMemList->prev;
      }
      else {
        // If the old block doesn't have a next, it must currently be fCurrentBlock
        fCurrentBlock = oldMemList->prev;
      }

      --objectsAllocated_;
      totalMemoryAllocated_ -= oldMemList->size;

      internal_deallocate((void*)oldMemList);
    }
#else
    char *oldBlock = ((char *)p) - sizeof(MemList);
    MemList *oldMemList = (MemList *)oldBlock;
#if DEBUG_MEMORY
    if (oldMemList->magic != DEBUG_MEMORY_ALLOCD) {
	    if (oldMemList->magic == DEBUG_MEMORY_FREED)
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): Trying to free free'd memory"));
	    else
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): Trying to free unalloc'd memory"));
    } else
	    oldMemList->magic = DEBUG_MEMORY_FREED;
#endif

    // Decouple the old block from the list
    if(oldMemList->prev) {
#if DEBUG_MEMORY
    if (oldMemList->prev->magic != DEBUG_MEMORY_ALLOCD) {
	    if (oldMemList->prev->magic == DEBUG_MEMORY_FREED)
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): Free'd memory found on list"));
	    else
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): stray memory found on list"));
    }
#endif	    
      oldMemList->prev->next = oldMemList->next;
    }
    if(oldMemList->next) {
#if DEBUG_MEMORY
    if (oldMemList->next->magic != DEBUG_MEMORY_ALLOCD) {
	    if (oldMemList->next->magic == DEBUG_MEMORY_FREED)
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): Free'd memory found on list (next)"));
	    else
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): stray memory found on list (next)"));
    }
#endif	    
      oldMemList->next->prev = oldMemList->prev;
    }
    else {
      // If the old block doesn't have a next, it must currently be fCurrentBlock
      fCurrentBlock = oldMemList->prev;
    }
#if DEBUG_MEMORY
    if (fCurrentBlock && fCurrentBlock->magic != DEBUG_MEMORY_ALLOCD) {
	    if (fCurrentBlock->magic == DEBUG_MEMORY_FREED)
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): Free'd memory found on list (fCurrent)"));
	    else
		    throw PathanException(PathanException::RUNTIME_ERR, X("BaseMemoryManager::deallocate(): stray memory found on list (fCurrent)"));
    }
#endif
    --objectsAllocated_;
    totalMemoryAllocated_ -= oldMemList->size;

    internal_deallocate(oldBlock);
#endif
  }
}

const XMLCh *BaseMemoryManager::getPooledString(const XMLCh *src)
{
  return fStringPool->getPooledString(src);
}

const XMLCh *BaseMemoryManager::getPooledString(const char *src)
{
  return fStringPool->getPooledString(src);
}

void BaseMemoryManager::dumpStatistics() const
{
  std::cout << "Memory statistics:" << std::endl;

  fStringPool->dumpStatistics();

  std::cout << "\tAllocated objects: " << objectsAllocated_ << std::endl;
  std::cout << "\tTotal Memory Allocated: " << totalMemoryAllocated_ << std::endl;
}

/** create a collation */
Collation* BaseMemoryManager::createCollation(CollationHelper* helper) {
  return new (this) CollationImpl(this,helper);
}

/** create a resolver */
PathanNSResolver* BaseMemoryManager::createNSResolver(DOMNode *resolverNode) {
  return new (this) PathanNSResolverImpl(this, resolverNode);
}

/** create a store for variables */
VariableStore* BaseMemoryManager::createVariableStore() {
  return new (this) VarStoreImpl(this);
} 

/** create a store for variables */
VariableTypeStore* BaseMemoryManager::createVariableTypeStore() {
  return new (this) VarTypeStoreImpl(this);
} 

/** create a function table */
FunctionLookup* BaseMemoryManager::createFunctionTable() {
  return new (this) FunctionLookupImpl(this);
}

/** create a ATDecimalOrDerived for the given integer */
ATDecimalOrDerived* BaseMemoryManager::createInteger(int value) {
  if (!fIntegerPool)
    fIntegerPool = new (this) RefHashTableOf<ATDecimalOrDerived>(53,true, new (this) HashPtr(),this);

  if(fIntegerPool->containsKey((const void*)value))
      return fIntegerPool->get((const void*)value);
  ATDecimalOrDerivedImpl* itemValue=new ATDecimalOrDerivedImpl(value);
  itemValue->incrementRefCount();
  fIntegerPool->put((void*)value, itemValue);
  return itemValue;
}
