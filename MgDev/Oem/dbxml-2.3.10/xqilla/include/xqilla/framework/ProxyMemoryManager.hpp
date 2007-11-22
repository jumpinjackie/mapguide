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
 * $Id: ProxyMemoryManager.hpp,v 1.7 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef __PROXYMEMORYMANAGER_HPP
#define __PROXYMEMORYMANAGER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/BaseMemoryManager.hpp>

class XQILLA_API ProxyMemoryManager : public BaseMemoryManager
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

