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
 * $Id: XPath2TypeCastException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  XPath2TypeCastException - exception class for casting errors

*/

#ifndef _XPATH2TYPECASTEXCEPTION_HPP
#define _XPATH2TYPECASTEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for data items */
class XQILLA_API XPath2TypeCastException : public XQException
{
public:

  XPath2TypeCastException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("XPath2TypeCastException"), functionName, reason, info, file, line) {};
};

#endif // _XPATH2TYPECASTEXCEPTION_HPP
