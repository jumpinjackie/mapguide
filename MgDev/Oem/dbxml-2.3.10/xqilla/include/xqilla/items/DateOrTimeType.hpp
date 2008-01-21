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
 * $Id: DateOrTimeType.hpp,v 1.7 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _DATEORTIMETYPE_HPP
#define _DATEORTIMETYPE_HPP

#include <xercesc/util/XMLString.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

#include <xqilla/framework/XQillaExport.hpp>

class ATDecimalOrDerived;
class ATDurationOrDerived;

class XQILLA_API DateOrTimeType : public AnyAtomicType
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
