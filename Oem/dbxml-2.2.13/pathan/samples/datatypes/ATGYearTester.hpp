/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGYEARTESTER_HPP
#define _ATGYEARTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATGYearOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATGYearOrDerived
 */
class ATGYearTester: public DatatypeTester {
public: 
  ATGYearTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATGYearOrDerived::Ptr createGYear(const XMLCh* value,
                                    const DynamicContext* context);
};

#endif //_ATGYEARTESTER_HPP
