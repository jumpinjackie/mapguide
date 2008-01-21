
/* 
 *  M_APM  -  mapm_fam.c
 *
 *  Copyright (C) 2001 - 2003  Michael C. Ring
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

/****************************************************************************/
void	m_apm_free_all_mem()
{
M_free_all_add();    /* call each module which has statically declared data */
M_free_all_div();
M_free_all_exp();

#ifndef NO_FFT_MULTIPLY
M_free_all_fft();
#endif

M_free_all_pow();
M_free_all_rnd();
M_free_all_set();
M_free_all_cnst();
M_free_all_fmul();
M_free_all_stck();
M_free_all_util();
}
/****************************************************************************/
void	m_apm_trim_mem_usage()
{
m_apm_free_all_mem();
m_apm_free(m_apm_init());
}
/****************************************************************************/
