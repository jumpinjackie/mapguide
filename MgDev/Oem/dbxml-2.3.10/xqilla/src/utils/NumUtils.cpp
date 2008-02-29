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
 * $Id: NumUtils.cpp,v 1.7 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/NumUtils.hpp>
#include <stdlib.h>

int NumUtils::MAPMtoInt(MAPM m) {
  char tmpout[1024];
  m.toIntegerString(tmpout);
  int result = atoi(tmpout);
  return result; 
}   
    
double NumUtils::MAPMtoDouble(MAPM m) {
  char tmpdouble[1024];
  m.toFixPtString(tmpdouble,1000);
  double result = atof(tmpdouble);
  return result;
}   
