/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/Pathan.hpp>
#include <pathan/PathanException.hpp>

bool PathanException::_debug = false;

PathanException::PathanException(const PathanException &other): XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException(other.code, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(other.msg)) 
{
  // nothing to do
}


PathanException::~PathanException() {
    msg = 0;
}

/*static*/ void PathanException::setDebug(bool flag)
{
    _debug = flag;
}

/*static*/ bool PathanException::getDebug(void)
{
    return _debug;
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::ExceptionCode PathanException::getCode(void) const 
{
  return code;
}//getCode

const XMLCh* PathanException::getString(void) const 
{
  return msg;
}//getString
