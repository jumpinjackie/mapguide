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
 * $Id: ATGYearOrDerivedImpl.hpp,v 1.2 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATGYEARORDERIVEDIMPL_HPP
#define _ATGYEARORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/items/ATGYearOrDerived.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Timezone.hpp>
#include <xqilla/mapm/m_apm.h>

class DynamicContext;

class XQILLA_API ATGYearOrDerivedImpl : public ATGYearOrDerived 
{

public:
  /* constructor */
  ATGYearOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context = 0);

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

  /* returns true if the two objects are equal
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;
  
  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATGYearOrDerived::Ptr &other, const DynamicContext *context) const;
 
  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const;

  /** Sets the timezone to the given timezone.*/
  virtual ATGYearOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;
  
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

  static MAPM parseGYear(const XMLCh* const value, const MAPM &implicitTimezone);

private:

  /* parse the gDay */
  void setGYear(const XMLCh* const value);

  MAPM buildReferenceDateTime(const DynamicContext *context) const;
  
  /*The value of this gDay*/
  MAPM _YY;

  /* whether this gDay has a timezone value*/
  bool _hasTimezone;

  /* the timezone value, if it exist */
  Timezone::Ptr timezone_;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATGYEARORDERIVEDIMPL_HPP
