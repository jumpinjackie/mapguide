# - Find MgFoundation component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgFoundation_INCLUDE_DIR AND MgFoundation_LIBRARIES)
   # in cache already
   SET(MgFoundation_FIND_QUIETLY TRUE)
ENDIF (MgFoundation_INCLUDE_DIR AND MgFoundation_LIBRARIES)

FIND_PATH(MgFoundation_INCLUDE_DIR Foundation.h
   HINTS
   ${MG_SRC_ROOT}/../Common/Foundation
   )

FIND_LIBRARY(MgFoundation_LIBRARIES NAMES MgFoundation
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgFoundation DEFAULT_MSG
                                  MgFoundation_INCLUDE_DIR MgFoundation_LIBRARIES)
MARK_AS_ADVANCED(MgFoundation_INCLUDE_DIR MgFoundation_LIBRARIES)
