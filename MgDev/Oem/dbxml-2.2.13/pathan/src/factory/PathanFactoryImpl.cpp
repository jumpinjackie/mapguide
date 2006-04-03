/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>

#include <pathan/internal/factory/PathanFactoryImpl.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/functions/FunctionConstructor.hpp> // for datatype-uri
#include <pathan/internal/items/impl/ATBooleanOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATFloatOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATQNameOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/XPath2Utils.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/dom/DOM.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

PathanFactoryImpl::PathanFactoryImpl(const DocumentCache* dc, MemoryManager* memMgr)
  : datatypeLookup_(dc, memMgr)
{
}

PathanFactoryImpl::~PathanFactoryImpl()
{
}

AnyAtomicType::AtomicObjectType PathanFactoryImpl::getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName) const
{
  bool isPrimitive;
  const DatatypeFactory* dtf = datatypeLookup_.lookupDatatype(typeURI, typeName, isPrimitive);
  return dtf->getPrimitiveTypeIndex();
}

ATQNameOrDerived::Ptr PathanFactoryImpl::createQName(const XMLCh* uri,
	const XMLCh *prefix,
	const XMLCh* name, 
	const DynamicContext* context
	) {
  return createQNameOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_QNAME,
    uri,
    prefix,
    name,
    context
    );
}

ATDoubleOrDerived::Ptr PathanFactoryImpl::createDouble(const MAPM value, const DynamicContext* context) {
  return createDoubleOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_DOUBLE,
    value, context);
}

ATDoubleOrDerived::Ptr PathanFactoryImpl::createDouble(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDoubleFactory()->createInstance(value, context);
}

ATFloatOrDerived::Ptr PathanFactoryImpl::createFloat(const MAPM value, const DynamicContext* context) {
  return createFloatOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_FLOAT,
    value, context);
}

ATFloatOrDerived::Ptr PathanFactoryImpl::createFloat(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getFloatFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr PathanFactoryImpl::createDecimal(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_DECIMAL,
    value, context);
}

ATDecimalOrDerived::Ptr PathanFactoryImpl::createDecimal(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDecimalFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr PathanFactoryImpl::createInteger(const int value, const DynamicContext* context) {
  return context->getMemoryManager()->createInteger(value);
}

ATDecimalOrDerived::Ptr PathanFactoryImpl::createInteger(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_INTEGER,
    value, context);
}

ATBooleanOrDerived::Ptr PathanFactoryImpl::createBoolean(bool value, const DynamicContext* context) {
  return createBooleanOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_BOOLEAN,
    value, context);  
}

ATBooleanOrDerived::Ptr PathanFactoryImpl::createBoolean(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getBooleanFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr PathanFactoryImpl::createNonNegativeInteger(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_NONNEGATIVEINTEGER,
    value, context);
}
      
ATDurationOrDerived::Ptr PathanFactoryImpl::createDayTimeDuration(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDurationFactory()->
    createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
                   ATDurationOrDerived::fgDT_DAYTIMEDURATION, value, context);
}

ATDurationOrDerived::Ptr PathanFactoryImpl::createYearMonthDuration(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDurationFactory()->
    createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
                   ATDurationOrDerived::fgDT_YEARMONTHDURATION, value, context);
}

ATDateOrDerived::Ptr PathanFactoryImpl::createDate(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDateFactory()->createInstance(value, context);
}

ATDateTimeOrDerived::Ptr PathanFactoryImpl::createDateTime(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDateTimeFactory()->createInstance(value, context);
}

ATTimeOrDerived::Ptr PathanFactoryImpl::createTime(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getTimeFactory()->createInstance(value, context);
}

ATAnyURIOrDerived::Ptr PathanFactoryImpl::createAnyURI(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getAnyURIFactory()->createInstance(value, context);
}

ATStringOrDerived::Ptr PathanFactoryImpl::createString(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getStringFactory()->createInstance(value, context);
}

//////////////////////////
// All creation methods //
//////////////////////////


AnyAtomicType::Ptr PathanFactoryImpl::createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
                                                                  const XMLCh* typeName, const XMLCh* value, const DynamicContext* context)
{
  return datatypeLookup_.lookupDatatype(typeIndex)->createInstance(typeURI, typeName, value, context);
}

AnyAtomicType::Ptr PathanFactoryImpl::createDerivedFromAtomicType(const XMLCh* typeURI,
                                                                  const XMLCh* typeName, 
                                                                  const XMLCh* value, const DynamicContext* context) {
  bool isPrimitive;
  const DatatypeFactory* dtf = datatypeLookup_.lookupDatatype(typeURI, typeName, isPrimitive);
  if(isPrimitive) {
    return dtf->createInstance(value, context);
  }
  else {
    return dtf->createInstance(typeURI, typeName, value, context);
  }
}

