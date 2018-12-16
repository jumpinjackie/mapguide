include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}
    ${MG_AUTO_GENERATED_HEADER_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_SERVER_SERVICES_DIR}/Drawing
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Kml
    ${MG_SERVER_SERVICES_DIR}/Mapping
    ${MG_SERVER_SERVICES_DIR}/Profiling
    ${MG_SERVER_SERVICES_DIR}/Rendering
    ${MG_SERVER_SERVICES_DIR}/Resource
    ${MG_SERVER_SERVICES_DIR}/ServerAdmin
    ${MG_SERVER_SERVICES_DIR}/Site
    ${MG_SERVER_SERVICES_DIR}/Tile
    ${CMAKE_CURRENT_SOURCE_DIR}/../Cache
    ${CMAKE_CURRENT_SOURCE_DIR}/../Thread
)

set (MgServerManager_SRCS
    ServerManagerBuild.cpp
)

add_library(MgServerManager${MG_VERSION_SUFFIX} SHARED ${MgServerManager_SRCS})
install(TARGETS MgServerManager${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerManager${MG_VERSION_SUFFIX}.so libMgServerManager.so ${MG_COMPONENT})

target_link_libraries(MgServerManager${MG_VERSION_SUFFIX}
    ${FDO_LIBRARY}
    ${ACE_LIBRARY}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    MgServerThread${MG_VERSION_SUFFIX}
    MgServerCache${MG_VERSION_SUFFIX}
)