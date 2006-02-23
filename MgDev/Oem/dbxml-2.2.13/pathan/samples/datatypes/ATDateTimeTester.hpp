/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATDATETIMETESTER_HPP
#define _ATDATETIMETESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATDateTimeOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATDateTimeOrDerived
 */
class ATDateTimeTester: public DatatypeTester {
public: 
  ATDateTimeTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  const ATDateTimeOrDerived::Ptr createDateTime(const XMLCh* value,
                                            const DynamicContext* context);
};

#endif //_ATDATETIMETESTER_HPP
