
/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _NUMUTILS_HPP
#define _NUMUTILS_HPP

#include <pathan/Pathan.hpp>

#include <pathan/internal/m_apm.h>

class XPath2MemoryManager;

/** class with static methods for dealing with floating point numbers */
class PATHAN_EXPORT NumUtils
{
public:
  /** return MAPM to int */
  static int MAPMtoInt(MAPM m);

  /** return MAPM to double */
  static double MAPMtoDouble(MAPM m);
};

#endif // _NUMUTILS_HPP

