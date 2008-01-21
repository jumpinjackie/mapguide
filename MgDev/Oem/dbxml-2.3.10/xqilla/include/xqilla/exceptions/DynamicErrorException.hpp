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
 * $Id: DynamicErrorException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _DYNAMICERROREXCEPTION_HPP
#define _DYNAMICERROREXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** 
 * This means that an error was found while evaluating the expression
 */

class XQILLA_API DynamicErrorException : public XQException
{
public:
  DynamicErrorException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("DynamicErrorException"), functionName, reason, info, file, line) {};
};

#endif // _DYNAMICERROREXCEPTION_HPP
