# - Find JsonCpp
# Find the JsonCpp includes and client library
# This module defines
#  JsonCpp_INCLUDE_DIR, where to find JsonCpp.h
#  JsonCpp_LIBRARY, the libraries needed to use JsonCpp.
#  JsonCpp_FOUND, If false, do not try to use JsonCpp.

find_path(JsonCpp_INCLUDE_DIR 
    NAMES "json/json.h"
    HINTS "/usr/include"
          "/usr/include/jsoncpp")
find_library(JsonCpp_LIBRARY NAMES jsoncpp)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JsonCpp DEFAULT_MSG JsonCpp_LIBRARY JsonCpp_INCLUDE_DIR)

if (JsonCpp_FOUND)
   message(STATUS "Found libJsonCpp: ${JsonCpp_INCLUDE_DIR}, ${JsonCpp_LIBRARY}")
endif (JsonCpp_FOUND)

mark_as_advanced(JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)

