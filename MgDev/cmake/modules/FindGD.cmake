# - Find GD
# Find the GD includes and client library
# This module defines
#  GD_INCLUDE_DIR, where to find gd.h
#  GD_LIBRARY, the libraries needed to use GD.
#  GD_FOUND, If false, do not try to use GD.

find_path(GD_INCLUDE_DIR NAMES "gd.h")
find_library(GD_LIBRARY NAMES gd)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set GD_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GD DEFAULT_MSG GD_LIBRARY GD_INCLUDE_DIR)

if (GD_FOUND)
   message(STATUS "Found libgd: ${GD_INCLUDE_DIR}, ${GD_LIBRARY}")
endif (GD_FOUND)

mark_as_advanced(GD_INCLUDE_DIR GD_LIBRARY)

