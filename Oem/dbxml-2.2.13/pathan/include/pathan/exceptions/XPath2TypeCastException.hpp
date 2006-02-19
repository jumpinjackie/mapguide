/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  XPath2TypeCastException - exception class for casting errors

*/

#ifndef _XPATH2TYPECASTEXCEPTION_HPP
#define _XPATH2TYPECASTEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** exception class for data items */
class PATHAN_EXPORT XPath2TypeCastException : public DSLException
{
public:

  XPath2TypeCastException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("XPath2TypeCastException"), functionName, reason, file, line) {};
};

#endif // _XPATH2TYPECASTEXCEPTION_HPP
