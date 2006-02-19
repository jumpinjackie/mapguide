/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDURATIONTESTER_HPP
#define _ATDURATIONTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATDurationOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATDurationOrDerived
 */
class ATDurationTester: public DatatypeTester {
public: 
  ATDurationTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATDurationOrDerived::Ptr createDuration(const XMLCh* value,
                                             const DynamicContext* context);
};
#endif //_ATDURATIONTESTER_HPP
