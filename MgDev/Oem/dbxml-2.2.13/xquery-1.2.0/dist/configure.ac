# $Id: configure.ac,v 1.16 2005/09/26 13:43:13 jsnelson Exp $
# Process this file with autoconf to produce a configure script.

PACKAGE=xquery
AC_INIT(Berkeley DB XQuery,
	__EDIT_XQUERY_VERSION__, support@sleepycat.com, xquery-__EDIT_XQUERY_VERSION__)
AC_CONFIG_SRCDIR([../src/XQContext.cpp])
AC_FUNC_ALLOCA
AC_CONFIG_HEADER(xquery_config.h:config.hin)

# Configure setup.
AC_CANONICAL_HOST()
AC_ARG_PROGRAM()

# We cannot build in the top-level directory.
AC_MSG_CHECKING(if building in the top-level directory)
[ test -f LICENSE ] && AC_MSG_ERROR([
Berkeley DB XQuery cannot be built in the top-level distribution directory.])
AC_MSG_RESULT(no)

# Substitution variables.
AC_SUBST(ADDITIONAL_CLASSPATH)
AC_SUBST(ADDITIONAL_LANG)
AC_SUBST(ADDITIONAL_PROGS)
AC_SUBST(BUILD_TARGET)
AC_SUBST(CXXFLAGS)
AC_SUBST(DEFAULT_LIB)
AC_SUBST(DEFAULT_LIB_CXX)
AC_SUBST(INSTALLER)
AC_SUBST(INSTALL_LIBS)
AC_SUBST(INSTALL_TARGET)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)
AC_SUBST(LIBSO_LIBS)
AC_SUBST(LIBTOOL)
AC_SUBST(LIBXSO_LIBS)
AC_SUBST(LOAD_LIBS)
AC_SUBST(MAKEFILE_CC)
AC_SUBST(MAKEFILE_CCLINK)
AC_SUBST(MAKEFILE_CC_MM)
AC_SUBST(MAKEFILE_CXX)
AC_SUBST(MAKEFILE_CXXLINK)
AC_SUBST(MAKEFILE_CXX_MM)
AC_SUBST(MAKEFILE_SOLINK)
AC_SUBST(MAKEFILE_XSOLINK)
AC_SUBST(MAKEFILE_LIBTOOL)
AC_SUBST(POSTLINK)
AC_SUBST(SOFLAGS)
AC_SUBST(o)
AC_SUBST(BUILD_CPP_DEP)
AC_SUBST(BUILD_CC_DEP)

# Set the default installation location.
AC_PREFIX_DEFAULT(/usr/local/BerkeleyDB.XQuery.__EDIT_XQUERY_VERSION_MAJOR__.__EDIT_XQUERY_VERSION_MINOR__)

# Configure the version information.
AC_SUBST(XQUERY_VERSION_MAJOR)
XQUERY_VERSION_MAJOR="__EDIT_XQUERY_VERSION_MAJOR__"
AC_SUBST(XQUERY_VERSION_MINOR)
XQUERY_VERSION_MINOR="__EDIT_XQUERY_VERSION_MINOR__"
AC_SUBST(XQUERY_VERSION_PATCH)
XQUERY_VERSION_PATCH="__EDIT_XQUERY_VERSION_PATCH__"
AC_SUBST(XQUERY_VERSION_STRING)
XQUERY_VERSION_STRING='"__EDIT_XQUERY_VERSION_STRING__"'

# Check for programs used in building and installation.
AM_PROGRAMS_SET
AC_PROG_INSTALL

# The default compiler is cc (NOT gcc), the default CFLAGS is as specified
# above, NOT what is set by AC_PROG_CC, as it won't set optimization flags
# for any compiler other than gcc.  We need to do this before checking the
# options because it relies on LIBS, and we want to add libtool *.la files
# to that, which cc/gcc doesn't understand.
#CFLAGS= ${CFLAGS:=""}
AC_PROG_CC(cc gcc)

