# - Find the FDO dir, include and library dirs

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )

set( FDO_VERSION 3.9.0 )
set( FDO_DIR /usr/local/fdo-${FDO_VERSION} )

if( FDO_INCLUDE_DIR AND FDO_LIBRARY_DIR)
        set( FDO_FIND_QUIETLY TRUE )
endif( FDO_INCLUDE_DIR AND FDO_LIBRARY_DIR )

FIND_PATH( FDO_INCLUDE_DIR Fdo.h ${FDO_DIR}/include )
FIND_LIBRARY( FDO_LIBRARY NAMES FDO PATHS ${FDO_DIR}/lib ${FDO_DIR}/lib64 )
FIND_LIBRARY( FDO_EXPRESSIONENGINE_LIBRARY NAMES ExpressionEngine PATHS ${FDO_DIR}/lib ${FDO_DIR}/lib64 )
FIND_PATH( FDO_LIBRARY_DIR libFDO.so ${FDO_DIR}/lib ${FDO_DIR}/lib64 )

if( FDO_INCLUDE_DIR AND FDO_LIBRARY AND FDO_LIBRARY_DIR )
        set(FDO_FOUND TRUE)
else( FDO_INCLUDE_DIR AND FDO_LIBRARY AND FDO_LIBRARY_DIR )
        set(FDO_FOUND FALSE)
endif( FDO_INCLUDE_DIR AND FDO_LIBRARY AND FDO_LIBRARY_DIR )

if( FDO_FOUND )
	if( NOT FDO_FIND_QUIETLY )
    	message( STATUS "Found FDO: ${FDO_LIBRARY}" )
  	endif( NOT FDO_FIND_QUIETLY )
  	mark_as_advanced( FDO_INCLUDE_DIR FDO_LIBRARY FDO_LIBRARY_DIR )
else( FDO_FOUND )
	if( FDO_FIND_REQUIRED )
    	message(FATAL_ERROR "Could NOT find FDO")
	endif (FDO_FIND_REQUIRED)
endif (FDO_FOUND)
