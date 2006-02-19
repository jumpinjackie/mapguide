/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  XPath2ErrorException - handles cases in the specification where the `error value' is returned

*/

#ifndef _MISCEXCEPTION_HPP
#define _MISCEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** exception class for miscellaneous errors */
class PATHAN_EXPORT MiscException : public DSLException
{
public:
  
  MiscException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("MiscException"), functionName, reason, file, line) {};
};

#endif // _MISCEXCEPTION_HPP
