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
 * $Id: ATQNameConstructor.cpp,v 1.9 2006/11/01 16:37:20 jpcs Exp $
 */

#include <sstream>

#include "../config/xqilla_config.h"
#include <xqilla/items/ATQNameConstructor.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/impl/ATQNameOrDerivedImpl.hpp>

#include <xqilla/utils/UTF8Str.hpp>

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
  _sType.flags = StaticType::QNAME_TYPE;
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