/** create a xs:boolean */
ATBooleanOrDerived::Ptr PathanFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const XMLCh* value, 
                                                                  const DynamicContext* context) {
  return (const ATBooleanOrDerived::Ptr)datatypeLookup_.getBooleanFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:boolean with a bool value */
ATBooleanOrDerived::Ptr PathanFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  bool value, 
                                                                  const DynamicContext* context) {
  // No need to validate
  return new ATBooleanOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:date */
ATDateOrDerived::Ptr PathanFactoryImpl::createDateOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context){
  return (const ATDateOrDerived::Ptr)datatypeLookup_.getDateFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:dateTime */
ATDateTimeOrDerived::Ptr PathanFactoryImpl::createDateTimeOrDerived(const XMLCh* typeURI, 
                                                                    const XMLCh* typeName,
                                                                    const XMLCh* value,
                                                                    const DynamicContext* context){
  return (const ATDateTimeOrDerived::Ptr)datatypeLookup_.getDateTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal */
ATDecimalOrDerived::Ptr PathanFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const XMLCh* value,
                                                                  const DynamicContext* context){
  return (const ATDecimalOrDerived::Ptr)datatypeLookup_.getDecimalFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal with a MAPM */
ATDecimalOrDerived::Ptr PathanFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const MAPM value,
                                                                  const DynamicContext* context){
  // No need to validate
  return new ATDecimalOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:double */  
ATDoubleOrDerived::Ptr PathanFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context){
  if(XPath2Utils::equals(value, Numeric::NAN_string)) {
    value= Numeric::NaN_string;
  }
  return (const ATDoubleOrDerived::Ptr)datatypeLookup_.getDoubleFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:double with a MAPM */
ATDoubleOrDerived::Ptr PathanFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const MAPM value, 
                                                                const DynamicContext* context){
  // No need to validate
  return  new ATDoubleOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:duration */
ATDurationOrDerived::Ptr PathanFactoryImpl::createDurationOrDerived(const XMLCh* typeURI, 
                                                                    const XMLCh* typeName,
                                                                    const XMLCh* value, 
                                                                    const DynamicContext* context){
  return (const ATDurationOrDerived::Ptr)datatypeLookup_.getDurationFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:float */
ATFloatOrDerived::Ptr PathanFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const XMLCh* value, 
                                                              const DynamicContext* context){
  if(XMLString::equals(value, Numeric::NAN_string)) {
    value= Numeric::NaN_string;
  }
  return (const ATFloatOrDerived::Ptr)datatypeLookup_.getFloatFactory()->createInstance(typeURI, typeName, value, context);

}

/** create a xs:float with a MAPM */
ATFloatOrDerived::Ptr PathanFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const MAPM value, 
                                                              const DynamicContext* context) {
  // No need to validate
  return  new ATFloatOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:gDay */
ATGDayOrDerived::Ptr PathanFactoryImpl::createGDayOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context) {
  return (const ATGDayOrDerived::Ptr)datatypeLookup_.getGDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gMonth */
ATGMonthOrDerived::Ptr PathanFactoryImpl::createGMonthOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context) {
  return (const ATGMonthOrDerived::Ptr)datatypeLookup_.getGMonthFactory()->createInstance(typeURI, typeName, value, context);

}


/** create a xs:gMonthDay */
ATGMonthDayOrDerived::Ptr PathanFactoryImpl::createGMonthDayOrDerived(const XMLCh* typeURI, 
                                                                      const XMLCh* typeName,
                                                                      const XMLCh* value, 
                                                                      const DynamicContext* context) {
  return (const ATGMonthDayOrDerived::Ptr)datatypeLookup_.getGMonthDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gYear */
ATGYearOrDerived::Ptr PathanFactoryImpl::createGYearOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const XMLCh* value, 
                                                              const DynamicContext* context) {
  return (const ATGYearOrDerived::Ptr)datatypeLookup_.getGYearFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:gYearMonth */
ATGYearMonthOrDerived::Ptr PathanFactoryImpl::createGYearMonthOrDerived(const XMLCh* typeURI, 
                                                                        const XMLCh* typeName,
                                                                        const XMLCh* value, 
                                                                        const DynamicContext* context) {
  return (const ATGYearMonthOrDerived::Ptr)datatypeLookup_.getGYearMonthFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:QName with two parameters */
ATQNameOrDerived::Ptr PathanFactoryImpl::createQNameOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* uri,
	const XMLCh* prefix,
	const XMLCh* name, 
	const DynamicContext* context) {

  ATQNameOrDerivedImpl* tmp =  new ATQNameOrDerivedImpl(typeURI, typeName, uri, prefix, name, context);
  
  const DatatypeFactory* dtf_anyURI = datatypeLookup_.getAnyURIFactory();
  if(dtf_anyURI->checkInstance(uri, context)) {
    const DatatypeFactory* dtf_NCName = datatypeLookup_.getStringFactory();
    if (dtf_NCName->checkInstance(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_NCNAME, name, context)) {
      return tmp;
    } else {
      // this call will obviously fail, but it is better for error reporting, 
      // since we actually get the XMLException's error message 
      return (const ATQNameOrDerived::Ptr )dtf_NCName->
        createInstance(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_NCNAME, name, context);
    }
  } else {
    // this call will obviously fail, but it is better for error reporting, 
    // since we actually get the XMLException's error message 
    return (const ATQNameOrDerived::Ptr )dtf_anyURI->createInstance(uri, context);
  }
}


/** create a xs:string */
ATStringOrDerived::Ptr PathanFactoryImpl::createStringOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context) {
  return (const ATStringOrDerived::Ptr)datatypeLookup_.getStringFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:time */
ATTimeOrDerived::Ptr PathanFactoryImpl::createTimeOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context){
  return (const ATTimeOrDerived::Ptr)datatypeLookup_.getTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create an xdt:untypedAtomic */
ATUntypedAtomic::Ptr PathanFactoryImpl::createUntypedAtomic(const XMLCh* value, const DynamicContext* context) {
  return (const ATUntypedAtomic::Ptr)datatypeLookup_.getUntypedAtomicFactory()->createInstance(value, context);
}
