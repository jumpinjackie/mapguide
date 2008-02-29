/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    pre.h
 *
 *  pre.h,v 4.3 2001/07/09 09:19:04 doccvs Exp
 *
 *  @author Christopher Kohlhoff <chris@kohlhoff.com>
 *
 *  This file save the original alignment rules and changes the alignment
 *  boundary to ACE's default.
 *
 *
 */
//=============================================================================


// No header guard
#if defined (_MSC_VER)
# pragma warning (disable:4103)
# pragma pack (push, 8)
// Disable deprecation and non-conforming warning introduced by VC8
# define _CRT_SECURE_NO_DEPRECATE 1
# define _CRT_NON_CONFORMING_SWPRINTFS 1
# pragma warning (disable:4996)
#elif defined (__BORLANDC__)
# pragma option push -a8 -b -Ve- -Vx- -w-rvl -w-rch -w-ccc -w-obs -w-aus -w-pia -w-inl -w-sig
# pragma nopushoptwarn
# pragma nopackwarning
#endif

