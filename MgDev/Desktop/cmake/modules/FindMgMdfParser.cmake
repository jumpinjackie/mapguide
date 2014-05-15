# - Find MgMdfParser component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgMdfParser_INCLUDE_DIR AND MgMdfParser_LIBRARIES)
   # in cache already
   SET(MgMdfParser_FIND_QUIETLY TRUE)
ENDIF (MgMdfParser_INCLUDE_DIR AND MgMdfParser_LIBRARIES)

FIND_PATH(MgMdfParser_INCLUDE_DIR MdfParser.h
   HINTS
   ${MG_SRC_ROOT}/../Common/MdfParser
   )

FIND_LIBRARY(MgMdfParser_LIBRARIES NAMES MgMdfParser
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgMdfParser DEFAULT_MSG
                                  MgMdfParser_INCLUDE_DIR MgMdfParser_LIBRARIES)
MARK_AS_ADVANCED(MgMdfParser_INCLUDE_DIR MgMdfParser_LIBRARIES)
