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
 * $Id: ATNotationOrDerivedImpl.hpp,v 1.9 2006/11/01 16:37:20 jpcs Exp $
 */

#ifndef _ATNOTATIONORDERIVEDIMPL_HPP
#define _ATNOTATIONORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/items/ATNotationOrDerived.hpp>

#include <xqilla/framework/XQillaExport.hpp>

class DynamicContext;

class XQILLA_API ATNotationOrDerivedImpl : public ATNotationOrDerived 
{

public:

  /* constructor */
  ATNotationOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const StaticContext* context);

  virtual void *getInterface(const XMLCh *name) const;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const;

  /* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;
  
  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATNotationOrDerived::Ptr &other, const DynamicContext *context) const;
 
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

private:

  /*The value of this NOTATION*/
  const XMLCh* _notation;

  /*The uri value of this NOTATION*/
  const XMLCh* _uri;

  /*The local name value of this NOTATION */
  const XMLCh* _name;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATNOTATIONORDERIVEDIMPL_HPP
