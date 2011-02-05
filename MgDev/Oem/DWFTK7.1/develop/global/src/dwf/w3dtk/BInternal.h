//
// Copyright (c) 2000 by Tech Soft America, LLC.
// The information contained herein is confidential and proprietary to
// Tech Soft America, LLC., and considered a trade secret as defined under
// civil and criminal statutes.  Tech Soft America shall pursue its civil
// and criminal remedies in the event of unauthorized use or misappropriation
// of its trade secrets.  Use of this information by anyone other than
// authorized employees of Tech Soft America, LLC. is granted only under a
// written non-disclosure agreement, expressly prescribing the scope and
// manner of such use.
//


#ifndef BSTREAM_INTERNALS
#define BSTREAM_INTERNALS


#ifndef null
#   define null         0
#endif
#ifndef nobreak
#   define nobreak      
#endif
#ifndef countof
#   define countof(x)   (int)(sizeof(x) / sizeof(x[0]))
#endif
#ifndef forever
#   define forever      while (1)
#endif
#ifndef until
#   define until(x)     while (!(x))
#endif


#define streq(a,b)      (!strcmp(a,b))
#define streqn(a,b,n)   (!strncmp(a,b,n))

#ifdef _MSC_VER	    
#   pragma warning (disable: 4127)  
#endif

#ifdef _M_IA64
#   define POINTER_SIZED_INT __int64
#   define POINTER_SIZED_UINT unsigned __int64
#else
#   define POINTER_SIZED_INT long
#   define POINTER_SIZED_UINT unsigned long
#endif

#define UL2VP(x) (void*)(POINTER_SIZED_UINT)x
#define SL2VP(x) (void*)(POINTER_SIZED_INT)x
#define VP2UL(x) (unsigned long)(POINTER_SIZED_UINT)x
#define VP2SL(x) (long)(POINTER_SIZED_INT)x


#ifndef UNREFERENCED
  #define UNREFERENCED(x) (void)(x)
#endif


#ifdef _DEBUG
    #ifdef _MSC_VER
        #include <windows.h>
    #endif
    #include <stdio.h>
    #include <stdarg.h>
    inline void dprintf (char const * format, ...) {
        auto        char        buffer[1024];
        auto        va_list     arguments;

        va_start (arguments, format);  
        vsprintf (buffer, format, arguments);
        va_end (arguments);  
        #ifdef _MSC_VER
#ifndef _WIN32_WCE
            OutputDebugStringA (buffer);
#endif
        #else
            fprintf (stderr, buffer);
        #endif
    }
#else
    inline void dprintf (char const * format, ...) {
        UNREFERENCED(format);
    }
#endif

#endif  
