/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <sstream>

#include "../config/pathan_config.h"
#include <pathan/ATQNameConstructor.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/items/impl/ATQNameOrDerivedImpl.hpp>

#include <pathan/internal/utils/UTF8Str.hpp>

ATQNameConstructor::ATQNameConstructor(const XMLCh* typeURI,
				       const XMLCh* typeName,
				       const XMLCh* uri,
                       const XMLCh* prefix,
				       const XMLCh* localname)
  : _typeURI(typeURI),
    _typeName(typeName),
    _uri(uri),
    _prefix(prefix),
    _localname(localname)
{
}

bool ATQNameConstructor::isNumeric() const
{
  return false;
}

Item::Ptr ATQNameConstructor::createItem(const DynamicContext* context) const
{
  return new ATQNameOrDerivedImpl(_typeURI, _typeName, _uri, _prefix, _localname, context);
}

std::string ATQNameConstructor::asString(const DynamicContext* context) const
{
  std::ostringstream s;

  s << "<ATQNameConstructor";
  s << " uri=\"" << UTF8(_uri);
  s << "\" prefix=\"" << UTF8(_prefix);
  s << "\" localname=\"" << UTF8(_localname);
  s << "\" typeuri=\"" << UTF8(_typeURI);
  s << "\" typename=\"" << UTF8(_typeName);
  s << "\"/>";

  return s.str();
}
