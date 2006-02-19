# Default paths for libraries
# $Id: lib_paths.sed,v 1.39 2005/09/25 23:36:37 gmf Exp $

#
# Assume dependent libraries are in ../../XXX_HOME, e.g.
#   ../../db-4.3.xx
#   ../../xerces-c-src_2_6_0
#   ../../pathan
#   ../../xquery-1.2.xx
#
# Windows Tcl paths - just use simple names because this doesn't apply to UNIX
s!@DB_TCLLIB@!../../bin/debug/libdb_tcl@DB_VERSION_MAJOR@@DB_VERSION_MINOR@d.dll!g
s!@DB_TEST_PATH@!@DB_HOME@/test!g
s!@DB_UTIL_PATH@!../../bin/debug!g

# DB_HOME - note that the ordering is important here so that the end result is right
s!@DB_LIB@!libdb@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
# DB_TCLLIB implies the dll, above
s!@DB_TCL_LINKLIB@!libdb_tcl@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_HOME@!../../@DB_NAME@!g
s!@DB_NAME@!db-@DB_VERSION_MAJOR@.@DB_VERSION_MINOR@.@DB_VERSION_PATCH@!g

# DB VERSION: 
s!@DB_VERSION_MAJOR@!4!g
s!@DB_VERSION_MINOR@!3!g
s!@DB_VERSION_PATCH@!29!g

s!@XQUERY_LIB@!libxquery@XQUERY_VERSION_MAJOR@@XQUERY_VERSION_MINOR@!g
# Xquery is in ../../xquery-major.minor.patch
s!@XQUERY_HOME@!../../@XQUERY_NAME@!g
s!@XQUERY_NAME@!xquery-@XQUERY_VERSION_MAJOR@.@XQUERY_VERSION_MINOR@.@XQUERY_VERSION_PATCH@!g

s!@XQUERY_VERSION_MAJOR@!1!g
s!@XQUERY_VERSION_MINOR@!2!g
s!@XQUERY_VERSION_PATCH@!0!g

s!@PATHAN_HOME@!/usr/local!g
s!@PATHAN_LIBHOME@!@PATHAN_WINHOME@/lib!g
s!@PATHAN_LIBHOME7@!@PATHAN_WINHOME@/lib!g
# Pathan 2 is in ../../pathan
#s!@PATHAN_WINHOME@!../../libpathan-@PATHAN_VERSION_MAJOR@@PATHAN_VERSION_MINOR@l!g
s!@PATHAN_WINHOME@!../../pathan!g
s!@PATHAN_VERSION_MAJOR@!2!g
s!@PATHAN_VERSION_MINOR@!0!g
s!@PATHAN_LIB@!Pathan!g
s!@PATHAN_LIB7@!Pathan_7.1!g
s!@PATHAN_LIBD7@!PathanD_7.1!g

# Xerces is assumed to be source (vs installation)
s!@XERCES_LIBHOME@!@XERCES_WINHOME@/Build/Win32/VC6!g
s!@XERCES_LIBHOME7@!@XERCES_WINHOME@/Build/Win32/VC7.1!g
# Xerces-c is in ../../xerces-c-src
s!@XERCES_WINHOME@!../../@XERCES_NAME@!g
s!@XERCES_TAR_NAME@!xerces-c-src_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@_@XERCES_VERSION_PATCH@!g
s!@XERCES_NAME@!xerces-c-src!g
s!@XERCES_UNIXHOME@!/usr/local!g
s!@XERCES_LIB@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_LIB7@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_DLL@!xerces-c_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@!g

# Current Xerces version is 2.7
s!@XERCES_VERSION_MAJOR@!2!g
s!@XERCES_VERSION_MINOR@!7!g
s!@XERCES_VERSION_PATCH@!0!g

# Tcl library (change these with new releases/locations).  Will
# have to be changed in projects based on user's machine configuration.
s!@TCLLIB@!tcl84!g
s!@TCL_HOME@!c:/tcl!g

# Python library (change these with new releases/locations).  Will
# have to be changed in projects based on user's machine configuration.
s!@PYLIB@!python24!g
s!@PY_HOME@!c:/python24!g

# NOTE: DBXML_VERSION_MAJOR/MINOR are shell variables,
# obtained from dist/RELEASE, and used in calling scripts
