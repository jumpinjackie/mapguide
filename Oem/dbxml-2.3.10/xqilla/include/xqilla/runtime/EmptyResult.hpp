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
 * $Id: EmptyResult.hpp,v 1.9 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _EMPTYRESULT_HPP
#define _EMPTYRESULT_HPP

#include <xqilla/runtime/ResultImpl.hpp>

class XQILLA_API EmptyResult : public ResultImpl
{
public:
  EmptyResult(const LocationInfo *o);

  Item::Ptr next(DynamicContext *context);

  std::string asString(DynamicContext *context, int indent) const;
};

#endif
