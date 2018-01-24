include_directories(${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_SERVER_SERVICES_DIR}/Resource
    ${MG_SERVER_SERVICES_DIR}/Site
)

set (MgServerThread_SRCS
    ServerThreadBuild.cpp
)

add_library(MgServerThread${MG_VERSION_SUFFIX} SHARED ${MgServerThread_SRCS})
install(TARGETS MgServerThread${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerThread${MG_VERSION_SUFFIX}.so libMgServerThread.so ${MG_COMPONENT})

target_link_libraries(MgServerThread${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
)