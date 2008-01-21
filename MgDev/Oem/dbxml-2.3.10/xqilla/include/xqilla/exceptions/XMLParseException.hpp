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
 * $Id: XMLParseException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  XMLParseException - exception class for parsing XML

*/

#ifndef _XMLPARSEEXCEPTION_HPP
#define _XMLPARSEEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** exception class for parsing XML */
class XQILLA_API XMLParseException : public XQException
{
public:

  XMLParseException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("XMLParseException"), functionName, reason, info, file, line) {};
};

#endif // _XMLPARSEEXCEPTION_HPP

