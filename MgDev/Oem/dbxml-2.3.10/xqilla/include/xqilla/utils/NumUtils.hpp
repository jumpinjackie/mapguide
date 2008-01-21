/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: NumUtils.hpp,v 1.7 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _NUMUTILS_HPP
#define _NUMUTILS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/mapm/m_apm.h>

class XPath2MemoryManager;

/** class with static methods for dealing with floating point numbers */
class XQILLA_API NumUtils
{
public:
  /** return MAPM to int */
  static int MAPMtoInt(MAPM m);

  /** return MAPM to double */
  static double MAPMtoDouble(MAPM m);
};

#endif // _NUMUTILS_HPP

