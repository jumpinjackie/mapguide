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
 * $Id: ATHexBinaryOrDerivedImpl.cpp,v 1.14 2006/11/01 16:37:20 jpcs Exp $
 */
  
#include "../../config/xqilla_config.h"
#include "ATHexBinaryOrDerivedImpl.hpp"
#include "ATBase64BinaryOrDerivedImpl.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/HexBin.hpp>
#include <xercesc/util/Base64.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

ATHexBinaryOrDerivedImpl::
ATHexBinaryOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATHexBinaryOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  XMLCh* tempValue=XMLString::replicate(value, context->getMemoryManager());
  XMLString::upperCase(tempValue);
  _hexBinaryData = context->getMemoryManager()->getPooledString(tempValue);
  XMLString::release((void**)&tempValue, context->getMemoryManager());
}

void *ATHexBinaryOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gXQilla) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATHexBinaryOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATHexBinaryOrDerivedImpl::getPrimitiveName()  {
  return SchemaSymbols::fgDT_HEXBINARY;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATHexBinaryOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATHexBinaryOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATHexBinaryOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::HEX_BINARY;
} 

/* If possible, cast this type to the target type */
AnyAtomicType::Ptr ATHexBinaryOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const
{
  switch(targetIndex) {
    case BASE_64_BINARY: {
      XMLByte* binData=HexBin::decodeToXMLByte(_hexBinaryData, context->getMemoryManager());
      unsigned int length=0;
      XMLByte* base64Data=Base64::encode(binData, 
                                                                        XMLString::stringLen(_hexBinaryData)/2, 
                                                                        &length, 
                                                                        context->getMemoryManager()); 
      XMLCh* uniBase64=(XMLCh*)context->getMemoryManager()->allocate((length+1)*sizeof(XMLCh));
      unsigned int i;
      for(i=0;i<length;i++)
          uniBase64[i]=(XMLCh)base64Data[i];
      uniBase64[i]=0;
      // replace #xA with #x20, collapse multiple spaces
      XMLString::collapseWS(uniBase64, context->getMemoryManager());
      ATBase64BinaryOrDerivedImpl* retVal=new ATBase64BinaryOrDerivedImpl(targetURI, targetType, uniBase64, context);
      context->getMemoryManager()->deallocate(uniBase64);
      context->getMemoryManager()->deallocate(binData);
      context->getMemoryManager()->deallocate(base64Data);
      return retVal;
    }
    default: {
      return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
    }
  }
}

/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATHexBinaryOrDerivedImpl::asString(const DynamicContext* context) const {
  return _hexBinaryData;
}

/* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
bool ATHexBinaryOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    XQThrow2(::IllegalArgumentException,X("ATHexBinaryOrDerivedImpl::equals"),
            X("Equality operator for given types not supported [err:XPTY0004]"));
  }
  return compare((const ATHexBinaryOrDerived *)target.get(), context) == 0;
}

int ATHexBinaryOrDerivedImpl::compare(const ATHexBinaryOrDerived::Ptr &other, const DynamicContext *context) const
{
  return XPath2Utils::compare(_hexBinaryData, ((const ATHexBinaryOrDerivedImpl *)other.get())->_hexBinaryData);
}

AnyAtomicType::AtomicObjectType ATHexBinaryOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
