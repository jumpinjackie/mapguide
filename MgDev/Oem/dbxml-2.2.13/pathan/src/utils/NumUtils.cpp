
/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */
#include "../config/pathan_config.h"
#include "NumUtils.hpp"
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
