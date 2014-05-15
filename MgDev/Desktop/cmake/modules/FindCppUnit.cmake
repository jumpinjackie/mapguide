# - Try to find cppunit
# Once done this will define
#
#  CPPUNIT_FOUND - System has cppunit
#  CPPUNIT_INCLUDE_DIR - The cppunit include directory
#  CPPUNIT_LIBRARIES - The libraries needed to use cppunit

# Copyright (c) 2009, Helio Chissini de Castro, <helio@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
   # in cache already
   SET(cppunit_FIND_QUIETLY TRUE)
ENDIF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)

FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/CompilerOutputter.h
   HINTS
   ${MG_SRC_ROOT}/../Oem/CppUnit-1.9.14/include
   PATH_SUFFIXES cppunit
   )

FIND_LIBRARY(CPPUNIT_LIBRARIES NAMES cppunit
   HINTS
   ${MG_SRC_ROOT}/../Oem/CppUnit-1.9.14/src/cppunit/.libs
   )

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(cppunit DEFAULT_MSG CPPUNIT_LIBRARIES CPPUNIT_INCLUDE_DIR)

MARK_AS_ADVANCED(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARIES CPPUNIT_LIBRARY_DIR)
