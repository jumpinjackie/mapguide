# - Find MgMdfModel component

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )
cmake_policy( SET CMP0011 OLD )

IF (MgMdfModel_INCLUDE_DIR AND MgMdfModel_LIBRARIES)
   # in cache already
   SET(MgMdfModel_FIND_QUIETLY TRUE)
ENDIF (MgMdfModel_INCLUDE_DIR AND MgMdfModel_LIBRARIES)

FIND_PATH(MgMdfModel_INCLUDE_DIR MdfModel.h
   HINTS
   ${MG_SRC_ROOT}/../Common/MdfModel
   )

FIND_LIBRARY(MgMdfModel_LIBRARIES NAMES MgMdfModel
   HINTS
   ${MG_LIB_DIR}
   ${MG_SRC_ROOT}/../Common/.libs
   )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MgMdfModel DEFAULT_MSG
                                  MgMdfModel_INCLUDE_DIR MgMdfModel_LIBRARIES)
MARK_AS_ADVANCED(MgMdfModel_INCLUDE_DIR MgMdfModel_LIBRARIES)
