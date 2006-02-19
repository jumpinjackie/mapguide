/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include "ATNotationOrDerivedImpl.hpp"
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <pathan/internal/DOMutils/XStr.hpp> // defines X() and XMLCh*
#include <pathan/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLString.hpp>
#include <pathan/DynamicContext.hpp>

ATNotationOrDerivedImpl::
ATNotationOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATNotationOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  _notation = context->getMemoryManager()->getPooledString(value);
      
}

void *ATNotationOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATNotationOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATNotationOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgNotationString;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATNotationOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATNotationOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATNotationOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::NOTATION;
} 

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATNotationOrDerivedImpl::asString(const DynamicContext* context) const {
  return _notation;
}

/* returns true if the two objects are equal (string comparison)
   * false otherwise */
bool ATNotationOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATNotationOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }
  return XPath2Utils::equals(target->asString(context), _notation);  
}

AnyAtomicType::AtomicObjectType ATNotationOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
