
/* 
 *  M_APM  -  mapmhasn.c
 *
 *  Copyright (C) 2000 - 2003   Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *      $Id: mapmhasn.c,v 1.1 2004/07/21 09:23:01 jpcs Exp $
 *
 *      This file contains the Inverse Hyperbolic SIN, COS, & TAN functions.
 *
 *      $Log: mapmhasn.c,v $
 *      Revision 1.1  2004/07/21 09:23:01  jpcs
 *      Initial check in of the Parthenon backup of Pathan.
 *
 *      Revision 1.2  2003/05/01 12:46:55  alby
 *      - Updated MAPM to version 4.6.1
 *      - The MAPM class is now thread-safe
 *
 *      Revision 1.3  2003/03/31 21:53:21  mike
 *      call generic error handling function
 *
 *      Revision 1.2  2002/11/03 21:25:03  mike
 *      Updated function parameters to use the modern style
 *
 *      Revision 1.1  2000/04/03 18:16:29  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

/****************************************************************************/
/*
 *      arcsinh(x) == log [ x + sqrt(x^2 + 1) ]
 */
void	m_apm_arcsinh(M_APM rr, int places, M_APM aa)
{
M_APM	tmp1, tmp2;
int     local_precision;

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();

local_precision = places + 4;

m_apm_multiply(tmp1, aa, aa);
m_apm_add(tmp2, tmp1, MM_One);
m_apm_sqrt(tmp1, local_precision, tmp2);
m_apm_add(tmp2, aa, tmp1);
m_apm_log(rr, places, tmp2);

M_restore_stack(2);
}
/****************************************************************************/
/*
 *      arccosh(x) == log [ x + sqrt(x^2 - 1) ]
 *
 *      x >= 1.0
 */
void	m_apm_arccosh(M_APM rr, int places, M_APM aa)
{
M_APM	tmp1, tmp2;
int     ii, local_precision;

ii = m_apm_compare(aa, MM_One);

if (ii == -1)       /* x < 1 */
  {
   M_apm_log_error_msg(M_APM_RETURN, 
                       "Warning! ... \'m_apm_arccosh\', Argument < 1");
   M_set_to_zero(rr);
   return;
  }

tmp1 = M_get_stack_var();
tmp2 = M_get_stack_var();

local_precision = places + 4;

m_apm_multiply(tmp1, aa, aa);
m_apm_subtract(tmp2, tmp1, MM_One);
m_apm_sqrt(tmp1, local_precision, tmp2);
m_apm_add(tmp2, aa, tmp1);
m_apm_log(rr, places, tmp2);

M_restore_stack(2);
}
/****************************************************************************/
/*
 *      arctanh(x) == 0.5 * log [ (1 + x) / (1 - x) ]
 *
 *      |x| < 1.0
 */
void	m_apm_arctanh(M_APM rr, int places, M_APM aa)
{
M_APM	tmp1, tmp2, tmp3;
int     ii, local_precision;

tmp1 = M_get_stack_var();

m_apm_absolute_value(tmp1, aa);

ii = m_apm_compare(tmp1, MM_One);

if (ii >= 0)       /* |x| >= 1.0 */
  {
   M_apm_log_error_msg(M_APM_RETURN,
                       "Warning! ... \'m_apm_arctanh\', |Argument| >= 1");
   M_set_to_zero(rr);
   M_restore_stack(1);
   return;
  }

tmp2 = M_get_stack_var();
tmp3 = M_get_stack_var();

local_precision = places + 6;

m_apm_add(tmp1, MM_One, aa);
m_apm_subtract(tmp2, MM_One, aa);
m_apm_divide(tmp3, local_precision, tmp1, tmp2);
m_apm_log(tmp2, local_precision, tmp3);
m_apm_multiply(tmp1, tmp2, MM_0_5);
m_apm_round(rr, places, tmp1);

M_restore_stack(3);
}
/****************************************************************************/
