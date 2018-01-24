# - Find GEOS
# Find the GEOS includes and client library. NOTE: This concerns the C++ library, not the geos_c C API.
# This module defines
#  GEOS_INCLUDE_DIR, where to find geos headers
#  GEOS_LIBRARY, the libraries needed to use GEOS.
#  GEOS_FOUND, If false, do not try to use GEOS.

find_path(GEOS_INCLUDE_DIR 
  NAMES "geos/version.h"
        "geos/geom.h"
        "geos/util.h"
        "geos/io.h"
        "geos/unload.h")
find_library(GEOS_LIBRARY NAMES geos)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOS DEFAULT_MSG GEOS_LIBRARY GEOS_INCLUDE_DIR)

if (GEOS_FOUND)
   message(STATUS "Found libGEOS: ${GEOS_INCLUDE_DIR}, ${GEOS_LIBRARY}")
endif (GEOS_FOUND)

mark_as_advanced(GEOS_INCLUDE_DIR GEOS_LIBRARY)

