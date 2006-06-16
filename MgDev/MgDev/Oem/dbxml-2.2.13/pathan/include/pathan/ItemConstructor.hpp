/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ITEMCONSTUCTOR_HPP
#define _ITEMCONSTUCTOR_HPP

#include <vector>
#include <string>

#include "Pathan.hpp"
#include "Item.hpp"
#include <pathan/XPath2MemoryManager.hpp>

class DynamicContext;

class PATHAN_EXPORT ItemConstructor {
public:
  /** do-nothing virtual dtor */	
  virtual ~ItemConstructor() {}
	
  typedef std::vector<ItemConstructor*,PathanAllocator<ItemConstructor*> > Vector;

  virtual bool isNumeric() const = 0;

  virtual Item::Ptr createItem(const DynamicContext* context) const = 0;
  virtual std::string asString(const DynamicContext* context) const = 0;
};

#endif

