/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  XMLParseException - exception class for parsing XML

*/

#ifndef _XMLPARSEEXCEPTION_HPP
#define _XMLPARSEEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** exception class for parsing XML */
class PATHAN_EXPORT XMLParseException : public DSLException
{
public:

  XMLParseException(const XMLCh* const functionName, const XMLCh* const reason, const char *file, int line)
    : DSLException(X("XMLParseException"), functionName, reason, file, line) {};
};

#endif // _XMLPARSEEXCEPTION_HPP