# Process all options before using them.
AM_OPTIONS_SET

# Set some #defines based on configuration options.
if test "$db_cv_diagnostic" = yes; then
	AC_DEFINE(DIAGNOSTIC)
	AH_TEMPLATE(DIAGNOSTIC,
    [Define to 1 if you want a version with run-time diagnostic checking.])
fi
if test "$db_cv_umrw" = yes; then
	AC_DEFINE(UMRW)
	AH_TEMPLATE(UMRW,
    [Define to 1 to mask harmless unitialized memory read/writes.])
fi

# This is where we handle stuff that autoconf can't handle: compiler,
# preprocessor and load flags, libraries that the standard tests don't
# look for.  The default optimization is -O.  We would like to set the
# default optimization for systems using gcc to -O2, but we can't.  By
# the time we know we're using gcc, it's too late to set optimization
# flags.
#
# There are additional libraries we need for some compiler/architecture
# combinations.
#
# Some architectures require DB to be compiled with special flags and/or
# libraries for threaded applications
#
# The makefile CC may be different than the CC used in config testing,
# because the makefile CC may be set to use $(LIBTOOL).
#
# XXX
# Don't override anything if it's already set from the environment.
optimize_def="-O"
case "$host_os" in
aix4.3.*|aix5*)
	optimize_def="-O2"
	CC=${CC-"xlc_r"}
	CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE"
	LDFLAGS="$LDFLAGS -Wl,-brtl";;
bsdi3*)	optimize_def="-O2"
	CC=${CC-"shlicc2"}
	LIBS="$LIBS -lipc";;
bsdi*)	optimize_def="-O2";;
freebsd*)
	optimize_def="-O2"
	CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE"
	LDFLAGS="$LDFLAGS -pthread";;
hpux*)	CPPFLAGS="$CPPFLAGS -D_REENTRANT";;
irix*)	optimize_def="-O2"
	CPPFLAGS="$CPPFLAGS -D_SGI_MP_SOURCE";;
linux*)	optimize_def="-O2"
	CPPFLAGS="$CPPFLAGS -D_ISOC99_SOURCE -D_GNU_SOURCE -D_REENTRANT";;
mpeix*)	CPPFLAGS="$CPPFLAGS -D_POSIX_SOURCE -D_SOCKET_SOURCE"
	LIBS="$LIBS -lsocket -lsvipc";;
osf*)	CPPFLAGS="$CPPFLAGS -pthread";;
solaris*)
	CPPFLAGS="$CPPFLAGS -D_REENTRANT";;
esac

# Default XML flags.
CPPFLAGS="$CPPFLAGS"

# If the user wants a debugging environment, add -g to the CFLAGS value.
#
# XXX
# Some compilers can't mix optimizing and debug flags.  The only way to
# handle this is to specify CFLAGS in the environment before configuring.
if test "$db_cv_profile" = yes; then
	CFLAGS="$CFLAGS -pg -g"
	CXXFLAGS="$CXXFLAGS -pg -g"
	LDFLAGS="$LDFLAGS -pg"
	db_cv_build_type="profile"
else
if test "$db_cv_debug" = yes; then
	AC_DEFINE(DEBUG)
	AH_TEMPLATE(DEBUG, [Define to 1 if you want a debugging version.])

  if test $ac_compiler_gnu = "yes"
     then CFLAGS="$CFLAGS -g -Wall"
          CXXFLAGS="$CXXFLAGS -g -Wall"
  fi
	db_cv_build_type="debug"
else
  # Add the optimize flag, if we're not in debug, and CFLAGS wasn't
  # defined in the environment already
  CFLAGS=${CFLAGS:-$optimize_def}
  CXXFLAGS=${CXXFLAGS:-"$CFLAGS"}

	db_cv_build_type="release"
fi
fi
AC_SUBST(db_cv_build_type)

