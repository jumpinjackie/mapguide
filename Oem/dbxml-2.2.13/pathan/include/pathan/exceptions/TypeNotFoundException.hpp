/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _TYPENOTFOUNDEXCEPTION_HPP
#define _TYPENOTFOUNDEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>

class PATHAN_EXPORT TypeNotFoundException : public DSLException
{
public:
  
  TypeNotFoundException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("TypeNotFoundException"), functionName, reason, file, line) {};
};

#endif // _TYPENOTFOUNDEXCEPTION_HPP
