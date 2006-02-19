/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __BASEMEMORYMANAGER_HPP
#define __BASEMEMORYMANAGER_HPP

#include <pathan/Pathan.hpp>
#include <pathan/XPath2MemoryManager.hpp>

#include <xercesc/util/RefHashTableOf.hpp>

// Define ALLOCATE_IN_CHUNKS to 1 to allocate
// CHUNK_SIZE blocks of memory at a time, and
// carve the requested memory from them. The
// DB XML benchmark suggests this behaves badly
// with larger data sets. - jpcs
#define ALLOCATE_IN_CHUNKS 0

XERCES_CPP_NAMESPACE_BEGIN
class DOMNode;
XERCES_CPP_NAMESPACE_END

class VariableStore;
class FunctionLookup;
class Collation;
class CollationHelper;
class PathanNSResolver;
class StringPool;

class PATHAN_EXPORT BaseMemoryManager : public XPath2MemoryManager
{
public:
  virtual ~BaseMemoryManager();

  // from MemoryManager
  virtual void* allocate(size_t numElements);
  virtual void deallocate(void* p);
  
  /** Returns a copy of the given string */
  virtual const XMLCh* getPooledString(const XMLCh *src);
  /** Returns a copy of the transcoding of the given string */
  virtual const XMLCh* getPooledString(const char *src);

  /** Use with extreme caution! */
  virtual void reset();

  virtual void dumpStatistics() const;
  virtual unsigned long getAllocatedObjectCount() const { return objectsAllocated_; }
  virtual unsigned long getTotalAllocatedMemory() const { return totalMemoryAllocated_; }
  virtual const StringPool *getStringPool() const {
	  return fStringPool;
  }
  
  /** create a collation */
  virtual Collation* createCollation(CollationHelper* helper);

  /** create a resolver */
  virtual PathanNSResolver* createNSResolver(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *resolverNode);
  
  /** create a store for variables */
  virtual VariableStore* createVariableStore();

  /** create a store for variables */
  virtual VariableTypeStore* createVariableTypeStore();

  /** create a function table */
  virtual FunctionLookup* createFunctionTable();

  /** create a ATDecimalOrDerived for the given integer */
  virtual ATDecimalOrDerived* createInteger(int value);

protected:
  virtual void initialise();
  virtual void releaseAll();

  virtual void *internal_allocate(size_t size) = 0;
  virtual void internal_deallocate(void *p) = 0;

  struct PATHAN_EXPORT MemList {
    MemList *prev;
    MemList *next;
    size_t size;
#if ALLOCATE_IN_CHUNKS
    size_t remaining;
    unsigned int allocCount;
#endif
  };

#if ALLOCATE_IN_CHUNKS
  struct PATHAN_EXPORT MemAlloc {
    MemList *list;
  };
#endif

  MemList *fCurrentBlock;

  unsigned long objectsAllocated_;
  unsigned long totalMemoryAllocated_;

  StringPool *fStringPool;

  XERCES_CPP_NAMESPACE_QUALIFIER RefHashTableOf<ATDecimalOrDerived>* fIntegerPool;
};

#endif //__BASEMEMORYMANAGER_HPP

