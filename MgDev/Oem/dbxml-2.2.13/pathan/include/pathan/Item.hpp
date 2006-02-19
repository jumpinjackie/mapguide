/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ITEM_HPP
#define _ITEM_HPP

#include "Pathan.hpp"
#include "ReferenceCounted.hpp"
#include <xercesc/util/XercesDefs.hpp>

// forward declare when we can
class XPath2MemoryManager;
class DynamicContext;

class PATHAN_EXPORT Item : public ReferenceCounted
{
public:
  typedef RefCountPointer<const Item> Ptr;

  /** The "Pathan" item interface */
  static const XMLCh gPathan[];

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

