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
 * $Id: ItemConstructor.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ITEMCONSTUCTOR_HPP
#define _ITEMCONSTUCTOR_HPP

#include <vector>
#include <string>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

class DynamicContext;
class StaticType;

class XQILLA_API ItemConstructor {
public:
  typedef std::vector<ItemConstructor*,XQillaAllocator<ItemConstructor*> > Vector;

  virtual ~ItemConstructor() {}

  virtual const StaticType &getStaticType() const = 0;

  virtual Item::Ptr createItem(const DynamicContext* context) const = 0;
  virtual std::string asString(const DynamicContext* context) const = 0;

  virtual const XMLCh* getTypeURI() const = 0;
  virtual const XMLCh* getTypeName() const = 0;
};

#endif

