/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGYEARMONTHTESTER_HPP
#define _ATGYEARMONTHTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATGYearMonthOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATGYearMonthOrDerived
 */
class ATGYearMonthTester: public DatatypeTester {
public: 
  ATGYearMonthTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATGYearMonthOrDerived::Ptr createGYearMonth(const XMLCh* value,
                                    const DynamicContext* context);
};

#endif //_ATGYEARMONTHTESTER_HPP
