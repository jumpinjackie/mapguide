/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDECIMALTESTER_HPP
#define _ATDECIMALTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;
class ATDecimalOrDerived;

/**
 * Tests ATDecimalOrDerived
 */
class ATDecimalTester: public DatatypeTester {
public: 
  ATDecimalTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
};


#endif //_ATDECIMALTESTER_HPP
