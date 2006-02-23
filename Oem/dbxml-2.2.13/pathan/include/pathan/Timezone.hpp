/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _TZONE_HPP
#define _TZONE_HPP

#include "Pathan.hpp"
#include "ReferenceCounted.hpp"
#include "ATDurationOrDerived.hpp"
#include "ATDecimalOrDerived.hpp"

#include <xercesc/util/XercesDefs.hpp>

class DynamicContext;
class DynamicContext;

class PATHAN_EXPORT Timezone : public ReferenceCounted
{
public :
  typedef RefCountPointer<const Timezone> Ptr;

  /**
   * Constructor.  Sets _hh and _mm as specified.
   */
  Timezone(const ATDecimalOrDerived::Ptr &hour, const ATDecimalOrDerived::Ptr &minute, const DynamicContext* context);

  /**
   * Constructor.  Sets _hh and _mm as specified.
   */
  Timezone(int hour, int minute);


  /**
   * Constructor. Creates a Timezone representing the given number of seconds.
   */
  Timezone(int seconds);

  /**
   * Constructor. Creates a Timezone representing the given duration.
   */
  Timezone(const ATDurationOrDerived::Ptr &duration, const DynamicContext* context);

  /**
   * Copy constructor.
   */
  Timezone(const Timezone & other);

  /** Returns true if other is equal to this, 
   * false otherwise 
   */
  bool equals(const Timezone::Ptr &other) const;

  /** Returns true if this is greater than other, 
   * false otherwise 
   */  bool greaterThan(const Timezone::Ptr &other) const;

  /** Returns true if this is greater than other, 
   * false otherwise 
   */  bool lessThan(const Timezone::Ptr &other) const;

  ATDurationOrDerived::Ptr asDayTimeDuration(const DynamicContext* context) const;
  
  /**
   * Returns a string representation of this Timezone.  If positive, does
   * print leading '+' sign, and if UTC returns "Z".
   */
  const XMLCh* asString(const DynamicContext* context) const;

  /**
   * Returns a string representation of this Timezone.  If positive, does not
   * print leading '+' sign, and if UTC prints 00:00.
   */
  const XMLCh* printTimezone(const DynamicContext* context) const;
  
  /**
   * Returns _mm.
   */
  const int getMinutes() const;

  /**
   * Returns _hh.
   */
  const int getHours() const;

private:
  int _hh;    // time zone hour
  int _mm;    // time zone minute

  void init(int seconds);
  void validate() const;
};

#endif // _TZONE_HPP


