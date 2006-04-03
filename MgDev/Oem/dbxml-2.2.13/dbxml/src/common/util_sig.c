/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000-2005
 *	Sleepycat Software.  All rights reserved.
 *
 * $Id: util_sig.c,v 1.2 2005/07/29 19:01:29 gmf Exp $
 */

/*
 * This file is virtually identical to db/common/util_sig.c
 * It is intended to be used by common BDB XML utilities to
 * handle signals as necessary.
 * It's intended usage is to hold off important signals across
 * critical sections. The application can then check to see if
 * an interrupt happened in locations of its own choosing.
 *
 * Application usage pattern:
 *   __db_util_siginit() // initialize
 *   while (true) {
 *     do stuff
 *     if (__db_util_interrupted())
 *       break;
 *   }
 *   app-specific cleanup
 *   __db_util_sigresend();
 *   exit
 */

#include <sys/types.h>
#include <signal.h>

static int interrupt;
static void onint(int);

/*
 * onint --
 *	Interrupt signal handler.
 */
static void
onint(signo)
	int signo;
{
	if ((interrupt = signo) == 0)
		interrupt = SIGINT;
}

/*
 * __db_util_siginit --
 *
 * PUBLIC: void __db_util_siginit __P((void));
 */
void
__db_util_siginit()
{
	/*
	 * Initialize the set of signals for which we want to clean up.
	 * Generally, we try not to leave the shared regions locked if
	 * we can.
	 */
#ifdef SIGHUP
	(void)signal(SIGHUP, onint);
#endif
	(void)signal(SIGINT, onint);
#ifdef SIGPIPE
	(void)signal(SIGPIPE, onint);
#endif
	(void)signal(SIGTERM, onint);
}

/*
 * __db_util_interrupted --
 *	Return if interrupted.
 *
 * PUBLIC: int __db_util_interrupted __P((void));
 */
int
__db_util_interrupted()
{
	return (interrupt != 0);
}

/*
 * __db_util_sigresend --
 *
 * PUBLIC: void __db_util_sigresend __P((void));
 */
void
__db_util_sigresend()
{
	/* Resend any caught signal. */
	if (interrupt != 0) {
		(void)signal(interrupt, SIG_DFL);
		(void)raise(interrupt);
		/* NOTREACHED */
	}
}
