//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQEngine.hpp,v 1.6 2005/11/21 23:16:52 gmf Exp $
//

// The following ifdef block is the standard way of creating macros which
// make exporting from a DLL simpler. All files within this DLL are
// compiled with the XQENGINE_EXPORTS symbol defined on the command line.
// this symbol should not be defined on any project that uses this DLL.
// This way any other project whose source files include this file see
// XQENGINE_API functions as being imported from a DLL, wheras this DLL
// sees symbols defined with this macro as being exported.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
  #ifdef XQENGINE_EXPORTS
    #define XQENGINE_API __declspec(dllexport)
  #else
    #define XQENGINE_API __declspec(dllimport)
  #endif
  #pragma warning(disable:4786)
#else
  #define XQENGINE_API 
#endif

