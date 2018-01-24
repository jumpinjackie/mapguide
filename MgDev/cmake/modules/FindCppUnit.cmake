# - Try to find cppunit
# Once done this will define
#
#  CPPUNIT_FOUND - System has cppunit
#  CPPUNIT_INCLUDE_DIR - The cppunit include directory
#  CPPUNIT_LIBRARY_DIR - The cppunit library dir
#  CPPUNIT_LIBRARIES - The libraries needed to use cppunit
#  CPPUNIT_DEFINITIONS - Compiler switches required for using cppunit

# Copyright (c) 2009, Helio Chissini de Castro, <helio@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
   # in cache already
   SET(cppunit_FIND_QUIETLY TRUE)
ENDIF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   FIND_PACKAGE(PkgConfig)
   PKG_CHECK_MODULES(PC_CPPUNIT cppunit)
   SET(CPPUNIT_DEFINITIONS ${PC_CPPUNIT_CFLAGS_OTHER})
   SET(CPPUNIT_LIBRARY_DIR ${PC_CPPUNIT_LIBRARY_DIRS})
ENDIF (NOT WIN32)

FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/CompilerOutputter.h
   HINTS
   ${PC_CPPUNIT_INCLUDEDIR}
   ${PC_CPPUNIT_INCLUDE_DIRS}
   PATH_SUFFIXES cppunit
   )

FIND_LIBRARY(CPPUNIT_LIBRARIES NAMES cppunit
   HINTS
   ${PC_CPPUNIT_LIBDIR}
   ${PC_CPPUNIT_LIBRARY_DIRS}
   )

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(cppunit DEFAULT_MSG CPPUNIT_LIBRARIES CPPUNIT_INCLUDE_DIR)

IF(CPPUNIT_FOUND)
	TRY_COMPILE(CPPUNIT_MODERN_API ${CMAKE_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/TestCmakeVersion.cxx )

	IF(CPPUNIT_MODERN_API)
		MESSAGE(STATUS "CPPUNIT uses modern API ( > 1.10 )")
		ADD_DEFINITIONS(-DCPPUNIT_MODERN_API)
	ENDIF(CPPUNIT_MODERN_API)

	MARK_AS_ADVANCED(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARIES CPPUNIT_LIBRARY_DIR)
ENDIF(CPPUNIT_FOUND)

