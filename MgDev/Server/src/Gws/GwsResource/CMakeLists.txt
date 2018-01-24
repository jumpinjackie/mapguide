include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Include)

set(GwsResource_SRCS
    GwsResourceLinux.cpp
)

add_library(MgGwsResource${MG_VERSION_SUFFIX} SHARED ${GwsResource_SRCS})
install(TARGETS MgGwsResource${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgGwsResource${MG_VERSION_SUFFIX}.so libMgGwsResource.so ${MG_COMPONENT})