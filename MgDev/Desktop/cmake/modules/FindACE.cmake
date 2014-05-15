# - Find ACE component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (ACE_INCLUDE_DIR AND ACE_LIBRARIES)
   # in cache already
   SET(ACE_FIND_QUIETLY TRUE)
ENDIF (ACE_INCLUDE_DIR AND ACE_LIBRARIES)

FIND_PATH(ACE_INCLUDE_DIR ace/INET_Addr.h
   HINTS
   ${MG_SRC_ROOT}/../Oem/ACE/ACE_wrappers
   )

FIND_LIBRARY(ACE_LIBRARIES NAMES ACE
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Oem/ACE/ACE_wrappers/lib
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ACE DEFAULT_MSG
                                  ACE_INCLUDE_DIR ACE_LIBRARIES )
MARK_AS_ADVANCED(ACE_INCLUDE_DIR ACE_LIBRARIES)
