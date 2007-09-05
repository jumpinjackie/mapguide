# Default paths for libraries
#
# DO NOT EDIT lib_paths.sed
#   use lib_paths.sed.template
#
#
# Assume dependent libraries are in ../../XXX_HOME, e.g.
#   ../../db-4.5.xx
#   ../../xerces-c-src
#   ../../XQilla-x.y
#
# Windows Tcl paths - just use simple names because this doesn't apply to UNIX
s!@DB_TCLLIB@!../../bin/debug/libdb_tcl@DB_VERSION_MAJOR@@DB_VERSION_MINOR@d.dll!g
s!@DB_TEST_PATH@!@DB_HOME@/test!g
s!@DB_UTIL_PATH@!../../bin/debug!g

# DB_HOME - note that the ordering is important here so that the end result is right
s!@DB_LIB@!libdb@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_DLL@!libdb@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_JAVALIB@!libdb_java@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_JAVADLL@!libdb_java@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
# DB_TCLLIB implies the dll, above
s!@DB_TCL_LIB@!libdb_tcl@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_TCL_DLL@!libdb_tcl@DB_VERSION_MAJOR@@DB_VERSION_MINOR@!g
s!@DB_HOME@!../../@DB_NAME@!g
s!@DB_NAME@!db-@DB_VERSION_MAJOR@.@DB_VERSION_MINOR@.@DB_VERSION_PATCH@!g

# DB VERSION: 
s!@DB_VERSION_MAJOR@!4!g
s!@DB_VERSION_MINOR@!5!g
s!@DB_VERSION_PATCH@!20!g

# NOTE: this info must be sync'd with XQilla itself
s!@XQILLA_LIBHOME@!@XQILLA_HOME@/build/windows/vc6!g
s!@XQILLA_LIBHOME_D@!@XQILLA_HOME@/build/windows/vc6!g
s!@XQILLA_LIBHOME7@!@XQILLA_HOME@/build/windows/vc7.1!g
s!@XQILLA_LIBHOME7_D@!@XQILLA_HOME@/build/windows/vc7.1!g
s!@XQILLA_PROJ@!@XQILLA_HOME@/Win32Projects!g
s!@XQILLA_LIB@!xqilla@XQILLA_VERSION_MAJOR@@XQILLA_VERSION_MINOR@!g
s!@XQILLA_DLL@!xqilla@XQILLA_VERSION_MAJOR@@XQILLA_VERSION_MINOR@!g
s!@XQILLA_HOME@!../../@XQILLA_NAME@!g
#s!@XQILLA_NAME@!XQilla-@XQILLA_VERSION_MAJOR@.@XQILLA_VERSION_MINOR@!g
# Simple "xqilla"
s!@XQILLA_NAME@!xqilla!g

s!@XQILLA_VERSION_MAJOR@!1!g
s!@XQILLA_VERSION_MINOR@!0!g
s!@XQILLA_VERSION_PATCH@!1!g

# Xerces is assumed to be source (vs installation)
s!@XERCES_LIBHOME@!@XERCES_WINHOME@/Build/Win32/VC6!g
s!@XERCES_LIBHOME7@!@XERCES_WINHOME@/Build/Win32/VC7.1!g
# Xerces-c is in ../../xerces-c-src
s!@XERCES_WINHOME@!../../@XERCES_NAME@!g
s!@XERCES_TAR_NAME@!xerces-c-src_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@_@XERCES_VERSION_PATCH@!g
s!@XERCES_NAME@!xerces-c-src!g
s!@XERCES_UNIXHOME@!/usr/local!g
s!@XERCES_LIB@!xerces-c_@XERCES_VERSION_MAJOR@!g
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
s!@PYREL@!2.4!g
s!@PY_HOME@!c:/python24!g
s!@PYBSDDB_REL@!bsddb3-4.5.0!g

# Install directories (use '\' for Windows commands that may be
# used, such as xcopy)
s!@JAVA_INSTALL@!..\\..\\jar!g
s!@DLL_INSTALL@!..\\..\\bin!g
s!@BIN_INSTALL@!..\\..\\bin!g
s!@LIB_INSTALL@!..\\..\\lib!g

# DBXML names
s!@DBXML_LIBNAME@!libdbxml@DBXML_VERSION_MAJOR@@DBXML_VERSION_MINOR@!g
s!@DBXML_TCL_LIBNAME@!libdbxml_tcl@DBXML_VERSION_MAJOR@@DBXML_VERSION_MINOR@!g
s!@DBXML_JAVA_LIBNAME@!libdbxml_java@DBXML_VERSION_MAJOR@@DBXML_VERSION_MINOR@!g
s!@DBXML_PYTHON_LIBNAME@!libdbxml_python@DBXML_VERSION_MAJOR@@DBXML_VERSION_MINOR@!g

# NOTE: DBXML_VERSION_* values are set using lib_paths.sed.template,
# processed against the dist/RELEASE file, which holds the
# release numbers.  Do not edit these by hand in lib_paths.sed.
s!@DBXML_VERSION_MAJOR@!2!g
s!@DBXML_VERSION_MINOR@!3!g
s!@DBXML_VERSION_PATCH@!10!g
s!@DBXML_VERSION@!23!g
