include_directories(${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${MG_OEM_DWF_INCLUDE_DIR}
    ${DBXML_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Cache
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Site
)

set (MgServerResourceService_SRCS
    ServerResourceServiceBuild.cpp
)

add_library(MgServerResourceService${MG_VERSION_SUFFIX} SHARED ${MgServerResourceService_SRCS})
target_link_libraries(MgServerResourceService${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    MgServerBase${MG_VERSION_SUFFIX}
    MgServerCache${MG_VERSION_SUFFIX}
    MgServerManager${MG_VERSION_SUFFIX}
    ${DB_CXX_LIBRARY}
    ${DBXML_LIBRARY}
    ${XQILLA_LIBRARY}
    ${XERCESC_LIBRARIES}
)
install(TARGETS MgServerResourceService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerResourceService${MG_VERSION_SUFFIX}.so libMgServerResourceService.so ${MG_COMPONENT})