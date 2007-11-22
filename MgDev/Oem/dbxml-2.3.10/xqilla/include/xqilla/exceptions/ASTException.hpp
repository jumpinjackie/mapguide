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
 * $Id: ASTException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  ASTException - exception class for ASTNodes

*/

#ifndef _ASTEXCEPTION_HPP
#define _ASTEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for data items */

class XQILLA_API ASTException : public XQException
{
public:

  ASTException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("ASTException"), functionName, reason, info, file, line) {};
};

#endif
