// I think we want this for all WIN32 stuff, not just #ifdef _MSC_VER
// but I could be wrong -- swhite
#if defined(WIN32) && !defined(__CYGWIN__)
#include "pathan_config_win32.h"
#else
#include "pathan_config_unix.h"
#endif

