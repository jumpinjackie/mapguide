/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATBASE64BINARYTESTER_HPP
#define _ATBASE64BINARYTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATBase64BinaryOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATBase64BinaryOrDerived
 */
class ATBase64BinaryTester: public DatatypeTester {
public: 
  ATBase64BinaryTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATBase64BinaryOrDerived::Ptr createBinary(const XMLCh* value,
                                                  const DynamicContext* context);
};


#endif //_ATBASE64BINARYTESTER_HPP
