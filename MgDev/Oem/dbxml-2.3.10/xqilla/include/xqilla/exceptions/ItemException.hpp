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
 * $Id: ItemException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*
   Item Exception.  Should be thrown only from Item hierarchy
*/

#ifndef _ITEMEXCEPTION_HPP
#define _ITEMEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for tests. Inherit from this if you need to throw an exception
    in the test suite.*/
class XQILLA_API ItemException : public XQException
{
public:
  
  ItemException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("ItemException"), functionName, reason, info, file, line) {};
};

#endif // _ITEMEXCEPTION_HPP
