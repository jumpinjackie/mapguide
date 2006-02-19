/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATSTRINGTESTER_HPP
#define _ATSTRINGTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATStringOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATStringOrDerived
 */
class ATStringTester: public DatatypeTester {
public: 
  ATStringTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATStringOrDerived::Ptr createString(const XMLCh* value,
                                        const DynamicContext* context);
};

#endif //_ATSTRINGTESTER_HPP
