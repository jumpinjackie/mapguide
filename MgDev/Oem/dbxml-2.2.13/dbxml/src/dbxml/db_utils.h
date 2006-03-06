/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002-2005
 *	Sleepycat Software.  All rights reserved.
 *
 * $Id: db_utils.h,v 1.8 2005/04/20 18:31:29 bostic Exp $
 */

#ifndef __DB_UTILS_HPP
#define	__DB_UTILS_HPP

/*
 * Handy macro from db_swap.h:
 *	M_32_SWAP	swap a memory location
 */
#define	M_32_SWAP(a) {							\
	u_int32_t _tmp;							\
	_tmp = a;							\
	((u_int8_t *)&a)[0] = ((u_int8_t *)&_tmp)[3];			\
	((u_int8_t *)&a)[1] = ((u_int8_t *)&_tmp)[2];			\
	((u_int8_t *)&a)[2] = ((u_int8_t *)&_tmp)[1];			\
	((u_int8_t *)&a)[3] = ((u_int8_t *)&_tmp)[0];			\
}

#define M_16_SWAP(a) {                                                  \
        u_int16_t _tmp;                                                 \
        _tmp = (u_int16_t)a;                                            \
        ((u_int8_t *)&a)[0] = ((u_int8_t *)&_tmp)[1];                   \
        ((u_int8_t *)&a)[1] = ((u_int8_t *)&_tmp)[0];                   \
}

/* Flag helpers */
#define	LF_SET(f)		((flags) |= (f))
#define	LF_CLR(f)		((flags) &= ~(f))
#define	LF_ISSET(f)	((flags) & (f))

#endif
