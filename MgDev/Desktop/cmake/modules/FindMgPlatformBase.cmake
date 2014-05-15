# - Find MgPlatformBase component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgPlatformBase_INCLUDE_DIR AND MgPlatformBase_LIBRARIES)
   # in cache already
   SET(MgPlatformBase_FIND_QUIETLY TRUE)
ENDIF (MgPlatformBase_INCLUDE_DIR AND MgPlatformBase_LIBRARIES)

FIND_PATH(MgPlatformBase_INCLUDE_DIR PlatformBase.h
   HINTS
   ${MG_SRC_ROOT}/../Common/PlatformBase
   )

FIND_LIBRARY(MgPlatformBase_LIBRARIES NAMES MgPlatformBase
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgPlatformBase DEFAULT_MSG
                                  MgPlatformBase_INCLUDE_DIR MgPlatformBase_LIBRARIES)
MARK_AS_ADVANCED(MgPlatformBase_INCLUDE_DIR MgPlatformBase_LIBRARIES)
