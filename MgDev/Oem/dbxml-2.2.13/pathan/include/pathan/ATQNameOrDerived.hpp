/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATQNAMEORDERIVED_HPP
#define _ATQNAMEORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/Pathan.hpp>
#include <pathan/AnyAtomicType.hpp>

class DynamicContext;

class PATHAN_EXPORT ATQNameOrDerived : public AnyAtomicType
{
public:
  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

	/* returns the URI */
	virtual const XMLCh* getURI(void) const = 0;

	/* returns the prefix */
	virtual const XMLCh* getPrefix(void) const = 0;

	/* returns the name */
	virtual const XMLCh* getName(void) const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATQNAMEORDERIVED_HPP
