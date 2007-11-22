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
 * $Id: IllegalArgumentException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*
   Illegal Argument exception.  Should be thrown when arguments given are not as expected
*/

#ifndef _ILLEGALARGUMENTEXCEPTION_HPP
#define _ILLEGALARGUMENTEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for tests. Inherit from this if you need to throw an exception
    in the test suite.*/
class XQILLA_API IllegalArgumentException : public XQException
{
public:
  
  IllegalArgumentException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("IllegalArgumentException"), functionName, reason, info, file, line) {};
};

#endif // _ILLEGALARGUMENTEXCEPTION_HPP
