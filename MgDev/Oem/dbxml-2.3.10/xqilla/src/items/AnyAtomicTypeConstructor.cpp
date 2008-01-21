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
 * $Id: AnyAtomicTypeConstructor.cpp,v 1.9 2006/11/01 16:37:20 jpcs Exp $
 */

#include <sstream>

#include "../config/xqilla_config.h"
#include <xqilla/items/AnyAtomicTypeConstructor.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/utils/UTF8Str.hpp>

AnyAtomicTypeConstructor::AnyAtomicTypeConstructor(const XMLCh* typeURI,
                                                   const XMLCh* typeName,
                                                   const XMLCh* value,
                                                   AnyAtomicType::AtomicObjectType primitiveType)
  : _typeURI(typeURI),
    _typeName(typeName),
    _value(value),
    _primitiveType(primitiveType)
{
  _sType.flags = StaticType::getFlagsFor(primitiveType);
}

Item::Ptr AnyAtomicTypeConstructor::createItem(const DynamicContext* context) const
{
  return context->getItemFactory()->createDerivedFromAtomicType(_primitiveType, _typeURI, _typeName, _value, context);
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
