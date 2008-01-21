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
 * $Id: NamespaceLookupException.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  NamespaceLookupException - exception class for DSLPath parser

*/

#ifndef _NAMESPACELOOKUPEXCPETION_HPP
#define _NAMESPACELOOKUPEXCPETION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>

/** exception class for DSLPath parser */

class XQILLA_API NamespaceLookupException : public XQException
{
public:

  NamespaceLookupException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("NamespaceLookupException"), functionName, reason, info, file, line) {};
};

#endif // _XPATHPARSEEXCEPTION_HPP

