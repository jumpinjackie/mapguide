/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDOUBLETESTER_HPP
#define _ATDOUBLETESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;
class ATDoubleOrDerived;

/**
 * Tests ATDoubleOrDerived
 */
class ATDoubleTester: public DatatypeTester {
public: 
  ATDoubleTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
};

#endif //_ATDOUBLETESTER_HPP
