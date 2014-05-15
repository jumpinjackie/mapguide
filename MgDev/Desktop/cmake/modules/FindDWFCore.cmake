# - Find DWFCore component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (DWFCore_INCLUDE_DIR AND DWFCore_LIBRARIES)
   # in cache already
   SET(DWFCore_FIND_QUIETLY TRUE)
ENDIF (DWFCore_INCLUDE_DIR AND DWFCore_LIBRARIES)

FIND_PATH(DWFCore_INCLUDE_DIR dwfcore/Core.h
   HINTS
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src
   )

FIND_LIBRARY(DWFCore_LIBRARIES NAMES dwfcore-1.1.1
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src/dwfcore/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DWFCore DEFAULT_MSG
                                  DWFCore_INCLUDE_DIR DWFCore_LIBRARIES )
MARK_AS_ADVANCED(DWFCore_INCLUDE_DIR DWFCore_LIBRARIES)
