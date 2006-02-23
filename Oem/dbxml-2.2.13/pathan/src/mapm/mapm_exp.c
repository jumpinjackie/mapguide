
/* 
 *  M_APM  -  mapm_exp.c
 *
 *  Copyright (C) 1999 - 2003   Michael C. Ring
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
 *      $Id: mapm_exp.c,v 1.1 2004/07/21 09:23:01 jpcs Exp $
 *
 *      This file contains the EXP function.
 *
 *      $Log: mapm_exp.c,v $
 *      Revision 1.1  2004/07/21 09:23:01  jpcs
 *      Initial check in of the Parthenon backup of Pathan.
 *
 *      Revision 1.2  2003/05/01 12:46:55  alby
 *      - Updated MAPM to version 4.6.1
 *      - The MAPM class is now thread-safe
 *
 *      Revision 1.27  2003/03/30 21:16:37  mike
 *      use global version of log(2) instead of local copy
 *
 *      Revision 1.26  2002/11/03 22:30:18  mike
 *      Updated function parameters to use the modern style
 *
 *      Revision 1.25  2001/08/06 22:07:00  mike
 *      round the 'nn' calculation to the nearest int
 *
 *      Revision 1.24  2001/08/05 21:58:59  mike
 *      make 1 / log(2) constant shorter
 *
 *      Revision 1.23  2001/07/16 19:10:23  mike
 *      add function M_free_all_exp
 *
 *      Revision 1.22  2001/07/10 21:55:36  mike
 *      optimize raw_exp by using fewer digits as the
 *      subsequent terms get smaller
 *
 *      Revision 1.21  2001/02/06 21:20:31  mike
 *      optimize 'nn' calculation (for the future)
 *
 *      Revision 1.20  2001/02/05 21:55:12  mike
 *      minor optimization, use a multiply instead
 *      of a divide
 *
 *      Revision 1.19  2000/08/22 21:34:41  mike
 *      increase local precision
 *
 *      Revision 1.18  2000/05/18 22:05:22  mike
 *      move _pow to a separate file
 *
 *      Revision 1.17  2000/05/04 23:21:01  mike
 *      use global var 256R
 *
 *      Revision 1.16  2000/03/30 21:33:30  mike
 *      change termination of raw_exp to use ints, not MAPM numbers
 *
 *      Revision 1.15  2000/02/05 22:59:46  mike
 *      adjust decimal places on calculation
 *
 *      Revision 1.14  2000/02/04 20:45:21  mike
 *      re-compute log(2) on the fly if we need a more precise value
 *
 *      Revision 1.13  2000/02/04 19:35:14  mike
 *      use just an approx log(2) for the integer divide
 *
 *      Revision 1.12  2000/02/04 16:47:32  mike
 *      use the real algorithm for EXP
 *
 *      Revision 1.11  1999/09/18 01:27:40  mike
 *      if X is 0 on the pow function, return 0 right away
 *
 *      Revision 1.10  1999/06/19 20:54:07  mike
 *      changed local static MAPM to stack variables
 *
 *      Revision 1.9  1999/06/01 22:37:44  mike
 *      adjust decimal places passed to raw function
 *
 *      Revision 1.8  1999/06/01 01:44:03  mike
 *      change threshold from 1000 to 100 for 65536 divisor
 *
 *      Revision 1.7  1999/06/01 01:03:31  mike
 *      vary 'q' instead of checking input against +/- 10 and +/- 40
 *
 *      Revision 1.6  1999/05/15 01:54:27  mike
 *      add check for number of decimal places
 *
 *      Revision 1.5  1999/05/15 01:09:49  mike
 *      minor tweak to POW decimal places
 *
 *      Revision 1.4  1999/05/13 00:14:00  mike
 *      added more comments
 *
 *      Revision 1.3  1999/05/12 23:39:05  mike
 *      change #places passed to sub functions
 *
 *      Revision 1.2  1999/05/10 21:35:13  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"

static  M_APM  MM_exp_log2R;
static  M_APM  MM_exp_1024R;
static	int    MM_firsttime1 = TRUE;

/****************************************************************************/
void	M_free_all_exp()
{
if (MM_firsttime1 == FALSE)
  {
   m_apm_free(MM_exp_log2R);
   m_apm_free(MM_exp_1024R);

   MM_firsttime1 = TRUE;
  }
}
/****************************************************************************/
void	m_apm_exp(M_APM r, int places, M_APM x)
{
M_APM   tmp7, tmp8, tmp9;
char    outstr[64];
int	dplaces, nn, ii;

if (MM_firsttime1)
  {
   MM_firsttime1 = FALSE;

   MM_exp_log2R = m_apm_init();
   MM_exp_1024R = m_apm_init();

   m_apm_set_string(MM_exp_log2R, "1.44269504089");   /* ~ 1 / log(2) */
   m_apm_set_string(MM_exp_1024R, "9.765625E-4");     /*   1 / 1024   */
  }

tmp7 = M_get_stack_var();
tmp8 = M_get_stack_var();
tmp9 = M_get_stack_var();

/*
    From David H. Bailey's MPFUN Fortran package :

    exp (t) =  (1 + r + r^2 / 2! + r^3 / 3! + r^4 / 4! ...) ^ q * 2 ^ n

    where q = 256, r = t' / q, t' = t - n Log(2) and where n is chosen so
    that -0.5 Log(2) < t' <= 0.5 Log(2).  Reducing t mod Log(2) and
    dividing by 256 insures that -0.001 < r <= 0.001, which accelerates
    convergence in the above series.

    we won't let n == 1 or -1. if n computes out to be 1, we will set
    n = 0. this is so we can still calculate log(2). if we let n = 1, 
    then we need log(2) in order to compute log(2), which is no good ...

    if the new 'n' == 0, we will then use q = 1024 instead of 256 so
    the series still converges as fast.

    since the value of log(2) is critical to this function, log, and
    log10, we will keep our own local copy of it. if we need more
    precision in our log(2), we will re-compute it on the fly to get
    the desired precision.
*/

/* find 'n' and convert it to a normal C int            */
/* we just need an approx 1/log(2) for this calculation */

m_apm_multiply(tmp8, x, MM_exp_log2R);

/* round to the nearest int */

if (x->m_apm_sign >= 0)
  m_apm_add(tmp9, tmp8, MM_0_5);
else
  m_apm_subtract(tmp9, tmp8, MM_0_5);

m_apm_to_integer_string(outstr, tmp9);
nn = atoi(outstr);

if (abs(nn) == 1)
  nn = 0;

if (nn == 0)
  {
   ii = 10;
   dplaces = places + 8;
   m_apm_multiply(tmp9, x, MM_exp_1024R);
  }
else
  {
   ii = 8;
   dplaces = places + 6;

   /* check to make sure our log(2) is accurate enough */

   M_check_log_places(dplaces);

   m_apm_set_long(tmp7, (long)nn);
   m_apm_multiply(tmp8, tmp7, MM_lc_log2);
   m_apm_subtract(tmp7, x, tmp8);

   /*
    *     guarantee that |tmp7| < 1.
    *     the normal case will break out immediately.
    *	  this is just here to plan for the future.
    *     when sizeof(int) gets real big, it may
    *     be possible for the 'nn' calculation from
    *     above to not be optimized due to the inexact
    *     constant used for 'MM_exp_log2R'.
    */

   while (TRUE)
     {
      if (tmp7->m_apm_exponent <= 0)
        break;
     
      if (tmp7->m_apm_sign == 0)
        break;
     
      if (tmp7->m_apm_sign == 1)
        {
	 nn++;
         m_apm_subtract(tmp8, tmp7, MM_lc_log2);
	 m_apm_copy(tmp7, tmp8);
	}
      else
        {
	 nn--;
         m_apm_add(tmp8, tmp7, MM_lc_log2);
	 m_apm_copy(tmp7, tmp8);
	}
     }

   m_apm_multiply(tmp9, tmp7, MM_5x_256R);
  }

/* perform the series expansion ... */

M_raw_exp(tmp8, dplaces, tmp9);

/*
 *   raise result to the 256 OR 1024 power (q = 256 OR 1024) 
 *
 *   note : x ^ 256    == (((x ^ 2) ^ 2) ^ 2) ... 8  times
 *        : x ^ 1024   == (((x ^ 2) ^ 2) ^ 2) ... 10 times
 */

while (--ii >= 0)   
  {
   m_apm_multiply(tmp9, tmp8, tmp8);
   m_apm_round(tmp8, dplaces, tmp9);
  }

/* now compute 2 ^ N */

m_apm_integer_pow(tmp7, dplaces, MM_Two, nn);

m_apm_multiply(tmp9, tmp7, tmp8);
m_apm_round(r, places, tmp9);
M_restore_stack(3);                    /* restore the 3 locals we used here */
}
/****************************************************************************/
/*
	calculate the exponential function using the following series :

                              x^2     x^3     x^4     x^5
	exp(x) == 1  +  x  +  ---  +  ---  +  ---  +  ---  ...
                               2!      3!      4!      5!

*/
void	M_raw_exp(M_APM rr, int places, M_APM xx)
{
M_APM   tmp0, digit, term;
int	tolerance,  local_precision, prev_exp;
long    m1;

tmp0  = M_get_stack_var();
term  = M_get_stack_var();
digit = M_get_stack_var();

local_precision = places + 8;
tolerance       = -(places + 4);
prev_exp        = 0;

m_apm_add(rr, MM_One, xx);
m_apm_copy(term, xx);

m1 = 2L;

while (TRUE)
  {
   m_apm_set_long(digit, m1);
   m_apm_multiply(tmp0, term, xx);
   m_apm_divide(term, local_precision, tmp0, digit);
   m_apm_add(tmp0, rr, term);
   m_apm_copy(rr, tmp0);

   if ((term->m_apm_exponent < tolerance) || (term->m_apm_sign == 0))
     break;

   if (m1 != 2L)
     {
      local_precision = local_precision + term->m_apm_exponent - prev_exp;

      if (local_precision < 20)
        local_precision = 20;
     }

   prev_exp = term->m_apm_exponent;
   m1++;
  }

M_restore_stack(3);                    /* restore the 3 locals we used here */
}
/****************************************************************************/
