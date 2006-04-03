/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  NamespaceLookupException - exception class for DSLPath parser

*/

#ifndef _NAMESPACELOOKUPEXCPETION_HPP
#define _NAMESPACELOOKUPEXCPETION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>

/** exception class for DSLPath parser */

class PATHAN_EXPORT NamespaceLookupException : public DSLException
{
public:

  NamespaceLookupException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("NamespaceLookupException"), functionName, reason, file, line) {};
};

#endif // _XPATHPARSEEXCEPTION_HPP

