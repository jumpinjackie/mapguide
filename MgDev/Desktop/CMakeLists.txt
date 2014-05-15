project(mgdesktop)

# CMake base requires and policy tweaks
cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy( SET CMP0005 OLD )

# Look first in internal modules
set(CMAKE_MODULE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
	)

# Make CPack available to easy generate binary packages
include(CPack)

# Set MapGuide version macros

set( MG_VERSION_MAJOR 2 )
set( MG_VERSION_MINOR 6 )
set( MG_VERSION_RELEASE 0 )
set( MG_VERSION "${MG_VERSION_MAJOR}.${MG_VERSION_MINOR}.${MG_VERSION_RELEASE}" )
set( MG_DISTRIBUTION_TEXT "Self compiled sources" CACHE STRING "Indicate compilation origin" )
set( MG_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )

set( MG_ROOT_DIR "/usr/local/mapguideopensource-${MG_VERSION}")
set( MG_LIB_DIR "${MG_ROOT_DIR}/lib" )
set( MG_COORDSYS_DIR "${MG_ROOT_DIR}/share/gis/coordsys" )
# Override install directory to match what the automake build does (/usr/local, rather than /usr/local/lib)
set( INSTALL_PREFIX "${MG_ROOT_DIR}/desktop" )

if( UNIX )
	add_definitions( -DFULLPROTO -fPIC -DPIC )
endif( UNIX )

if (CMAKE_SYSTEM_NAME MATCHES Linux)
	add_definitions( -DLINUX -DLINUX_IA32 -pthread )
endif (CMAKE_SYSTEM_NAME MATCHES Linux)

if( CMAKE_COMPILER_IS_GNUCXX )
	add_definitions( -D__USE_GNU -Wno-deprecated -Wno-write-strings -Wno-format )
	set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
	set ( CMAKE_CXX_FLAGS "-O2 -march=i686" )
endif( CMAKE_COMPILER_IS_GNUCXX )

add_subdirectory( MgDesktop )
#add_subdirectory( DesktopUnmanagedApi )
add_subdirectory( UnitTest )

