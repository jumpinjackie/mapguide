# - Find TCLAP
# Find the headers for the TCLAP command-line parser library
# This module defines
#  TCLAP_INCLUDE_DIR, where to find TCLAP headers

find_path(TCLAP_INCLUDE_DIR 
    NAMES "tclap/CmdLine.h"
    PATHS 
        /usr/include
        ${MG_OEM_ROOT_DIR}/tclap-1.2.2/include)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set TCLAP_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TCLAP DEFAULT_MSG TCLAP_INCLUDE_DIR)

if (TCLAP_FOUND)
   message(STATUS "Found TCLAP: ${TCLAP_INCLUDE_DIR}")
endif (TCLAP_FOUND)

mark_as_advanced(TCLAP_INCLUDE_DIR)

