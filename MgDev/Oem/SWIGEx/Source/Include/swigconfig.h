/* Source/Include/swigconfig.h.in.  Generated from configure.in by autoheader.  */

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the `dl' library (-ldl). */
#undef HAVE_LIBDL

/* Define to 1 if you have the `dld' library (-ldld). */
#undef HAVE_LIBDLD

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Runtime library installation directory */
#undef LIBDIR

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
#undef NO_MINUS_C_MINUS_O

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Executable and runtime libraries release suffix for co-existence with older
   versions */
#undef RELEASE_SUFFIX

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Compiler that built SWIG */
#undef SWIG_CXX

/* Directory for SWIG system-independent libraries */
#undef SWIG_LIB

/* Platform that SWIG is built for */
#undef SWIG_PLATFORM

/* Version number of package */
#undef VERSION


/* Default language */
#define SWIG_LANG               "-php"

#define LIBDIR				"/usr/local/lib"
#define RELEASE_SUFFIX		""
#define PACKAGE_VERSION		"1.3.21"
#ifdef WIN32
#define SWIG_CXX			"Visual C++ v13.10.3077"
#else
#define SWIG_CXX			"GNU C++ v3.2.3"
#endif
#define SWIG_PLATFORM		"i686"
#define PACKAGE_BUGREPORT	"<Undefined Bug report package>"
#define SWIG_LIB			".\\"

/* Values returned by SWIG when invoked with the -ldflags option */
#define SWIG_GUILE_RUNTIME      "-L" LIBDIR " -lswigguile" RELEASE_SUFFIX
#define SWIG_GUILESCM_RUNTIME   "-L" LIBDIR " -lswigguilescm" RELEASE_SUFFIX
#define SWIG_MZSCHEME_RUNTIME   "-L" LIBDIR " -lswigmz" RELEASE_SUFFIX
#define SWIG_PERL_RUNTIME       "-L" LIBDIR " -lswigpl" RELEASE_SUFFIX
#define SWIG_PIKE_RUNTIME       "-L" LIBDIR " -lswigpike" RELEASE_SUFFIX
#define SWIG_PHP_RUNTIME        "-L" LIBDIR " -lswigphp4" RELEASE_SUFFIX
#define SWIG_PYTHON_RUNTIME     "-L" LIBDIR " -lswigpy" RELEASE_SUFFIX
#define SWIG_RUBY_RUNTIME       "-L" LIBDIR " -lswigrb" RELEASE_SUFFIX
#define SWIG_TCL_RUNTIME        "-L" LIBDIR " -lswigtcl" RELEASE_SUFFIX
#define SWIG_CHICKEN_RUNTIME    "-L" LIBDIR " -lswigchicken" RELEASE_SUFFIX

