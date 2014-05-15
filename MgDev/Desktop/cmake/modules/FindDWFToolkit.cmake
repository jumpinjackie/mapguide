# - Find DWFToolkit component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (DWFToolkit_INCLUDE_DIR AND DWFToolkit_LIBRARIES)
   # in cache already
   SET(DWFToolkit_FIND_QUIETLY TRUE)
ENDIF (DWFToolkit_INCLUDE_DIR AND DWFToolkit_LIBRARIES)

FIND_PATH(DWFToolkit_INCLUDE_DIR dwf/Toolkit.h
   HINTS
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src
   )

FIND_LIBRARY(DWFToolkit_LIBRARIES NAMES dwftk-7.1.1
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src/dwf/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DWFToolkit DEFAULT_MSG
                                  DWFToolkit_INCLUDE_DIR DWFToolkit_LIBRARIES )
MARK_AS_ADVANCED(DWFToolkit_INCLUDE_DIR DWFToolkit_LIBRARIES)
