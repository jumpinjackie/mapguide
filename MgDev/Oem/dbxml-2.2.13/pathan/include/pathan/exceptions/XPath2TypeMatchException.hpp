/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  XPath2TypeCastException - exception class for casting errors

*/

#ifndef _XPATH2TYPEMATCHEXCEPTION_HPP
#define _XPATH2TYPEMATCHEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>

/** Exception to be thrown if an attempt to match a non-existent type is made */
class PATHAN_EXPORT XPath2TypeMatchException : public DSLException
{
public:

  XPath2TypeMatchException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("XPath2TypeCastException"), functionName, reason, file, line) {};
};

#endif // _XPATH2TYPEMATCHEXCEPTION_HPP
