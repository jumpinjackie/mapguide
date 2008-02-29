#	$Id: lib_paths.sed,v 1.1 2006/01/13 21:12:31 gmfeinberg Exp $
# Default path for XQilla library, and paths to
# Xerces
#
#
# XQilla libraries are named:
#   xqilla<major_version><minor_version>{d}.{lib,dll,pdb}
# e.g.
#   xqilla10.dll (release) or xqilla10d.dll (debug)
#
# Assume dependent libraries are in ../../../XXX_HOME, e.g.
#   ../../../xerces-c-src_2_7_0

s!@XQILLA_LIB@!@XQILLA_LIB_BASE@@XQILLA_VERSION_MAJOR@@XQILLA_VERSION_MINOR@!g
s!@XQILLA_LIB_BASE@!xqilla!g

s!@XQILLA_VERSION_MAJOR@!1!g
s!@XQILLA_VERSION_MINOR@!0!g
s!@XQILLA_VERSION_PATCH@!0!g

# Xerces is assumed to be source (vs installation)
s!@XERCES_LIBHOME@!@XERCES_WINHOME@/Build/Win32/VC6!g
s!@XERCES_LIBHOME7@!@XERCES_WINHOME@/Build/Win32/VC7.1!g
# Xerces-c is in ../../../xerces-c-src
s!@XERCES_WINHOME@!../../../@XERCES_NAME@!g
s!@XERCES_NAME@!xerces-c-src!g
s!@XERCES_LIB@!xerces-c_@XERCES_VERSION_MAJOR@!g
s!@XERCES_DLL@!xerces-c_@XERCES_VERSION_MAJOR@_@XERCES_VERSION_MINOR@!g

# Current Xerces version is 2.7
s!@XERCES_VERSION_MAJOR@!2!g
s!@XERCES_VERSION_MINOR@!7!g
s!@XERCES_VERSION_PATCH@!0!g

