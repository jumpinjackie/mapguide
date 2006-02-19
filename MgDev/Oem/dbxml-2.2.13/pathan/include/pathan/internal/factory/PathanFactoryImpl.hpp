/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Factory base class
*/

#ifndef _PATHANFACTORYIMPL_HPP
#define _PATHANFACTORYIMPL_HPP

#include <pathan/PathanFactory.hpp>
#include <pathan/functionAPI/DatatypeLookup.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>

#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/dom/DOMDocument.hpp>

class DatatypeLookup;
class DocumentCache;

class PATHAN_EXPORT PathanFactoryImpl : public PathanFactory
{
public:
  PathanFactoryImpl(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
  virtual ~PathanFactoryImpl();

  /* @name Node factory methods */

  /* @name Atomic type factory methods */

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName) const;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
                                                         const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  /* @name Number factory methods */

  virtual ATDoubleOrDerived::Ptr createDouble(const MAPM value, const DynamicContext* context);
  virtual ATDoubleOrDerived::Ptr createDouble(const XMLCh* value, const DynamicContext* context);
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATFloatOrDerived::Ptr createFloat(const MAPM value, const DynamicContext* context);
  virtual ATFloatOrDerived::Ptr createFloat(const XMLCh* value, const DynamicContext* context);
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATDecimalOrDerived::Ptr createInteger(const int value, const DynamicContext* context);
  virtual ATDecimalOrDerived::Ptr createInteger(const MAPM value, const DynamicContext* context);
  virtual ATDecimalOrDerived::Ptr createNonNegativeInteger(const MAPM value, const DynamicContext* context);

  virtual ATDecimalOrDerived::Ptr createDecimal(const MAPM value, const DynamicContext* context);
  virtual ATDecimalOrDerived::Ptr createDecimal(const XMLCh* value, const DynamicContext* context);
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context);
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  /* @name Date / time factory methods */

  virtual ATDateOrDerived::Ptr createDate(const XMLCh* value, const DynamicContext* context);
  virtual ATDateOrDerived::Ptr createDateOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATDateTimeOrDerived::Ptr createDateTime(const XMLCh* value, const DynamicContext* context);
  virtual ATDateTimeOrDerived::Ptr createDateTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATTimeOrDerived::Ptr createTime(const XMLCh* value, const DynamicContext* context);
  virtual ATTimeOrDerived::Ptr createTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATGDayOrDerived::Ptr createGDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
  virtual ATGMonthDayOrDerived::Ptr createGMonthDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
  virtual ATGMonthOrDerived::Ptr createGMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
  virtual ATGYearMonthOrDerived::Ptr createGYearMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);
  virtual ATGYearOrDerived::Ptr createGYearOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATDurationOrDerived::Ptr createDayTimeDuration(const XMLCh* value, const DynamicContext* context);
  virtual ATDurationOrDerived::Ptr createYearMonthDuration(const XMLCh* value, const DynamicContext* context);
  virtual ATDurationOrDerived::Ptr createDurationOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  /* @name Other factory methods */

  virtual ATStringOrDerived::Ptr createString(const XMLCh* value, const DynamicContext* context);
  virtual ATStringOrDerived::Ptr createStringOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATUntypedAtomic::Ptr createUntypedAtomic(const XMLCh* value, const DynamicContext* context);

  virtual ATBooleanOrDerived::Ptr createBoolean(bool value, const DynamicContext* context);
  virtual ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value, const DynamicContext* context);
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, bool value, const DynamicContext* context);
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context);

  virtual ATAnyURIOrDerived::Ptr createAnyURI(const XMLCh* value, const DynamicContext* context);

  virtual ATQNameOrDerived::Ptr createQName(const XMLCh* uri, const XMLCh *prefix, const XMLCh* name, const DynamicContext* context);
  virtual ATQNameOrDerived::Ptr createQNameOrDerived(const XMLCh* typeURI, const XMLCh *prefix, const XMLCh* typeName, const XMLCh* uri, const XMLCh* name, const DynamicContext* context);

private:
  DatatypeLookup datatypeLookup_;

};

#endif
