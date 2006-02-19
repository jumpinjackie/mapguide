#ifndef _PATHAN_CONFIG_H
#define _PATHAN_CONFIG_H

/** BEGIN CONFIG.H **/
/* Configure-style definitions for Windows.  */

/* Define to empty if the keyword does not work.  */
/* MS VC++ supports const */
#undef const

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* Intel & Alpha architechtures are small endian */
#undef WORDS_BIGENDIAN

/* MSVC++ requires explicit exportation of symbols accessed in a DLL */
/* If we are building the library, export symbols */
/* If we are linking against the library, import symbols */

#define PATHAN_EXPORT __declspec(dllimport)

#if defined(PROJ_PATHAN)
#undef PATHAN_EXPORT
#define PATHAN_EXPORT __declspec(dllexport)
#endif

#if defined(PROJ_PATHAN_STATIC)
#undef PATHAN_EXPORT
#define PATHAN_EXPORT
#endif

// Warning 4064: switch statement contains 'default' but no 'case' labels
// 
// Warning 4251: We have a lot of std::vector objects inside exported classes, and VC++ complains about it
// "class 'std::vector<XXXX> >' needs to have dll-interface to be used by clients of class 'YYYY'"
//
// Warning 4786: "identifier was truncated to '255' characters in the browser information"
//
// Warning 4101: "unreferenced local variable"
//
// Warning 4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable: 4065 4251 4786 4101 4290)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/** END CONFIG.H **/
#endif

