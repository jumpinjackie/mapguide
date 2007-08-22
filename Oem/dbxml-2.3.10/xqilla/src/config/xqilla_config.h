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
 * $Id: xqilla_config.h,v 1.5 2006/11/01 16:37:18 jpcs Exp $
 */

#if defined(WIN32) && !defined(__CYGWIN__)
#include "xqilla_config_win32.h"
#else
#include "xqilla_config_unix.h"
#endif
