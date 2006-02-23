/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGMONTHDAYTESTER_HPP
#define _ATGMONTHDAYTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATGMonthDayOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATGMonthDayOrDerived
 */
class ATGMonthDayTester: public DatatypeTester {
public: 
  ATGMonthDayTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATGMonthDayOrDerived::Ptr createGMonthDay(const XMLCh* value,
                                    const DynamicContext* context);
};

#endif //_ATGMONTHDAYTESTER_HPP
