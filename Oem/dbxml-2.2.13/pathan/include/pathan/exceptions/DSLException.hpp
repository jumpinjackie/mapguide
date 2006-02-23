/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Exception - super class for all exceptions

*/

#ifndef _DSLEXCEPTION_HPP
#define _DSLEXCEPTION_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include "../internal/DOMutils/XStr.hpp"

#define DSLthrow(type, function, reason) throw type(function, reason, __FILE__, __LINE__)

/** The exception super class. Please inherit from this class, and sub class as
    appropriate. This can be used as a method of catching groups of exceptions. */

class PATHAN_EXPORT DSLException
{
public:

  DSLException(const XMLCh* const type, const XMLCh* const functionName, const XMLCh* const reason, const char *file, int line);
	DSLException(const DSLException &other);
  virtual ~DSLException();

  void printDebug(const XMLCh* const context) const;
  const XMLCh* getError(void) const;

  static void setDebug(bool flag);

private:
  
  XMLCh* _type;
  XMLCh* _function;
  XMLCh* _error;
  const char *_file;
  int _line;

  static bool _debug;
};

#endif // _DSLEXCEPTION_HPP
