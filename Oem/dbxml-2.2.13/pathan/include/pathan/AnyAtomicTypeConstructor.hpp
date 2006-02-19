/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ANYATOMICTYPECONSTUCTOR_HPP
#define _ANYATOMICTYPECONSTUCTOR_HPP

#include "ItemConstructor.hpp"
#include <xercesc/util/XercesDefs.hpp>

class PATHAN_EXPORT AnyAtomicTypeConstructor : public ItemConstructor {
public:
  AnyAtomicTypeConstructor(const XMLCh* typeURI,
                           const XMLCh* typeName,
                           const XMLCh* value,
                           bool numeric);
  /** do-nothing virtual dtor */	
  virtual ~AnyAtomicTypeConstructor() {}

  virtual bool isNumeric() const;

  virtual Item::Ptr createItem(const DynamicContext* context) const;
  virtual std::string asString(const DynamicContext* context) const;

private:
  const XMLCh *_typeURI;
  const XMLCh *_typeName;
  const XMLCh *_value;
  bool _numeric;
};

#endif

