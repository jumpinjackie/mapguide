include_directories(${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_SERVICES_DIR}/Resource
)

set (MgServerSiteService_SRCS
    ServerSiteServiceBuild.cpp
)

add_library(MgServerSiteService${MG_VERSION_SUFFIX} SHARED ${MgServerSiteService_SRCS})
install(TARGETS MgServerSiteService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerSiteService${MG_VERSION_SUFFIX}.so libMgServerSiteService.so ${MG_COMPONENT})