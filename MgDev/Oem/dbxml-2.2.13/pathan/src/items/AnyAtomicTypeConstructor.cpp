/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <sstream>

#include "../config/pathan_config.h"
#include <pathan/AnyAtomicTypeConstructor.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/internal/utils/UTF8Str.hpp>

AnyAtomicTypeConstructor::AnyAtomicTypeConstructor(const XMLCh* typeURI,
                                                   const XMLCh* typeName,
                                                   const XMLCh* value,
                                                   bool numeric)
  : _typeURI(typeURI),
    _typeName(typeName),
    _value(value),
    _numeric(numeric)
{
}

bool AnyAtomicTypeConstructor::isNumeric() const
{
  return _numeric;
}

Item::Ptr AnyAtomicTypeConstructor::createItem(const DynamicContext* context) const
{
  return context->getPathanFactory()->createDerivedFromAtomicType(_typeURI, _typeName, _value, context);
}

std::string AnyAtomicTypeConstructor::asString(const DynamicContext* context) const
{
  std::ostringstream s;

  s << "<AnyAtomicTypeConstructor";
  s << " value=\"" << UTF8(_value);
  s << "\" typeuri=\"" << UTF8(_typeURI);
  s << "\" typename=\"" << UTF8(_typeName);
  s << "\"/>";

  return s.str();
}
