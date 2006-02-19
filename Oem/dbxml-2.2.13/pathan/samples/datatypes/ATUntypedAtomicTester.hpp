/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATUNTYPEDATOMICTESTER_HPP
#define _ATUNTYPEDATOMICTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;
class ATUntypedAtomic;

/**
 * Tests ATUntypedAtomic
 */
class ATUntypedAtomicTester: public DatatypeTester {
public: 
  ATUntypedAtomicTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
};
#endif //_ATUNTYPEDATOMICTESTER_HPP
