/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGMONTHTESTER_HPP
#define _ATGMONTHTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATGMonthOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATGMonthOrDerived
 */
class ATGMonthTester: public DatatypeTester {
public: 
  ATGMonthTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATGMonthOrDerived::Ptr createGMonth(const XMLCh* value,
                                    const DynamicContext* context);
};

#endif //_ATGMONTHTESTER_HPP
