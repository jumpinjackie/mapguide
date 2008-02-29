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
 * $Id: NumericTypeConstructor.hpp,v 1.3 2006/11/17 18:08:07 jpcs Exp $
 */

#ifndef _NUMERICTYPECONSTUCTOR_HPP
#define _NUMERICTYPECONSTUCTOR_HPP

#include <xqilla/items/ItemConstructor.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/ast/StaticType.hpp>
#include <xqilla/mapm/m_apm.h>

class XQILLA_API NumericTypeConstructor : public ItemConstructor {
public:
  NumericTypeConstructor(const XMLCh* typeURI,
                         const XMLCh* typeName,
                         const MAPM& value,
                         AnyAtomicType::AtomicObjectType primitiveType,
                         XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
  virtual ~NumericTypeConstructor() {}

  virtual const StaticType &getStaticType() const { return _sType; }

  virtual Item::Ptr createItem(const DynamicContext* context) const;
  virtual std::string asString(const DynamicContext* context) const;

  virtual const XMLCh* getTypeURI() const { return _typeURI; }
  virtual const XMLCh* getTypeName() const { return _typeName; }

private:
  const XMLCh *_typeURI;
  const XMLCh *_typeName;
  M_APM_struct _value;
  AnyAtomicType::AtomicObjectType _primitiveType;
  StaticType _sType;
};

#endif

