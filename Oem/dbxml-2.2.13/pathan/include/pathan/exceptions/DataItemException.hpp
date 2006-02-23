/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  DataItemDSLException - exception class for DataItems

*/

#ifndef _DATAITEMEXCEPTION_HPP
#define _DATAITEMEXCEPTION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

/** exception class for data items */

class PATHAN_EXPORT DataItemException : public DSLException
{
public:

  DataItemException(const XMLCh *functionName, const XMLCh *reason, const char *file, int line)
    : DSLException(X("DataItemException"), functionName, reason, file, line) {};
};

#endif // _DATAITEMEXCEPTION_HPP
