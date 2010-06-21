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
 * $Id: XPath2ErrorException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  XPath2ErrorException - handles cases in the specification where the `error value' is returned

*/

#ifndef _XPATH2ERROREXCEPTON_HPP
#define _XPATH2ERROREXCEPTON_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for data items */
class XQILLA_API XPath2ErrorException : public XQException
{
public:
  
  XPath2ErrorException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("XPath2ErrorException"), functionName, reason, info, file, line) {};
};

#endif // _XPATH2ERROREXCEPTON_HPP
