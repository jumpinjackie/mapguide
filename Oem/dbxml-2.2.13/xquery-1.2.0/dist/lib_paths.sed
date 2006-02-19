# Default paths for libraries
# $Id: lib_paths.sed,v 1.8 2005/09/08 19:38:55 gmf Exp $

s!@PATHAN_WINHOME@!../../pathan!g
s!@PATHAN_HOME@!../../pathan!g
s!@PATHAN_LIB@!Pathan!g
s!@PATHAN_LIBHOME@!../../pathan/lib!g
s!@PATHAN_LIB7@!Pathan_7.1!g
s!@PATHAN_LIBD7@!PathanD_7.1!g

# These are the same while we are building from source - that will change
s!@XERCES_LIBHOME@!@XERCES_WINHOME@/Build/Win32/VC6!
s!@XERCES_LIBHOME7@!@XERCES_WINHOME@/Build/Win32/VC7.1!
s!@XERCES_WINHOME@!../../xerces-c-src!g
s!@XERCES_UNIXHOME@!../../xerces-c-src!g
s!@XERCES_LIB@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_LIB7@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_DLL@!xerces-c_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@_@XERCES_VERSION_PATCH@!g

s!@XERCES_VERSION_MAJOR@!2!g
s!@XERCES_VERSION_MINOR@!&!g
s!@XERCES_VERSION_PATCH@!0!g
