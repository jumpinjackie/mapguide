
/* 
 *  M_APM  -  mapmgues.c
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
 *      $Id: mapmgues.c,v 1.1 2004/07/21 09:23:01 jpcs Exp $
 *
 *      This file contains the functions that generate the initial 
 *	'guesses' for the sqrt, cbrt, log, arcsin, and arccos functions.
 *
 *      $Log: mapmgues.c,v $
 *      Revision 1.1  2004/07/21 09:23:01  jpcs
 *      Initial check in of the Parthenon backup of Pathan.
 *
 *      Revision 1.2  2003/05/01 12:46:55  alby
 *      - Updated MAPM to version 4.6.1
 *      - The MAPM class is now thread-safe
 *
 *      Revision 1.16  2003/04/11 14:18:13  mike
 *      add comments
 *
 *      Revision 1.15  2003/04/10 22:28:35  mike
 *      .
 *
 *      Revision 1.14  2003/04/09 21:33:19  mike
 *      induce same error for asin and acos
 *
 *      Revision 1.13  2003/04/09 20:11:38  mike
 *      induce error of 10 ^ -5 in log guess for known starting
 *      point in the iterative algorithm
 *
 *      Revision 1.12  2003/03/27 19:32:59  mike
 *      simplify log guess since caller guarantee's limited input magnitude
 *
 *      Revision 1.11  2002/11/03 21:45:53  mike
 *      Updated function parameters to use the modern style
 *
 *      Revision 1.10  2001/03/20 22:08:27  mike
 *      delete unneeded logic in asin guess
 *
 *      Revision 1.9  2000/09/26 17:05:11  mike
 *      guess 1/sqrt instead of sqrt due to new sqrt algorithm
 *
 *      Revision 1.8  2000/04/10 21:13:13  mike
 *      minor tweaks to log_guess
 *
 *      Revision 1.7  2000/04/03 17:25:45  mike
 *      added function to estimate the cube root (cbrt)
 *
 *      Revision 1.6  1999/07/18 01:57:35  mike
 *      adjust arc-sin guess for small exponents
 *
 *      Revision 1.5  1999/07/09 22:32:50  mike
 *      optimize some functions
 *
 *      Revision 1.4  1999/05/12 21:22:27  mike
 *      add more comments
 *
 *      Revision 1.3  1999/05/12 21:00:51  mike
 *      added new sqrt guess function
 *
 *      Revision 1.2  1999/05/11 02:10:12  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:31  mike
 *      Initial revision
 */

#include "m_apm_lc.h"
#include <math.h>

/****************************************************************************/
void	M_get_sqrt_guess(M_APM r, M_APM a)
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);                     /* sqrt algorithm actually finds 1/sqrt */
m_apm_set_double(r, (1.0 / sqrt(dd)));
}
/****************************************************************************/
void	M_get_cbrt_guess(M_APM r, M_APM a)
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
dd = log(dd) / 3.0;
m_apm_set_double(r, exp(dd));
}
/****************************************************************************/
/*
 *	for log, asin, and acos we induce an error of 10 ^ -5.
 *	this enables the iterative routine to be more efficient
 *	by knowing exactly how accurate the initial guess is.
 *
 *	but the real reason ... this also prevents some corner
 *	conditions where the iterative functions may terminate too soon.
 */
void	M_get_log_guess(M_APM r, M_APM a)
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
m_apm_set_double(r, (1.00001 * log(dd)));        /* induce error of 10 ^ -5 */
}
/****************************************************************************/
/*
 *	the implementation of the asin & acos functions 
 *	guarantee that 'a' is always < 0.85, so it is 
 *	safe to multiply by a number > 1
 */
void	M_get_asin_guess(M_APM r, M_APM a)
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
m_apm_set_double(r, (1.00001 * asin(dd)));       /* induce error of 10 ^ -5 */
}
/****************************************************************************/
void	M_get_acos_guess(M_APM r, M_APM a)
{
char	buf[48];
double  dd;

m_apm_to_string(buf, 15, a);
dd = atof(buf);
m_apm_set_double(r, (1.00001 * acos(dd)));       /* induce error of 10 ^ -5 */
}
/****************************************************************************/
/*
	convert a C 'double' into an M_APM value. 
*/
void	m_apm_set_double(M_APM atmp, double dd)
{
char	*cp, *p, *ps, buf[64];

if (dd == 0.0)                     /* special case for 0 exactly */
   m_apm_set_string(atmp, "0");
else
  {
   sprintf(buf,"%.14E",dd);
   
   cp = strstr(buf, "E");
   if (atoi(cp + sizeof(char)) == 0)
     *cp = '\0';
   
   p = cp;
   
   while (TRUE)
     {
      p--;
      if (*p == '0' || *p == '.')
        *p = ' ';
      else
        break;
     }
   
   ps = buf;
   p  = buf;
   
   while (TRUE)
     {
      if ((*p = *ps) == '\0')
        break;
   
      if (*ps++ != ' ')
        p++;
     }

   m_apm_set_string(atmp, buf);
  }
}
/****************************************************************************/
