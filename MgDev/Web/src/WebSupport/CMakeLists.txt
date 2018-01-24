include_directories(${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${ACE_INCLUDE_DIR}
)

set(MgWebSupport_SRCS
    InitializeWebTier.cpp
)

add_library(MgWebSupport${MG_VERSION_SUFFIX} SHARED ${MgWebSupport_SRCS})
install(TARGETS MgWebSupport${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgWebSupport${MG_VERSION_SUFFIX}.so libMgWebSupport.so ${MG_COMPONENT})

target_link_libraries(MgWebSupport${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
)