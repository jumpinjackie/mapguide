/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FLOOKUP_HPP
#define _FLOOKUP_HPP

#include <pathan/Pathan.hpp>

#include <vector>
#include <xercesc/util/XercesDefs.hpp>
#include <pathan/dataItem/DataItem.hpp>

class FuncFactory;
class XPath2MemoryManager;

class PATHAN_EXPORT FunctionLookup
{
public:
  /// do-nothing virtual dtor
  virtual ~FunctionLookup() {}

  ///adds a function to the custom function table
  virtual void insertFunction(FuncFactory *func) = 0;
  ///returns the approriate Function object
  virtual DataItem* lookUpFunction(const XMLCh* URI, const XMLCh* fname, const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const = 0;

  /// returns all the defined functions
  virtual std::vector< std::pair<const XMLCh*,const XMLCh*> > getFunctions() const = 0;
  virtual std::vector< FuncFactory* > getFunctionFactories() const = 0;
};

#endif 
