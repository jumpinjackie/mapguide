/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  FunctionException - exception class for functions
*/

#ifndef _FUNCTIONEXCEPTION_HPP
#define _FUNCTIONEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** exception class for tests. Inherit from this if you need to throw an exception
    in the test suite.*/
class PATHAN_EXPORT FunctionException : public DSLException
{
public:

  FunctionException(const XMLCh* const functionName, const XMLCh* const reason, const char *file, int line)
    : DSLException(X("FunctionException"), functionName, reason, file, line) {};
};

#endif // _FUNCTIONEXCEPTION_HPP
