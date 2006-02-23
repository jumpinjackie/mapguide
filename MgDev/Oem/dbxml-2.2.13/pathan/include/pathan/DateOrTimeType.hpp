/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATEORTIMETYPE_HPP
#define _DATEORTIMETYPE_HPP

#include <xercesc/util/XMLString.hpp>
#include <pathan/AnyAtomicType.hpp>

#include <pathan/Pathan.hpp>

class ATDecimalOrDerived;
class ATDurationOrDerived;

class PATHAN_EXPORT DateOrTimeType : public AnyAtomicType
{
public:
  typedef RefCountPointer<const DateOrTimeType> Ptr;

  /* is this type date or time based?  Return true by default */
  virtual bool isDateOrTimeTypeValue() const;

  /**
   * Returns true if the timezone is defined for this object, false otherwise.
   */
  virtual bool hasTimezone() const = 0;

};

#endif //  _ATDATETIMEORDERIVED_HPP
