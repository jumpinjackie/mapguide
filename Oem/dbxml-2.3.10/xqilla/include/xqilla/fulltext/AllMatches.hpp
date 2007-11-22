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
 * $Id: AllMatches.hpp,v 1.3 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _ALLMATCHES_HPP
#define _ALLMATCHES_HPP


#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xqilla/fulltext/Match.hpp>

class DynamicContext;

class XQILLA_API AllMatches : public ReferenceCounted, public LocationInfo
{
public:
  typedef RefCountPointer<AllMatches> Ptr;

  virtual ~AllMatches() {}

  /// Get the next Match from the iterator. Returns null if the is no next value.
  virtual Match::Ptr next(DynamicContext *context) = 0;

protected:
  AllMatches(const LocationInfo *o)
  {
    setLocationInfo(o);
  }
};

#endif
