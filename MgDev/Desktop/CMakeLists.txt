# On ubuntu at least, the system installed ACE does not define this (required by us)
if (NOT INTERNAL_ACE)
    add_definitions( -DACE_HAS_THREAD_SAFE_ACCEPT )
endif (NOT INTERNAL_ACE)

# NOTE: Not applied for server tier projects except for mgserver main executable
if( CMAKE_COMPILER_IS_GNUCXX )
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
endif( CMAKE_COMPILER_IS_GNUCXX )

set(MG_COMPONENT "portable")
set_install_subdir("portable/")

add_subdirectory(MgDesktop)
add_subdirectory(UnitTest)