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
 * $Id: XQillaPlatformUtils.hpp,v 1.8 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _XQILLAPLATFORMUTILS_HPP
#define _XQILLAPLATFORMUTILS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xercesc/util/XercesDefs.hpp>
XERCES_CPP_NAMESPACE_BEGIN
class MemoryManager;
XERCES_CPP_NAMESPACE_END

/**
 * Static utility methods for initializing and terminating the XQilla
 * and Xerces environments.
 */
class XQILLA_API XQillaPlatformUtils
{
public:

  /**
   * Perform XQilla initialization
   *
   * Initialization should be called first in any user code.
   * 
   * Initializes Xerces and adds its own DOMImplementation to the registry
   */
  static void initialize(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr = 0);

  /**
   * Perform XQilla termination
   *
   * Terminates Xerces
   */
  static void terminate();

  /**
   * Enable/disable extended precision in numeric items
   *
   * When disabled, xs:decimal values will hold only 18 digits, xs:float 7 digits and xs:double 15 digits 
   * as mandated 
   */
  static void enableExtendedPrecision(bool bEnable);

};
#endif

