/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATFLOATTESTER_HPP
#define _ATFLOATTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;
class ATFloatOrDerived;

/**
 * Tests ATFloatOrDerived
 */
class ATFloatTester: public DatatypeTester {
public: 
  ATFloatTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
};

#endif //_ATFLOATTESTER_HPP
