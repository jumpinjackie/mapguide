// -*- C++ -*-
// OS_NS_sys_shm.cpp,v 1.2 2003/11/01 11:15:15 dhinton Exp

#include "ace/OS_NS_sys_shm.h"

ACE_RCSID(ace, OS_NS_sys_shm, "OS_NS_sys_shm.cpp,v 1.2 2003/11/01 11:15:15 dhinton Exp")

#if !defined (ACE_HAS_INLINED_OSCALLS)
# include "ace/OS_NS_sys_shm.inl"
#endif /* ACE_HAS_INLINED_OS_CALLS */

// these don't appear to be used anywhere. dhinton
#if defined(INTEGRITY) && defined(ACE_HAS_SHM_OPEN)
char* shm_area_name = "ACE_Area";
char* shm_area_password = "******";
#endif

