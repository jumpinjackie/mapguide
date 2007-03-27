/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBOOLEANTESTER_HPP
#define _ATBOOLEANTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATBooleanOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATBooleanOrDerived
 */
class ATBooleanTester: public DatatypeTester {
public: 
  ATBooleanTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value,
                                          const DynamicContext* context);
  const ATBooleanOrDerived::Ptr createBoolean(bool value, const DynamicContext* context);
};


#endif //_ATBOOLEANTESTER_HPP