# build include dependencies or not
BUILD_CPP_DEP='$(nocppdeps)'
BUILD_CC_DEP='$(noccdeps)'
if test "$db_cv_depend" = yes; then
   	BUILD_CPP_DEP='$(cppdeps)'
   	BUILD_CC_DEP='$(ccdeps)'
fi

# Because of shared library building, the ${CC} used for config tests
# may be different than the ${CC} we want to put in the Makefile.
# The latter is known as ${MAKEFILE_CC} in this script.
MAKEFILE_CC="${CC}"
MAKEFILE_CCLINK="${CC}"
MAKEFILE_CC_MM="${CC} -MM"
MAKEFILE_CXX="nocxx"
MAKEFILE_CXXLINK="nocxx"
MAKEFILE_CXX_MM="nocxx"

BUILD_TARGET="library_build"
INSTALL_TARGET="library_install"

# Since we prefered cc over gcc, we'll also prefer the vendor's compiler over
# g++/gcc.  If we're wrong, the user can set CC and CXX in their environment
# before running configure.
#
# AC_PROG_CXX sets CXX, but it uses $CXX and $CCC (in that order) as its
# first choices.
if test "$GCC" != "yes"; then
	case "$host_os" in
	aix*)		AC_CHECK_TOOL(CCC, xlC_r)
			LIBXSO_LIBS="-lC_r $LIBXSO_LIBS"
			LIBS="-lC_r $LIBS";;
	hpux*)		AC_CHECK_TOOL(CCC, aCC);;
	irix*)		AC_CHECK_TOOL(CCC, CC);;
	osf*)		AC_CHECK_TOOL(CCC, cxx);;
	solaris*)	AC_CHECK_TOOL(CCC, CC);;
	esac
fi
AC_PROG_CXX

MAKEFILE_CXX="${CXX}"
MAKEFILE_CXXLINK="${CXX}"
MAKEFILE_CXX_MM="${CXX} -MM"

dnl --- Set up compiler flags ---
# turn off Xerces-based warning for aCC
# get correct flags for aCC for STL and PIC code
if test "$GXX" != "yes"
   then case $host_os in
      	   hpux11.0*|hpux11*) CXXFLAGS="$CXXFLAGS -mt"
	   case $CXX in
	   	aCC) CXXFLAGS="$CXXFLAGS -AA +W930,740,749,307,890"
		     MAKEFILE_CXX_MM="${CXX} +maked -E"
		     LDFLAGS="$LDFLAGS -AA"
		   ;;
	   esac
	   ;;
      	   aix*)
	   case $CXX in
	   	xlC*) CXXFLAGS="$CXXFLAGS -qstaticinline"
		     LDFLAGS="$LDFLAGS -bh:5"
		     MAKEFILE_CXX_MM="${CXX} -E"
		   ;;
	   esac
	   ;;
      esac
fi

# Export our compiler preferences for the libtool configuration.
export CC CCC
CCC=CXX

# Libtool configuration.
AC_PROG_LIBTOOL

SOFLAGS="-rpath \$(libdir)"

# Set SOSUFFIX and friends
SOSUFFIX_CONFIG
MODSUFFIX_CONFIG

INSTALLER="\$(LIBTOOL) --mode=install cp -p"

MAKEFILE_CC="\$(LIBTOOL) --mode=compile ${MAKEFILE_CC}"
MAKEFILE_SOLINK="\$(LIBTOOL) --mode=link ${MAKEFILE_CCLINK} -avoid-version"
MAKEFILE_CCLINK="\$(LIBTOOL) --mode=link ${MAKEFILE_CCLINK}"
MAKEFILE_CXX="\$(LIBTOOL) --mode=compile ${MAKEFILE_CXX}"
MAKEFILE_XSOLINK="\$(LIBTOOL) --mode=link ${MAKEFILE_CXXLINK} -avoid-version"
MAKEFILE_CXXLINK="\$(LIBTOOL) --mode=link ${MAKEFILE_CXXLINK}"
MAKEFILE_LIBTOOL="$SHELL \$(builddir)/libtool"
LIBTOOL="$SHELL ./libtool"

