
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
