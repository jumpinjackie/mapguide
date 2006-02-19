/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000-2005
 *	Sleepycat Software.  All rights reserved.
 *
 * $Id: common_utils.h,v 1.1 2005/07/27 17:56:01 gmf Exp $
 */
#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
	
void __db_util_siginit();
int __db_util_interrupted();
void __db_util_sigresend();
#ifdef __cplusplus
}
#endif

namespace DbXml {
        // class to wrap above interfaces
	class  SigBlock {
	public:
		SigBlock() { __db_util_siginit(); }
		~SigBlock() { __db_util_sigresend(); }
		bool isInterrupted() const {
			return (__db_util_interrupted() ? true : false);
		}
	};
}
#endif
