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
 * $Id: ATQNameOrDerived.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATQNAMEORDERIVED_HPP
#define _ATQNAMEORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

class DynamicContext;

class XQILLA_API ATQNameOrDerived : public AnyAtomicType
{
public:
  typedef RefCountPointer<const ATQNameOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the URI */
  virtual const XMLCh* getURI() const = 0;

  /* returns the prefix */
  virtual const XMLCh* getPrefix() const = 0;

  /* returns the name */
  virtual const XMLCh* getName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATQNameOrDerived::Ptr &other, const DynamicContext *context) const = 0;
 
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATQNAMEORDERIVED_HPP
