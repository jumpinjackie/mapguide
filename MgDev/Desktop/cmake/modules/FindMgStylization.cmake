# - Find MgStylization component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgStylization_INCLUDE_DIR AND MgStylization_LIBRARIES)
   # in cache already
   SET(MgStylization_FIND_QUIETLY TRUE)
ENDIF (MgStylization_INCLUDE_DIR AND MgStylization_LIBRARIES)

FIND_PATH(MgStylization_INCLUDE_DIR Stylization.h
   HINTS
   ${MG_SRC_ROOT}/../Common/Stylization
   )

FIND_LIBRARY(MgStylization_LIBRARIES NAMES MgStylization
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgStylization DEFAULT_MSG
                                  MgStylization_INCLUDE_DIR MgStylization_LIBRARIES)
MARK_AS_ADVANCED(MgStylization_INCLUDE_DIR MgStylization_LIBRARIES)
