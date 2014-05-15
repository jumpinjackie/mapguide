# - Find MgGwsCommon component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgGwsCommon_INCLUDE_DIR AND MgGwsCommon_LIBRARIES)
   # in cache already
   SET(MgGwsCommon_FIND_QUIETLY TRUE)
ENDIF (MgGwsCommon_INCLUDE_DIR AND MgGwsCommon_LIBRARIES)

FIND_PATH(MgGwsCommon_INCLUDE_DIR GwsCommonImp.h
   HINTS
   ${MG_SRC_ROOT}/../Server/src/Gws/GwsCommon/inc
   )

FIND_LIBRARY(MgGwsCommon_LIBRARIES NAMES MgGwsCommon
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Server/src/Gws/GwsCommon/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgGwsCommon DEFAULT_MSG
                                  MgGwsCommon_INCLUDE_DIR MgGwsCommon_LIBRARIES)
MARK_AS_ADVANCED(MgGwsCommon_INCLUDE_DIR MgGwsCommon_LIBRARIES)
