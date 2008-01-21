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
 * $Id: Item.hpp,v 1.7 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ITEM_HPP
#define _ITEM_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xercesc/util/XercesDefs.hpp>

// forward declare when we can
class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API Item : public ReferenceCounted
{
public:
  typedef RefCountPointer<const Item> Ptr;

  /** The "XQilla" item interface */
  static const XMLCh gXQilla[];

  virtual bool isNode() const = 0;

  virtual bool isAtomicValue() const = 0;

  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  virtual const XMLCh* getTypeURI() const = 0;

  virtual const XMLCh* getTypeName() const = 0;

  /** Method for returning arbitrary interfaces from the implementations */
  virtual void *getInterface(const XMLCh *name) const = 0;

protected:
  Item() {}

private:
  // copy constructor
  Item (const Item & other);            
  // assignment operation
  Item & operator=(const Item & item) ;
};

#endif // _ITEM_HPP

