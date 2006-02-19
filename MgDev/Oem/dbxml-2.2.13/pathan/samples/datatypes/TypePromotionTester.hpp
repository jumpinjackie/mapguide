/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _TYPEPROMOTIONTESTER_HPP
#define _TYPEPROMOTIONTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;
class ATAnyURIOrDerived;

/**
 * Tests ATAnyURIOrDerived
 */
class TypePromotionTester: public DatatypeTester {
public: 
  TypePromotionTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();
};


#endif // _TYPEPROMOTIONTESTER_HPP
