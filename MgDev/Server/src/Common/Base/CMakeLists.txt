include_directories(${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/PlatformBase
    ${CMAKE_CURRENT_SOURCE_DIR}/../Manager
    ${MG_SERVER_SERVICES_DIR}/Resource
    ${MG_SERVER_SERVICES_DIR}/Site
)

set (MgServerBase_SRCS
    ServerBaseBuild.cpp
)

add_library(MgServerBase${MG_VERSION_SUFFIX} SHARED ${MgServerBase_SRCS})
install(TARGETS MgServerBase${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerBase${MG_VERSION_SUFFIX}.so libMgServerBase.so ${MG_COMPONENT})

target_link_libraries(MgServerBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    MgServerManager${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
)