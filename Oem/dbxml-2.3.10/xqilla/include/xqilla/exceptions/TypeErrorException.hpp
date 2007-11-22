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
 * $Id: TypeErrorException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _TYPEERROREXCEPTION_HPP
#define _TYPEERROREXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** 
 * This means that an error was found while manipulating typed data
 */

class XQILLA_API TypeErrorException : public XQException
{
public:
  TypeErrorException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("TypeErrorException"), functionName, reason, info, file, line) {};
};

#endif // _TYPEERROREXCEPTION_HPP
