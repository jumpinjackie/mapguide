include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${MG_OEM_DWF_INCLUDE_DIR}
    ${MG_COMMON_DIR}/CoordinateSystem
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Renderers
    ${MG_COMMON_DIR}/Stylization
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Mapping
)

add_definitions(-DDWFTK_USE_DWFCORE_ZLIB)

set (MgServerKmlService_SRCS
    ServerKmlServiceBuild.cpp
)

add_library(MgServerKmlService${MG_VERSION_SUFFIX} SHARED ${MgServerKmlService_SRCS})
install(TARGETS MgServerKmlService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerKmlService${MG_VERSION_SUFFIX}.so libMgServerKmlService.so ${MG_COMPONENT})