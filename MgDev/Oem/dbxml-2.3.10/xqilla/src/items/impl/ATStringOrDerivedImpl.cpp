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
 * $Id: ATStringOrDerivedImpl.cpp,v 1.13 2006/11/01 16:37:20 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include "ATStringOrDerivedImpl.hpp"
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATFloatOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/regx/RegxUtil.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>


ATStringOrDerivedImpl::
ATStringOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context): 
    ATStringOrDerived(),
    _typeName(typeName),
    _typeURI(typeURI) { 
    
  _value = context->getMemoryManager()->getPooledString(value);      
}

void *ATStringOrDerivedImpl::getInterface(const XMLCh *name) const
{
  if(name == Item::gXQilla) {
    return (void*)this;
  }
  return 0;
}

/* Get the name of the primitive type (basic type) of this type
 * (ie "decimal" for xs:decimal) */
const XMLCh* ATStringOrDerivedImpl::getPrimitiveTypeName() const {
  return this->getPrimitiveName();
}

const XMLCh* ATStringOrDerivedImpl::getPrimitiveName()  {
  return XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_STRING;
}

/* Get the name of this type  (ie "integer" for xs:integer) */
const XMLCh* ATStringOrDerivedImpl::getTypeName() const {
  return _typeName;
}

/* Get the namespace URI for this type */
const XMLCh* ATStringOrDerivedImpl::getTypeURI() const {
  return _typeURI; 
}

AnyAtomicType::AtomicObjectType ATStringOrDerivedImpl::getTypeIndex() {
  return AnyAtomicType::STRING;
} 

AnyAtomicType::Ptr ATStringOrDerivedImpl::castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const {
  switch (targetIndex) {
    case DOUBLE: {
      const XMLCh* upCase = XPath2Utils::toUpper(_value, context->getMemoryManager());
      return (const AnyAtomicType::Ptr)context->getItemFactory()->createDoubleOrDerived(targetURI, targetType, upCase, context);
    }
    case FLOAT: {
      const XMLCh* upCase = XPath2Utils::toUpper(_value, context->getMemoryManager());
      return (const AnyAtomicType::Ptr)context->getItemFactory()->createFloatOrDerived(targetURI, targetType, upCase, context);
    }
    default: return AnyAtomicType::castAsInternal(targetIndex, targetURI, targetType, context);
  }

}


/* returns the XMLCh* (canonical) representation of this type */
const XMLCh* ATStringOrDerivedImpl::asString(const DynamicContext* context) const {
  return _value;
}

/* returns this string as a set of codepoints */
Result ATStringOrDerivedImpl::asCodepoints(const DynamicContext* context) const {
  unsigned int length = this->getLength();
  Sequence result(length,context->getMemoryManager());
  for(unsigned int i=0; i<length; i++) {
    if(XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isHighSurrogate(_value[i]) && (i+1)<length && 
       XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isLowSurrogate(_value[i+1]))
    {
      result.addItem(context->getItemFactory()->createInteger((long int)XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::composeFromSurrogate(_value[i], _value[i+1]), context));
      i++;
    }
    else
     result.addItem(context->getItemFactory()->createInteger((long int)_value[i], context));
  } 
  return result;
}

