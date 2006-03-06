/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATSTRINGORDERIVED_HPP
#define _ATSTRINGORDERIVED_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <pathan/Pathan.hpp>
#include "AnyAtomicType.hpp"
#include "Numeric.hpp"

class Collation;
class Result;
class DynamicContext;

class PATHAN_EXPORT ATStringOrDerived : public AnyAtomicType
{
public:
  typedef RefCountPointer<const ATStringOrDerived> Ptr;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns this string as a set of codepoints */
  virtual Result asCodepoints(const DynamicContext* context) const = 0;

  /* returns the length of this string */
  virtual unsigned int getLength() const = 0;

  /* returns the substring starting at startingLoc of given length */
  virtual ATStringOrDerived::Ptr substring(const Numeric::Ptr &startingLoc, const Numeric::Ptr &length, const DynamicContext* context) const = 0;

  /* returns the substring that occurs after the first occurence of pattern */
  virtual ATStringOrDerived::Ptr substringAfter(const ATStringOrDerived::Ptr &pattern, Collation* collation, const DynamicContext* context) const = 0;

  /* returns the substring that occurs before the first occurence of pattern */
  virtual ATStringOrDerived::Ptr substringBefore(const ATStringOrDerived::Ptr &pattern, Collation* collation, const DynamicContext* context) const = 0;

  /* returns true if the two objects' URI are equal (string comparison)
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const = 0;
};

#endif //  _ATSTRINGORDERIVED_HPP
