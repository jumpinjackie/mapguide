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
 * $Id: StaticErrorException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _STATICERROREXCEPTION_HPP
#define _STATICERROREXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** 
 * This means that an error was found while parsing or when doing static analysis of the expression
 */

class XQILLA_API StaticErrorException : public XQException
{
public:
  StaticErrorException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("StaticErrorException"), functionName, reason, info, file, line) {};
};

#endif // _STATICERROREXCEPTION_HPP
