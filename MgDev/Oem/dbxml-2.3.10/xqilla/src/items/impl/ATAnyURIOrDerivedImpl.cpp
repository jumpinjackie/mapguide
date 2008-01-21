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
 * $Id: ATAnyURIOrDerivedImpl.cpp,v 1.15 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include "ATAnyURIOrDerivedImpl.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/ItemFactory.hpp>

ATAnyURIOrDerivedImpl::
ATAnyURIOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  _uri = context->getMemoryManager()->getPooledString(value);      
}

void *ATAnyURIOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gXQilla) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATAnyURIOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATAnyURIOrDerivedImpl::getPrimitiveName() {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_ANYURI;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATAnyURIOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATAnyURIOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATAnyURIOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::ANY_URI;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATAnyURIOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  switch (targetIndex) {
    case ANY_SIMPLE_TYPE:
    case UNTYPED_ATOMIC:
      //anySimpleType and untypedAtomic follow the same casting rules as string.
    case STRING: {
      return context->getItemFactory()->createDerivedFromAtomicType(targetIndex, targetURI, targetType, _uri, context);
    }
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATAnyURIOrDerivedImpl::asString(const DynamicContext* context) const {
  return _uri;
}

/* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
bool ATAnyURIOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex() &&
     target->getPrimitiveTypeIndex()!=STRING) 
  {
    XQThrow2(IllegalArgumentException,X("ATAnyURIOrDerivedImpl::equals"), X("Equality operator for given types not supported [err:XPTY0004]"));
  }
  return XPath2Utils::equals(target->asString(context), _uri);
}

AnyAtomicType::AtomicObjectType ATAnyURIOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
