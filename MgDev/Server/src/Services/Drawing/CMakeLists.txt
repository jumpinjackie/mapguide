include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${MG_OEM_DWF_INCLUDE_DIR}
    ${MG_OEM_DWF_INCLUDE_DIR}/dwf
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_SERVICES_DIR}/Resource
)

add_definitions(-DDWFTK_BUILD_EXPAT -DDWFTK_USE_DWFCORE_ZLIB)

set (MgServerDrawingService_SRCS
    ServerDrawingServiceBuild.cpp
)

add_library(MgServerDrawingService${MG_VERSION_SUFFIX} SHARED ${MgServerDrawingService_SRCS})
install(TARGETS MgServerDrawingService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerDrawingService${MG_VERSION_SUFFIX}.so libMgServerDrawingService.so ${MG_COMPONENT})