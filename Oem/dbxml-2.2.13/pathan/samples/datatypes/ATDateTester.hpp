/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDATETESTER_HPP
#define _ATDATETESTER_HPP

#include <xercesc/util/XMLString.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATDateOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATDateOrDerived
 */
class ATDateTester: public DatatypeTester {
public: 
  ATDateTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATDateOrDerived::Ptr createDate(const XMLCh* value,
                                     const DynamicContext* context);
};


#endif //_ATDATETESTER_HPP
