/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATANYURITESTER_HPP
#define _ATANYURITESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"
#include <pathan/ATAnyURIOrDerived.hpp>

class DynamicContext;
class XPath2MemoryManager;

/**
 * Tests ATAnyURIOrDerived
 */
class ATAnyURITester: public DatatypeTester {
public: 
  ATAnyURITester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
private:
  ATAnyURIOrDerived::Ptr createURI(const XMLCh* value,
                                   const DynamicContext* context);
};


#endif //_ATANYURITESTER_HPP
