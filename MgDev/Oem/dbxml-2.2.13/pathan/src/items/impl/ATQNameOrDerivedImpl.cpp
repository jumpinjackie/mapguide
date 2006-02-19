/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../../config/pathan_config.h"
#include <pathan/internal/items/impl/ATQNameOrDerivedImpl.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/internal/DOMutils/XStr.hpp> // defines X() and XMLCh*
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanFactory.hpp>

#include <xercesc/util/XMLString.hpp>

ATQNameOrDerivedImpl::
ATQNameOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATQNameOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
  
  const XMLCh* prefix = XPath2NSUtils::getPrefix(value, context->getMemoryManager());
  const XMLCh* uri;
  if(XPath2Utils::equals(prefix, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString)) {
  // If $qname has no prefix
    uri = context->getDefaultElementAndTypeNS();
  }
  // If $qname has a prefix
  else {
    uri = context->getUriBoundToPrefix(prefix);
    if (uri == 0) {
       DSLthrow(IllegalArgumentException, X("ATQNameOrDerivedImpl::ATQNameOrDerivedImpl"),X("No namespace for prefix"));
    }
  }

  // _uri will be null if there is no default NS
  _uri = context->getMemoryManager()->getPooledString(uri);
  _prefix = context->getMemoryManager()->getPooledString(prefix);
  _name = context->getMemoryManager()->getPooledString(XPath2NSUtils::getLocalName(value));

}

ATQNameOrDerivedImpl::
ATQNameOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* uri, const XMLCh* prefix, const XMLCh* name, const StaticContext* context): 
    ATQNameOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
  
  // if uri is NULL, so will _uri
  _uri = context->getMemoryManager()->getPooledString(uri);
  _prefix = context->getMemoryManager()->getPooledString(prefix);
  _name = context->getMemoryManager()->getPooledString(name);

}


void *ATQNameOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gPathan) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATQNameOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATQNameOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATQNameOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

/* returns the URI */
const XMLCh* ATQNameOrDerivedImpl::getURI(void) const {
  return _uri;
}

/* returns the prefix */
const XMLCh* ATQNameOrDerivedImpl::getPrefix(void) const {
  return _prefix;
}

/* returns the name */
const XMLCh* ATQNameOrDerivedImpl::getName(void) const {
  return _name;
}


AnyAtomicType::AtomicObjectType ATQNameOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::QNAME;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATQNameOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  switch (targetIndex) {
    case ANY_SIMPLE_TYPE:
    case UNTYPED_ATOMIC:
      //anySimpleType and untypedAtomic follow the same casting rules as string.
    case STRING: {
      return context->getPathanFactory()->createDerivedFromAtomicType(targetURI, targetType, this->asLexicalString(context), context);
    } 
    case QNAME: {
      return context->getPathanFactory()->createQNameOrDerived(targetURI, targetType, _uri, _prefix, _name, context);
    }
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATQNameOrDerivedImpl::asString(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  buffer.set(_uri);
  buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
  buffer.append(_name);

  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

/* returns the XMLCh* (lexical => prefix:localname) representation of this type */
const XMLCh* ATQNameOrDerivedImpl::asLexicalString(const DynamicContext* context) const {
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  const XMLCh* prefix;
  if(this->_uri == 0 || XPath2Utils::equals(_uri, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString)) {
    if(context->getDefaultElementAndTypeNS() != 0) {
      DSLthrow(NamespaceLookupException,X("ATQNameOrDerivedImpl::asLexicalString"), X("Default namespace is defined: casting a QName that has no namespace is ambiguous"));
    }
    buffer.set(_name);
  } 
  else {
    // if we have a prefix, use it, otherwise try looking at the in-scope namespaces
    if(XPath2Utils::equals(_prefix,XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString))
        prefix = context->getPrefixBoundToUri(this->_uri);
    else
        prefix = _prefix;
    if (prefix == 0) {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer msg(1023, context->getMemoryManager());
      msg.set(X("No prefix defined for namespace '"));
      msg.append(_uri);
      msg.append(X("'"));
      DSLthrow(NamespaceLookupException,X("ATQNameOrDerivedImpl::asLexicalString"), msg.getRawBuffer());
    }
    buffer.set(prefix);
    buffer.append(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
    buffer.append(_name);    
  }
  return context->getMemoryManager()->getPooledString(buffer.getRawBuffer());
}

/* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
bool ATQNameOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    DSLthrow(IllegalArgumentException,X("ATQNameOrDerivedImpl::equals"), X("Equality operator for given types not supported"));
  }

  return XPath2Utils::equals(((ATQNameOrDerivedImpl*)(const AnyAtomicType*)target)->_name, _name) &&
         XPath2Utils::equals(((ATQNameOrDerivedImpl*)(const AnyAtomicType*)target)->_uri, _uri) ;  
  
}

const XMLCh* ATQNameOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME;
}

AnyAtomicType::AtomicObjectType ATQNameOrDerivedImpl::getPrimitiveTypeIndex() const {
    return this->getTypeIndex();
}
