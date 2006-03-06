/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATHEXBINARYTESTER_HPP
#define _ATHEXBINARYTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATHexBinaryOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATHexBinaryOrDerived
 */
class ATHexBinaryTester: public DatatypeTester {
public: 
  ATHexBinaryTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATHexBinaryOrDerived::Ptr createHexBinary(const XMLCh* value,
                                     const DynamicContext* context);
};

#endif //_ATHEXBINARYTESTER_HPP
