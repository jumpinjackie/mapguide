project(mgdesktop)

#NOTE: DWF headers *must* go before MdfModel headers because we want Version.h from
#MdfModel first and not DWF Toolkit
include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/CoordinateSystem
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/Renderers
    ${MG_COMMON_DIR}/Stylization
    ${MG_OEM_DWF_INCLUDE_DIR}
    ${MG_OEM_DWF_INCLUDE_DIR}/dwf
    ${MG_SERVER_GWS_DIR}/Include
    ${MG_SERVER_GWS_DIR}/GwsCommon/inc
    ${MG_SERVER_GWS_DIR}/GwsQueryEngine/inc
)

set (MgDesktop_SRCS
    MgDesktopBuild.cpp
)

add_definitions(-DDWFCORE_STATIC -DDWFTK_STATIC -DDWFTK_USE_DWFCORE_ZLIB -DDWFTK_BUILD_EXPAT -DWHIP_STATIC_LIB)

add_library(MgDesktop${MG_VERSION_SUFFIX} SHARED ${MgDesktop_SRCS})
install(TARGETS MgDesktop${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgDesktop${MG_VERSION_SUFFIX}.so libMgDesktop.so ${MG_COMPONENT})

target_link_libraries(MgDesktop${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    ${XERCESC_LIBRARIES}
    ${FDO_LIBRARY}
    ${FDO_EXPRESSIONENGINE_LIBRARY}
    dwfcore
    dwftk
    dwfwhiptk
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgSecurity${MG_VERSION_SUFFIX}
    MgRenderers${MG_VERSION_SUFFIX}
    MgStylization${MG_VERSION_SUFFIX}
    MgGwsCommon${MG_VERSION_SUFFIX}
    MgGwsQueryEngine${MG_VERSION_SUFFIX}
    pthread
)