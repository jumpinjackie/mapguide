#ifdef WIN32
#define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL
#define ACE_HAS_WCHAR
#define ACE_USES_WCHAR
#include "ace/config-win32.h"
#else
#define ACE_HAS_WCHAR
#define ACE_HAS_THREAD_SAFE_ACCEPT
#include "ace/config-linux.h"
#endif
