# - Find MgGwsQueryEngine component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgGwsQueryEngine_INCLUDE_DIR AND MgGwsQueryEngine_LIBRARIES)
   # in cache already
   SET(MgGwsQueryEngine_FIND_QUIETLY TRUE)
ENDIF (MgGwsQueryEngine_INCLUDE_DIR AND MgGwsQueryEngine_LIBRARIES)

FIND_PATH(MgGwsQueryEngine_INCLUDE_DIR GwsQueryEngine.h
   HINTS
   ${MG_SRC_ROOT}/../Server/src/Gws/Include
   )

FIND_LIBRARY(MgGwsQueryEngine_LIBRARIES NAMES MgGwsQueryEngine
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Server/src/Gws/GwsQueryEngine/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgGwsQueryEngine DEFAULT_MSG
                                  MgGwsQueryEngine_INCLUDE_DIR MgGwsQueryEngine_LIBRARIES)
MARK_AS_ADVANCED(MgGwsQueryEngine_INCLUDE_DIR MgGwsQueryEngine_LIBRARIES)
