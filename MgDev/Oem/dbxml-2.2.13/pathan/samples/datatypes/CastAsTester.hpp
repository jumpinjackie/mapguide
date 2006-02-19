/* * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _CASTASTESTER_HPP
#define _CASTASTESTER_HPP

#include <xercesc/util/XercesDefs.hpp>
#include "DatatypeTester.hpp"

class DynamicContext;
class XPath2MemoryManager;

#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATBase64BinaryOrDerived.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATDateOrDerived.hpp>
#include <pathan/ATDateTimeOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATFloatOrDerived.hpp>
#include <pathan/ATGDayOrDerived.hpp>
#include <pathan/ATGMonthOrDerived.hpp>
#include <pathan/ATGMonthDayOrDerived.hpp>
#include <pathan/ATGYearMonthOrDerived.hpp>
#include <pathan/ATGYearOrDerived.hpp>
#include <pathan/ATHexBinaryOrDerived.hpp>
#include <pathan/ATQNameOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATTimeOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>

/**
 * Tests CastAs for all datatypes
 */
class CastAsTester: public DatatypeTester {
public: 
  CastAsTester(XPath2MemoryManager* memMgr);
   
  // use default
  // virtual void init();

  virtual void run(const DynamicContext* context);
  
  // use default
  // virtual void wrapUp();

protected:
  const ATAnyURIOrDerived::Ptr createURI(const XMLCh* value, const DynamicContext* context);
  const ATBase64BinaryOrDerived::Ptr createBinary(const XMLCh* value, const DynamicContext* context);
  const ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value, const DynamicContext* context);
  const ATGDayOrDerived::Ptr createGDay(const XMLCh* value, const DynamicContext* context);
  const ATDurationOrDerived::Ptr createDuration(const XMLCh* value, const DynamicContext* context);
  const ATHexBinaryOrDerived::Ptr createHexBinary(const XMLCh* value, const DynamicContext* context);
  const ATGMonthDayOrDerived::Ptr createGMonthDay(const XMLCh* value, const DynamicContext* context);
  const ATGMonthOrDerived::Ptr createGMonth(const XMLCh* value, const DynamicContext* context);
  const ATQNameOrDerived::Ptr createQName(const XMLCh* value, const DynamicContext* context);
  const ATStringOrDerived::Ptr createString(const XMLCh* value, const DynamicContext* context);
  const ATGYearMonthOrDerived::Ptr createGYearMonth(const XMLCh* value, const DynamicContext* context);
  const ATGYearOrDerived::Ptr createGYear(const XMLCh* value, const DynamicContext* context);
};


#endif // _CASTASTESTER_HPP
