# - Find DWFEMap component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (DWFEMap_INCLUDE_DIR AND DWFEMap_LIBRARIES)
   # in cache already
   SET(DWFEMap_FIND_QUIETLY TRUE)
ENDIF (DWFEMap_INCLUDE_DIR AND DWFEMap_LIBRARIES)

FIND_PATH(DWFEMap_INCLUDE_DIR dwfemap/EMapToolkit.h
   HINTS
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src
   )

FIND_LIBRARY(DWFEMap_LIBRARIES NAMES dwfemap-1.0.0
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Oem/DWFTK7.1/develop/global/src/dwfemap/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DWFEMap DEFAULT_MSG
                                  DWFEMap_INCLUDE_DIR DWFEMap_LIBRARIES )
MARK_AS_ADVANCED(DWFEMap_INCLUDE_DIR DWFEMap_LIBRARIES)
