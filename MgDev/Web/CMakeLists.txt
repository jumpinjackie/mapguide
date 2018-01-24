add_definitions( -DACE_HAS_THREAD_SAFE_ACCEPT )

# NOTE: Not applied for server tier projects except for mgserver main executable
if( CMAKE_COMPILER_IS_GNUCXX )
    # Stash a copy of current shared linker flags in case any project needs to build
    # with undefined symbols
    set ( WEB_CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}" )
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
endif( CMAKE_COMPILER_IS_GNUCXX )

set(MG_COMPONENT ${MG_COMPONENT_WEB})
set_install_subdir("webserverextensions/")

add_subdirectory(src)