/* returns the substring starting at startingLoc of given length */
ATStringOrDerived::Ptr ATStringOrDerivedImpl::substring(const Numeric::Ptr &startingLoc, const Numeric::Ptr &length, const DynamicContext* context) const {
  const ATDecimalOrDerived::Ptr one = context->getItemFactory()->createInteger(1, context);
  long nLength=this->getLength();
  const ATDecimalOrDerived::Ptr strLength = context->getItemFactory()->createInteger(nLength, context); 
 
  // More specifically, returns the characters in $sourceString whose position $p obeys:
  //    fn:round($startingLoc) <= $p < fn:round($startingLoc) + fn:round($length)
  const Numeric::Ptr startIndex = startingLoc->round(context);
  const Numeric::Ptr endIndex = startIndex->add(length->round(context), context);

  if(startIndex->greaterThan(strLength, context) || startIndex->greaterThan(endIndex, context)) {
    return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
  }

  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buffer(1023, context->getMemoryManager());
  Numeric::Ptr index = one;

  // i is kept at one less than index, since XMLCh* start at index 0
  int i = 0;
  // for(index = one; index <= strLength; index++)
  for(; !index->greaterThan(strLength, context); index = index->add(one, context), i++) {
    // if (index >= startIndex and index < endIndex), add the char at i
    if(!index->lessThan(startIndex, context) && index->lessThan(endIndex, context))
    {
      buffer.append(_value[i]);
      // if it's a non-BMP char, add the following too
      if(XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isHighSurrogate(_value[i]) && (i+1)<nLength && 
         XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isLowSurrogate(_value[i+1]))
        buffer.append(_value[++i]);
    }
    // otherwise, skip the next one too
    else if(XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isHighSurrogate(_value[i]) && (i+1)<nLength && 
            XERCES_CPP_NAMESPACE_QUALIFIER RegxUtil::isLowSurrogate(_value[i+1]))
      i++;
  }

  return context->getItemFactory()->createString(buffer.getRawBuffer(), context);
}

/* returns the substring that occurs after the first occurence of pattern */
ATStringOrDerived::Ptr ATStringOrDerivedImpl::substringAfter(const ATStringOrDerived::Ptr &pattern, Collation* collation, const DynamicContext* context) const {
  unsigned int containerLength = this->getLength(); 
  unsigned int patternLength = pattern->getLength();  

	if(patternLength == 0) {
		return this;
	}

	if(patternLength > containerLength) {
		return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
	}

	const XMLCh* patternStr = pattern->asString(context);

	// search pattern must be shorter than (or same length as) string being searched
	for(unsigned int i = 0; i <= (containerLength - patternLength); i++) 
	{
		// extract a substring of the same length of the pattern and compare them
		const XMLCh *substr=XPath2Utils::subString(_value, i, patternLength, context->getMemoryManager());
		bool result = (collation->compare(substr,patternStr)==0);

		if(result)
		{
			int index = i + patternLength;
			const XMLCh* value = XPath2Utils::subString(_value, index, containerLength - index, context->getMemoryManager());
			return context->getItemFactory()->createString(value, context);
		}
	}
	
	return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
}

/* returns the substring that occurs before the first occurence of pattern */
ATStringOrDerived::Ptr ATStringOrDerivedImpl::substringBefore(const ATStringOrDerived::Ptr &pattern, Collation* collation, const DynamicContext* context) const {
  unsigned int containerLength = this->getLength(); 
  unsigned int patternLength = pattern->getLength();  

	if(patternLength == 0) {
		return this;
	}

	if(patternLength > containerLength) {
		return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
	}

	const XMLCh* patternStr = pattern->asString(context);

	// search pattern must be shorter than (or same length as) string being searched
	for(unsigned int i = 0; i <= (containerLength - patternLength); i++) 
	{
		// extract a substring of the same length of the pattern and compare them
		const XMLCh *substr=XPath2Utils::subString(_value, i, patternLength, context->getMemoryManager());
		bool result = (collation->compare(substr,patternStr)==0);

		if(result)
		{
      const XMLCh* value = XPath2Utils::subString(_value, 0, i, context->getMemoryManager());
			return context->getItemFactory()->createString(value, context);
    }
  }
	return context->getItemFactory()->createString(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString, context);
}

unsigned int ATStringOrDerivedImpl::getLength() const {
  return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(_value);
}

/* returns true if the two objects' are equal (string comparison)
   * false otherwise */
bool ATStringOrDerivedImpl::equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const {
  if(this->getPrimitiveTypeIndex() != target->getPrimitiveTypeIndex()) {
    XQThrow2(IllegalArgumentException,X("ATStringOrDerivedImpl::equals"), X("Equality operator for given types not supported [err:XPTY0004]"));
  }
  return XPath2Utils::equals(target->asString(context), _value);  
}

AnyAtomicType::AtomicObjectType ATStringOrDerivedImpl::getPrimitiveTypeIndex() const {
  return this->getTypeIndex();
}
