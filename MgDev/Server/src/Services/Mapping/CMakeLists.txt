include_directories(${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Stylization
    ${MG_COMMON_DIR}/Renderers
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_COMMON_DIR}/Cache
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${XERCESC_INCLUDE_DIR}
    ${MG_COMMON_DIR}/CoordinateSystem
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_GWS_DIR}/Include
)

add_definitions(-DDWFTK_BUILD_EXPAT -DDWFTK_USE_DWFCORE_ZLIB)

set (MgServerMappingService_SRCS
    ServerMappingServiceBuild.cpp
)

add_library(MgServerMappingService${MG_VERSION_SUFFIX} SHARED ${MgServerMappingService_SRCS})
install(TARGETS MgServerMappingService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerMappingService${MG_VERSION_SUFFIX}.so libMgServerMappingService.so ${MG_COMPONENT})