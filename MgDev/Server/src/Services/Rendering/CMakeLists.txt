include_directories(${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Cache
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Stylization
    ${MG_COMMON_DIR}/Renderers
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Mapping
)

set (MgServerRenderingService_SRCS
    ServerRenderingServiceBuild.cpp
)

add_library(MgServerRenderingService${MG_VERSION_SUFFIX} SHARED ${MgServerRenderingService_SRCS})
install(TARGETS MgServerRenderingService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerRenderingService${MG_VERSION_SUFFIX}.so libMgServerRenderingService.so ${MG_COMPONENT})