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
 * $Id: NumericTypeConstructor.cpp,v 1.3 2006/11/17 18:08:07 jpcs Exp $
 */

#include <sstream>
#include <assert.h>

#include "../config/xqilla_config.h"
#include <xqilla/items/NumericTypeConstructor.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/exceptions/TypeNotFoundException.hpp>
#include "../exceptions/InvalidLexicalSpaceException.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NumericTypeConstructor::NumericTypeConstructor(const XMLCh* typeURI,
                                               const XMLCh* typeName,
                                               const MAPM& value,
                                               AnyAtomicType::AtomicObjectType primitiveType,
                                               MemoryManager *mm)
  : _typeURI(typeURI),
    _typeName(typeName),
    _primitiveType(primitiveType)
{
  _sType.flags = StaticType::getFlagsFor(_primitiveType);

  memset(&_value, 0, sizeof(_value));

	const M_APM cval = value.c_struct();

  _value.m_apm_datalength = cval->m_apm_datalength;
  _value.m_apm_exponent = cval->m_apm_exponent;
  _value.m_apm_sign = cval->m_apm_sign;

  int len = (cval->m_apm_datalength + 1) >> 1;
  _value.m_apm_data = (UCHAR*)mm->allocate(len);

  memcpy(_value.m_apm_data, cval->m_apm_data, len);
}

Item::Ptr NumericTypeConstructor::createItem(const DynamicContext* context) const
{
  // Use the C API to copy our fake MAPM
  MAPM copy;
	m_apm_copy(const_cast<M_APM>(copy.c_struct()), const_cast<M_APM>(&_value));

  Numeric::Ptr retVal;
  switch(_primitiveType) {
  case AnyAtomicType::DECIMAL:
    retVal = context->getItemFactory()->createDecimalOrDerived(_typeURI, _typeName, copy, context);
    break;
  case AnyAtomicType::FLOAT:
    retVal = context->getItemFactory()->createFloatOrDerived(_typeURI, _typeName, copy, context);
    break;
  case AnyAtomicType::DOUBLE:
    retVal = context->getItemFactory()->createDoubleOrDerived(_typeURI, _typeName, copy, context);
    break;
  default:
    assert(false);
    break;
  }
  // check if it's a valid instance
  DatatypeValidator* validator=context->getDocumentCache()->getDatatypeValidator(_typeURI, _typeName);
  if(!validator) {
    XMLBuffer buf(1023, context->getMemoryManager());
    buf.append(X("Type "));
    buf.append(_typeURI);
    buf.append(chColon);
    buf.append(_typeName);
    buf.append(X(" not found"));
    XQThrow2(TypeNotFoundException, X("NumericTypeConstructor::createItem"), buf.getRawBuffer());
  }
  try {
    const XMLCh* valueToValidate=retVal->asString(context);
    validator->validate(valueToValidate, 0, context->getMemoryManager());
  } catch (XMLException &e) {
    XMLBuffer buf(1023, context->getMemoryManager());
    buf.append(e.getMessage());
    buf.append(X(" [err:FORG0001]"));
    XQThrow2(InvalidLexicalSpaceException, X("NumericTypeConstructor::createItem"), buf.getRawBuffer());
  }
  return retVal;
}

std::string NumericTypeConstructor::asString(const DynamicContext* context) const
{
  std::ostringstream s;

  s << "<NumericTypeConstructor";
  char obuf[1024];
  m_apm_to_string_mt(obuf, _value.m_apm_datalength, const_cast<M_APM>(&_value));
  s << " value=\"" << obuf;
  s << "\" typeuri=\"" << UTF8(_typeURI);
  s << "\" typename=\"" << UTF8(_typeName);
  s << "\"/>";

  return s.str();
}
