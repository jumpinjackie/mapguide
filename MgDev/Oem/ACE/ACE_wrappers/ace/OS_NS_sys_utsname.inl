// -*- C++ -*-
//
// OS_NS_sys_utsname.inl,v 1.3 2003/11/04 17:17:32 ossama Exp

#include "ace/os_include/os_errno.h"


#if !defined (ACE_WIN32) && !defined (VXWORKS) && !defined (CHORUS) && !defined (ACE_PSOS)
// Don't inline on those platforms because this function contains
// string literals, and some compilers, e.g., g++, don't handle those
// efficiently in unused inline functions.
ACE_INLINE int
ACE_OS::uname (ACE_utsname *name)
{
#if defined (INTEGRITY)
  if(!name) {
      errno = EFAULT;
      return -1;
  }
  strcpy(name->sysname,"INTEGRITY");
  int status = gethostname(name->nodename,_SYS_NMLN);
  strcpy(name->release,"4.0");
  strcpy(name->version,"4.0.9");
  strcpy(name->machine,"a standard name");
  return status;
#else
  ACE_OS_TRACE ("ACE_OS::uname");
  ACE_OSCALL_RETURN (::uname (name), int, -1);
#endif
}
#endif /* ! ACE_WIN32 && ! VXWORKS && ! CHORUS */
