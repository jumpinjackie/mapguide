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
 * $Id: ContextException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  ContextException

*/

#ifndef _CONTEXTEXCEPTION_HPP
#define _CONTEXTEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for DSLPath parser */

class XQILLA_API ContextException : public XQException
{
public:

  ContextException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("ContextException"), functionName, reason, info, file, line) {};
};

#endif // _CONTEXTEXCEPTION_HPP

