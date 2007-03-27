/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATQNAMECONSTUCTOR_HPP
#define _ATQNAMECONSTUCTOR_HPP

#include "ItemConstructor.hpp"
#include <xercesc/util/XercesDefs.hpp>

class PATHAN_EXPORT ATQNameConstructor : public ItemConstructor {
public:
  ATQNameConstructor(const XMLCh* typeURI,
		     const XMLCh* typeName,
		     const XMLCh* uri,
             const XMLCh* prefix,
		     const XMLCh* localname);
  /** do-nothing virtual dtor */	
  virtual ~ATQNameConstructor() {}

  virtual bool isNumeric() const;

  virtual Item::Ptr createItem(const DynamicContext* context) const;
  virtual std::string asString(const DynamicContext* context) const;

private:
  const XMLCh *_typeURI;
  const XMLCh *_typeName;
  const XMLCh *_uri;
  const XMLCh *_prefix;
  const XMLCh *_localname;
};

#endif
