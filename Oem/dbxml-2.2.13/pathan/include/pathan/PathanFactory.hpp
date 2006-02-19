/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Factory base class
*/

#ifndef _PATHANFACTORY_HPP
#define _PATHANFACTORY_HPP

#include <pathan/Pathan.hpp>
#include <pathan/internal/m_apm.h>
#include "ATAnyURIOrDerived.hpp"
#include "ATBooleanOrDerived.hpp"
#include "ATDateOrDerived.hpp"
#include "ATDateTimeOrDerived.hpp"
#include "ATDecimalOrDerived.hpp"
#include "ATDoubleOrDerived.hpp"
#include "ATDurationOrDerived.hpp"
#include "ATFloatOrDerived.hpp"
#include "ATGDayOrDerived.hpp"
#include "ATGMonthOrDerived.hpp"
#include "ATGMonthDayOrDerived.hpp"
#include "ATGYearMonthOrDerived.hpp"
#include "ATGYearOrDerived.hpp"
#include "ATQNameOrDerived.hpp"
#include "ATStringOrDerived.hpp"
#include "ATTimeOrDerived.hpp"
#include "ATUntypedAtomic.hpp"
#include "Node.hpp"

class PATHAN_EXPORT PathanFactory
{
public:
  virtual ~PathanFactory() {};

  /* @name Atomic type factory methods */

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName) const = 0;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
                                                         const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Number factory methods */

  virtual ATDoubleOrDerived::Ptr createDouble(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDouble(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATFloatOrDerived::Ptr createFloat(const MAPM value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloat(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDecimalOrDerived::Ptr createInteger(const int value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createInteger(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createNonNegativeInteger(const MAPM value, const DynamicContext* context) = 0;

  virtual ATDecimalOrDerived::Ptr createDecimal(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimal(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Date / time factory methods */

  virtual ATDateOrDerived::Ptr createDate(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDateOrDerived::Ptr createDateOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDateTimeOrDerived::Ptr createDateTime(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDateTimeOrDerived::Ptr createDateTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATTimeOrDerived::Ptr createTime(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATTimeOrDerived::Ptr createTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATGDayOrDerived::Ptr createGDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGMonthDayOrDerived::Ptr createGMonthDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGMonthOrDerived::Ptr createGMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGYearMonthOrDerived::Ptr createGYearMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGYearOrDerived::Ptr createGYearOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDurationOrDerived::Ptr createDayTimeDuration(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createYearMonthDuration(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createDurationOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Other factory methods */

  virtual ATStringOrDerived::Ptr createString(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATStringOrDerived::Ptr createStringOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATUntypedAtomic::Ptr createUntypedAtomic(const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATBooleanOrDerived::Ptr createBoolean(bool value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, bool value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATAnyURIOrDerived::Ptr createAnyURI(const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATQNameOrDerived::Ptr createQName(const XMLCh* uri, const XMLCh* prefix, const XMLCh* name, const DynamicContext* context) = 0;
  virtual ATQNameOrDerived::Ptr createQNameOrDerived(const XMLCh* typeURI, const XMLCh *prefix, const XMLCh* typeName, const XMLCh* uri, const XMLCh* name, const DynamicContext* context) = 0;

};

#endif
