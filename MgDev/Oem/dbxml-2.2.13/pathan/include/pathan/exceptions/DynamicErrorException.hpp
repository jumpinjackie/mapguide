/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DYNAMICERROREXCEPTION_HPP
#define _DYNAMICERROREXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** 
 * This means that an error was found while evaluating the expression
 */

class PATHAN_EXPORT DynamicErrorException : public DSLException
{
public:
  DynamicErrorException(const XMLCh* const functionName, const XMLCh* const reason, const char *file, int line)
    : DSLException(X("DynamicErrorException"), functionName, reason, file, line) {};
};

#endif // _DYNAMICERROREXCEPTION_HPP
