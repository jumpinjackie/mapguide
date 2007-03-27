/*
 * Copyright (c) 2001-2004, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHAN_HPP
#define __PATHAN_HPP

/*
 * We are linking against the library,
 * so if this is Windows we need dllimport
 */
#ifndef PATHAN_EXPORT
#if defined(WIN32) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable: 4251 4786 4101 4290)
#include <windows.h>
#define PATHAN_EXPORT __declspec(dllimport)
#else
#define PATHAN_EXPORT
#endif
#endif

#endif
