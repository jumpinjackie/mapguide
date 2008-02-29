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
 * $Id: AnyAtomicTypeConstructor.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ANYATOMICTYPECONSTUCTOR_HPP
#define _ANYATOMICTYPECONSTUCTOR_HPP

#include <xqilla/items/ItemConstructor.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/ast/StaticType.hpp>

class XQILLA_API AnyAtomicTypeConstructor : public ItemConstructor {
public:
  AnyAtomicTypeConstructor(const XMLCh* typeURI,
                           const XMLCh* typeName,
                           const XMLCh* value,
                           AnyAtomicType::AtomicObjectType primitiveType);
  virtual ~AnyAtomicTypeConstructor() {}

  virtual const StaticType &getStaticType() const { return _sType; }

  virtual Item::Ptr createItem(const DynamicContext* context) const;
  virtual std::string asString(const DynamicContext* context) const;

  virtual const XMLCh* getTypeURI() const { return _typeURI; }
  virtual const XMLCh* getTypeName() const { return _typeName; }

private:
  const XMLCh *_typeURI;
  const XMLCh *_typeName;
  const XMLCh *_value;
  AnyAtomicType::AtomicObjectType _primitiveType;
  StaticType _sType;
};

#endif

