/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATTIMETESTER_HPP
#define _ATTIMETESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATTimeOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATTimeOrDerived
 */
class ATTimeTester: public DatatypeTester {
public: 
  ATTimeTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATTimeOrDerived::Ptr createTime(const XMLCh* value,
                                            const DynamicContext* context);
};

#endif //_ATTIMETESTER_HPP
