include_directories(${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Manager
)

set (MgServerAdminService_SRCS
    ServerAdminServiceBuild.cpp
)

add_library(MgServerAdminService${MG_VERSION_SUFFIX} SHARED ${MgServerAdminService_SRCS})
install(TARGETS MgServerAdminService${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerAdminService${MG_VERSION_SUFFIX}.so libMgServerAdminService.so ${MG_COMPONENT})