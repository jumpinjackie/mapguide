find_path(ACE_INCLUDE_DIR NAMES "ace/Init_ACE.h")
find_library(ACE_LIBRARY NAMES ACE)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ACE DEFAULT_MSG ACE_LIBRARY ACE_INCLUDE_DIR)

if (ACE_FOUND)
   message(STATUS "Found ACE: ${ACE_INCLUDE_DIR}, ${ACE_LIBRARY}")
endif (ACE_FOUND)

mark_as_advanced(ACE_INCLUDE_DIR ACE_LIBRARY)
