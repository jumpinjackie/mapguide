# - Try to find XercesC
# Once done this will define
#
#  XercesC_FOUND - System has XercesC
#  XercesC_INCLUDE_DIR - The XercesC include directory
#  XercesC_LIBRARIES - The libraries needed to use XercesC

# Copyright (c) 2009, Helio Chissini de Castro, <helio@kde.org>. Modified for use by mg-desktop by Jackie Ng <jumpinjackie@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (XercesC_INCLUDE_DIR AND XercesC_LIBRARIES)
   # in cache already
   SET(XercesC_FIND_QUIETLY TRUE)
ENDIF (XercesC_INCLUDE_DIR AND XercesC_LIBRARIES)

FIND_PATH(XercesC_INCLUDE_DIR xercesc/util/PlatformUtils.hpp
   HINTS
   ${MG_SRC_ROOT}/../Oem/dbxml/xerces-c-src/src
   PATH_SUFFIXES xerces-c
   )

FIND_LIBRARY(XercesC_LIBRARIES NAMES xerces-c
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Oem/dbxml/xerces-c-src/src/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XercesC DEFAULT_MSG
                                  XercesC_INCLUDE_DIR XercesC_LIBRARIES)
MARK_AS_ADVANCED(XercesC_INCLUDE_DIR XercesC_LIBRARIES)
