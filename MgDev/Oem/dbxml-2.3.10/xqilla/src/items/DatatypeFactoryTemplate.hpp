/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: DatatypeFactoryTemplate.hpp,v 1.8 2006/11/01 16:37:20 jpcs Exp $
 */

/*
  Factory template class for Datatypes
*/

#ifndef _DatatypeFactoryTemplate_HPP
#define _DatatypeFactoryTemplate_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include "impl/ATStringOrDerivedImpl.hpp"

class AnyAtomicType;

///used to facilitate the creation of datatypes
template<class TYPE>
class DatatypeFactoryTemplate : public DatatypeFactory
{
public:
  DatatypeFactoryTemplate(const DocumentCache* dc)
    : DatatypeFactory(dc, const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator*>
                      (dc->getDatatypeValidator(getPrimitiveTypeURI(), getPrimitiveTypeName()))) {}

  AnyAtomicType::Ptr createInstanceNoCheck(const XMLCh* typeURI,
                                           const XMLCh* typeName,
                                           const XMLCh* value,
                                           const DynamicContext* context) const {
    return new TYPE(typeURI, typeName, value, context);
  }

  AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const
  {
    return TYPE::getTypeIndex();
  }

  virtual const XMLCh* getPrimitiveTypeURI() const {
    return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA;
  }

  const XMLCh* getPrimitiveTypeName() const {
    return TYPE::getPrimitiveName();
  }

protected:
  DatatypeFactoryTemplate(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *val)
    : DatatypeFactory(dc, val) {}
};

template<class TYPE>
class StringDatatypeFactory : public DatatypeFactoryTemplate<TYPE>
{
public:
  StringDatatypeFactory(const DocumentCache* dc)
    : DatatypeFactoryTemplate<TYPE>(dc) {}

  virtual const XMLCh* getPrimitiveTypeURI() const {
    return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA;
  }

  const XMLCh* getPrimitiveTypeName() const {
    return TYPE::getPrimitiveName();
  }

  AnyAtomicType::Ptr createInstance(const XMLCh* value,
                                    const DynamicContext* context) const
  {
    return createInstanceNoCheck(getPrimitiveTypeURI(), getPrimitiveTypeName(), value, context);
  }

  bool checkInstance(const XMLCh* value,
                     const StaticContext* context) const
  {
    return true;
  }

protected:
  StringDatatypeFactory(const DocumentCache* dc, XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator *val)
    : DatatypeFactoryTemplate<TYPE>(dc, val) {}
};

template<class TYPE>
class UntypedAtomicDatatypeFactory : public StringDatatypeFactory<TYPE>
{
public:
  UntypedAtomicDatatypeFactory(const DocumentCache* dc)
    : StringDatatypeFactory<TYPE>(dc, const_cast<XERCES_CPP_NAMESPACE_QUALIFIER DatatypeValidator*>
                                  (dc->getDatatypeValidator(getPrimitiveTypeURI(), getPrimitiveTypeName()))) {}

  virtual const XMLCh* getPrimitiveTypeURI() const {
    return FunctionConstructor::XMLChXPath2DatatypesURI;
  }

  const XMLCh* getPrimitiveTypeName() const {
    return TYPE::getPrimitiveName();
  }

};

template<class TYPE>
class DayTimeDurationDatatypeFactory : public DatatypeFactoryTemplate<TYPE>
{
public:
  DayTimeDurationDatatypeFactory(const DocumentCache* dc)
    : DatatypeFactoryTemplate<TYPE>(dc) {}

  AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const
  {
    return AnyAtomicType::DAY_TIME_DURATION;
  }

  virtual const XMLCh* getPrimitiveTypeURI() const {
    return FunctionConstructor::XMLChXPath2DatatypesURI;
  }

  const XMLCh* getPrimitiveTypeName() const {
    return ATDurationOrDerived::fgDT_DAYTIMEDURATION;
  }
};

template<class TYPE>
class YearMonthDurationDatatypeFactory : public DatatypeFactoryTemplate<TYPE>
{
public:
  YearMonthDurationDatatypeFactory(const DocumentCache* dc)
    : DatatypeFactoryTemplate<TYPE>(dc) {}

  AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const
  {
    return AnyAtomicType::YEAR_MONTH_DURATION;
  }

  virtual const XMLCh* getPrimitiveTypeURI() const {
    return FunctionConstructor::XMLChXPath2DatatypesURI;
  }

  const XMLCh* getPrimitiveTypeName() const {
    return ATDurationOrDerived::fgDT_YEARMONTHDURATION;
  }
};

#endif // _DatatypeFactoryTemplate_HPP
