# - Find MgGeometry component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgGeometry_INCLUDE_DIR AND MgGeometry_LIBRARIES)
   # in cache already
   SET(MgGeometry_FIND_QUIETLY TRUE)
ENDIF (MgGeometry_INCLUDE_DIR AND MgGeometry_LIBRARIES)

FIND_PATH(MgGeometry_INCLUDE_DIR Geometry.h
   HINTS
   ${MG_SRC_ROOT}/../Common/Geometry
   )

FIND_LIBRARY(MgGeometry_LIBRARIES NAMES MgGeometry
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgGeometry DEFAULT_MSG
                                  MgGeometry_INCLUDE_DIR MgGeometry_LIBRARIES)
MARK_AS_ADVANCED(MgGeometry_INCLUDE_DIR MgGeometry_LIBRARIES)
