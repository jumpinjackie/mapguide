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
 * $Id: TypeNotFoundException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _TYPENOTFOUNDEXCEPTION_HPP
#define _TYPENOTFOUNDEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>

class XQILLA_API TypeNotFoundException : public XQException
{
public:
  
  TypeNotFoundException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("TypeNotFoundException"), functionName, reason, info, file, line) {};
};

#endif // _TYPENOTFOUNDEXCEPTION_HPP
