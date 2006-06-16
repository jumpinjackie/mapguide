/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PROXYMEMORYMANAGER_HPP
#define __PROXYMEMORYMANAGER_HPP

#include <pathan/Pathan.hpp>
#include "BaseMemoryManager.hpp"

class PATHAN_EXPORT ProxyMemoryManager : public BaseMemoryManager
{
public:
  ProxyMemoryManager(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *parent);
  virtual ~ProxyMemoryManager();

protected:
  virtual void *internal_allocate(size_t size);
  virtual void internal_deallocate(void *p);

private:
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *parent_;
};

#endif //__PROXYMEMORYMANAGER_HPP

