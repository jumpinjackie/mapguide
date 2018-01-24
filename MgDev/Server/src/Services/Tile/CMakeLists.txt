include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}   
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
)

set (MgServerTileService_SRCS
    ServerTileServiceBuild.cpp
)

add_library(MgServerTileService${MG_VERSION_SUFFIX} SHARED ${MgServerTileService_SRCS})
install(TARGETS MgServerTileService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerTileService${MG_VERSION_SUFFIX}.so libMgServerTileService.so ${MG_COMPONENT})