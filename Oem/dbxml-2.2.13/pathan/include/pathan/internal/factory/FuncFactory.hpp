/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Factory base class
*/

#ifndef _FUNCFACTORY_HPP
#define _FUNCFACTORY_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItem.hpp>

class PATHAN_EXPORT FuncFactory
{
public:
  FuncFactory();
  virtual ~FuncFactory();

  ///Create instance of a function, using FuncFactoryTemplate to determine type
  virtual DataItem *createInstance(const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const = 0;

  virtual const XMLCh *getName() const = 0;
  virtual const XMLCh *getURI() const = 0;
  virtual unsigned int getMinArgs() const = 0;
  virtual unsigned int getMaxArgs() const = 0;
};

#endif

