/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _TYPEERROREXCEPTION_HPP
#define _TYPEERROREXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** 
 * This means that an error was found while manipulating typed data
 */

class PATHAN_EXPORT TypeErrorException : public DSLException
{
public:
  TypeErrorException(const XMLCh* const functionName, const XMLCh* const reason, const char *file, int line)
    : DSLException(X("TypeErrorException"), functionName, reason, file, line) {};
};

#endif // _TYPEERROREXCEPTION_HPP
