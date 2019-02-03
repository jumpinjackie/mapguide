# NOTE: Not applied for server tier projects except for mgserver main executable
if( CMAKE_COMPILER_IS_GNUCXX )
    if (NEED_LIBRT)
        set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed -Wl,--no-undefined -lc -lrt ${CMAKE_SHARED_LINKER_FLAGS}")
    else (NEED_LIBRT)
        set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    endif (NEED_LIBRT)
endif( CMAKE_COMPILER_IS_GNUCXX )

# On ubuntu at least, the system installed ACE does not define this (required by us)
if (NOT INTERNAL_ACE)
    add_definitions( -DACE_HAS_THREAD_SAFE_ACCEPT )
endif (NOT INTERNAL_ACE)

set(MG_COMPONENT ${MG_COMPONENT_COMMON})
set_install_subdir("")

add_subdirectory(Foundation)
add_subdirectory(Geometry)
add_subdirectory(MapGuideCommon)
add_subdirectory(MdfModel)
add_subdirectory(MdfParser)
add_subdirectory(PlatformBase)
add_subdirectory(Renderers)
add_subdirectory(Security)
add_subdirectory(Stylization)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Schema DESTINATION ${MG_INSTALL_SERVER_PREFIX} COMPONENT ${MG_COMPONENT_SERVER})

if (INTERNAL_XERCES)
    file(GLOB XERCES_LIBS "${MG_OEM_WORK_DIR}/dbxml/install/lib/libxerces-c*.so")
    install(FILES ${XERCES_LIBS} DESTINATION ${LIB_INSTALL_DIR})
endif (INTERNAL_XERCES)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../Oem/DWFTK/develop/global/src/dwfcore/libdwfcore-1.7.0.so DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../Oem/DWFTK/develop/global/src/dwf/libdwftk-7.7.0.so DESTINATION ${LIB_INSTALL_DIR})

configure_file(${CMAKE_MODULE_PATH}/../configs/ProductVersion.h.in ${MG_AUTO_GENERATED_HEADER_DIR}/ProductVersion.h)