# Configure for shared libraries, static libraries, or both.  If both are
# configured, build the utilities and example programs with shared versions.
#
# $o is set to ".o" or ".lo", and is the file suffix used in the Makefile
# instead of .o
if test `$LIBTOOL --config | grep build_libtool_libs | grep no` 2>/dev/null
then
	DEFAULT_LIB="\$(libxquery)"
	POSTLINK="@true"
	o=".o"
	enable_shared="no"
else
	DEFAULT_LIB="\$(libxso_target)"
	POSTLINK="\$(LIBTOOL) --mode=execute true"
	o=".lo"
	enable_shared="yes"
fi
INSTALL_LIBS="$DEFAULT_LIB"

# It's now safe to switch language to C++ and test whether the configuration works.
AC_LANG(C++)

AC_MSG_CHECKING([whether additional libraries are needed to link])
orig_cxx=$CXX
CXX="$LIBTOOL --mode=link $CXX"
AC_TRY_LINK([#include <pathan/PathanPlatformUtils.hpp>],
	[PathanPlatformUtils::initialize(0);], [db_cv_libneeded=no], [db_cv_libneeded=yes])
orig_libs=$LIBS
for lib in -lpthread -lthread "-lsocket -lnsl"; do
	if test "$db_cv_libneeded" = yes; then
		LIBS="$orig_libs $lib"
		AC_TRY_LINK([#include <pathan/PathanPlatformUtils.hpp>],
			[PathanPlatformUtils::initialize(0);], [db_cv_libneeded=$lib])
	fi
done
AC_MSG_RESULT([$db_cv_libneeded])
case "$db_cv_libneeded" in
yes)		AC_MSG_WARN([Unable to determine additional libraries required to link.  Check the build instructions for your platform]);;
no)		LIBS="$orig_libs";;
*)		LIBS="$orig_libs $db_cv_libneeded";;
esac
CXX=$orig_cxx

case "$host_os" in
darwin*)
	LIBTSO_MODULE=""
	LIBTSO_MODSUFFIX=".dylib"
	LIBJSO_LN_BUILD="test -f .libs/\$(libjso_base).dylib || \$(ln) -s \$(libjso_base).dylib .libs/\$(libjso_base).jnilib"
	LIBJSO_LN_INSTALL="test -f \$(libjso_base)-\$(SOVERSION).dylib && \$(ln) -s \$(libjso_base)-\$(SOVERSION).dylib \$(libjso_base)-\$(SOVERSION).jnilib";;
*)
	LIBTSO_MODULE="-module"
	LIBTSO_MODSUFFIX=@MODSUFFIX@
	LIBJSO_LN_BUILD=""
	LIBJSO_LN_INSTALL="";;
esac

AM_TCL_LOAD

# Checks for system functions for which we have replacements.
#
# XXX
# The only portable getcwd call is getcwd(char *, size_t), where the
# buffer is non-NULL -- Solaris can't handle a NULL buffer, and they
# deleted getwd().
orig_libs=$LIBS
LIBS=
AC_REPLACE_FUNCS(getcwd getopt memcmp memcpy memmove raise)
AC_REPLACE_FUNCS(snprintf strcasecmp strdup strerror vsnprintf)
LIBS=$orig_libs

# This is necessary so that .o files in LIBOBJS are also built via
# the ANSI2KNR-filtering rules.
LIB@&t@OBJS=`echo "$LIB@&t@OBJS" |
	     sed 's,\.[[^.]]* ,$U&,g;s,\.[[^.]]*$,$U&,'`
LTLIBOBJS=`echo "$LIB@&t@OBJS" |
	   sed 's,\.[[^.]]* ,.lo ,g;s,\.[[^.]]*$,.lo,'`
AC_SUBST(LTLIBOBJS)

# Initial output file list.
AC_CONFIG_FILES(Makefile.defs:Makefile.in include.tcl:$srcdir/../test/include.tcl)
AC_OUTPUT
