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
 * $Id: ATQNameConstructor.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATQNAMECONSTUCTOR_HPP
#define _ATQNAMECONSTUCTOR_HPP

#include <xqilla/items/ItemConstructor.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/ast/StaticType.hpp>

class XQILLA_API ATQNameConstructor : public ItemConstructor {
public:
  ATQNameConstructor(const XMLCh* typeURI,
                     const XMLCh* typeName,
                     const XMLCh* uri,
                     const XMLCh* prefix,
                     const XMLCh* localname);
  virtual ~ATQNameConstructor() {}

  virtual const StaticType &getStaticType() const { return _sType; }

  virtual Item::Ptr createItem(const DynamicContext* context) const;
  virtual std::string asString(const DynamicContext* context) const;

  virtual const XMLCh* getTypeURI() const { return _typeURI; }
  virtual const XMLCh* getTypeName() const { return _typeName; }

private:
  const XMLCh *_typeURI;
  const XMLCh *_typeName;
  const XMLCh *_uri;
  const XMLCh *_prefix;
  const XMLCh *_localname;
  StaticType _sType;
};

#endif
