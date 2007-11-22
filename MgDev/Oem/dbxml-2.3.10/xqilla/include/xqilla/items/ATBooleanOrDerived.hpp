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
 * $Id: ATBooleanOrDerived.hpp,v 1.8 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ATBOOLEANORDERIVED_HPP
#define _ATBOOLEANORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

#include <xqilla/framework/XQillaExport.hpp>

class XPath2MemoryManager;

class XQILLA_API ATBooleanOrDerived : public AnyAtomicType
{
public:
  typedef RefCountPointer<const ATBooleanOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type 
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects have the same boolean value
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATBooleanOrDerived::Ptr &other, const DynamicContext *context) const = 0;
  
  /* returns true if boolean value evaluates to true
   * false otherwise */
  virtual bool isTrue() const = 0;

  /* returns false if boolean value evaluates to true
   * true otherwise */
  virtual bool isFalse() const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATBOOLEANORDERIVED_HPP
