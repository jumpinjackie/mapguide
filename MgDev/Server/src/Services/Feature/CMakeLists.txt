include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Stylization
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Cache
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_GWS_DIR}/Include
    ${MG_SERVER_GWS_DIR}/GwsCommon/inc
    ${MG_SERVER_GWS_DIR}/GwsQueryEngine/inc
)

set (MgServerFeatureService_SRCS
    ServerFeatureServiceBuild.cpp
)

add_library(MgServerFeatureService${MG_VERSION_SUFFIX} SHARED ${MgServerFeatureService_SRCS})
install(TARGETS MgServerFeatureService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerFeatureService${MG_VERSION_SUFFIX}.so libMgServerFeatureService.so ${MG_COMPONENT})

target_link_libraries(MgServerFeatureService${MG_VERSION_SUFFIX}
    MgGwsCommon${MG_VERSION_SUFFIX}
    MgGwsQueryEngine${MG_VERSION_SUFFIX}
)