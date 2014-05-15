# - Find MgRenderers component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgRenderers_INCLUDE_DIR AND MgRenderers_LIBRARIES)
   # in cache already
   SET(MgRenderers_FIND_QUIETLY TRUE)
ENDIF (MgRenderers_INCLUDE_DIR AND MgRenderers_LIBRARIES)

FIND_PATH(MgRenderers_INCLUDE_DIR Renderers.h
   HINTS
   ${MG_SRC_ROOT}/../Common/Renderers
   )

FIND_LIBRARY(MgRenderers_LIBRARIES NAMES MgRenderers
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgRenderers DEFAULT_MSG
                                  MgRenderers_INCLUDE_DIR MgRenderers_LIBRARIES)
MARK_AS_ADVANCED(MgRenderers_INCLUDE_DIR MgRenderers_LIBRARIES)
