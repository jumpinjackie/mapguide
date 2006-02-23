// JAWS.h,v 1.2 2003/08/06 03:29:13 dhinton Exp

#if (JAWS_NTRACE == 1)
# define JAWS_TRACE(X)
#else
# define JAWS_TRACE(X) ACE_Trace ____ (ACE_TEXT (X), \
                                       __LINE__, \
                                       ACE_TEXT (__FILE__))
#include "ace/Trace.h"
#endif /* JAWS_NTRACE */

#ifndef JAWS_JAWS_H
#define JAWS_JAWS_H

#endif /* JAWS_JAWS_H */
