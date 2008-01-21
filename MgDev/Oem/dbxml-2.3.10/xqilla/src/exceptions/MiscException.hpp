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
 * $Id: MiscException.hpp,v 1.9 2006/11/01 16:37:19 jpcs Exp $
 */

/*

  XPath2ErrorException - handles cases in the specification where the `error value' is returned

*/

#ifndef _MISCEXCEPTION_HPP
#define _MISCEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for miscellaneous errors */
class XQILLA_API MiscException : public XQException
{
public:
  
  MiscException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("MiscException"), functionName, reason, info, file, line) {};
};

#endif // _MISCEXCEPTION_HPP
