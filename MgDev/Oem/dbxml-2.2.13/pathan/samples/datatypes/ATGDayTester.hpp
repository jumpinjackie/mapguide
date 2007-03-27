/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATGDAYTESTER_HPP
#define _ATGDAYTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATGDayOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATGDayOrDerived
 */
class ATGDayTester: public DatatypeTester {
public: 
  ATGDayTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATGDayOrDerived::Ptr createGDay(const XMLCh* value,
                                    const DynamicContext* context);
};

#endif //_ATGDAYTESTER_HPP
