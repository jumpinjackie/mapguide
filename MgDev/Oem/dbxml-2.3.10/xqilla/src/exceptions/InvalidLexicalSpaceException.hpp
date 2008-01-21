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
 * $Id: InvalidLexicalSpaceException.hpp,v 1.9 2006/11/01 16:37:19 jpcs Exp $
 */

/*

  Invalid Lexical Space Exception: thrown when a value is not in the appropriate lexical space

*/

#ifndef _INVALIDLEXICALSPACEEXCEPTION_HPP
#define _INVALIDLEXICALSPACEEXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

class XQILLA_API InvalidLexicalSpaceException : public XQException
{
public:
  
  InvalidLexicalSpaceException(const XMLCh *functionName, const XMLCh *reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("InvalidLexicalSpaceException"), functionName, reason, info, file, line) {};
};

#endif // _INVALIDLEXICALSPACEEXCEPTION_HPP
