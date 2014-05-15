# - Find MgSecurity component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgSecurity_INCLUDE_DIR AND MgSecurity_LIBRARIES)
   # in cache already
   SET(MgSecurity_FIND_QUIETLY TRUE)
ENDIF (MgSecurity_INCLUDE_DIR AND MgSecurity_LIBRARIES)

FIND_PATH(MgSecurity_INCLUDE_DIR CryptographyUtil.h
   HINTS
   ${MG_SRC_ROOT}/../Common/Security
   )

FIND_LIBRARY(MgSecurity_LIBRARIES NAMES MgSecurity
   HINTS
   ${MG_SRC_ROOT}/../Common/Security
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgSecurity DEFAULT_MSG
                                  MgSecurity_INCLUDE_DIR MgSecurity_LIBRARIES)
MARK_AS_ADVANCED(MgSecurity_INCLUDE_DIR MgSecurity_LIBRARIES)
