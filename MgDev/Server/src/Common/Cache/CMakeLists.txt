include_directories(${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/PlatformBase
)

set (MgServerCache_SRCS
    ServerCacheBuild.cpp
)

add_library(MgServerCache${MG_VERSION_SUFFIX} SHARED ${MgServerCache_SRCS})
install(TARGETS MgServerCache${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgServerCache${MG_VERSION_SUFFIX}.so libMgServerCache.so ${MG_COMPONENT})

target_link_libraries(MgServerCache${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
)