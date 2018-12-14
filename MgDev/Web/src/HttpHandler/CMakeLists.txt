find_package (JsonCpp REQUIRED)

include_directories(${JsonCpp_INCLUDE_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
)

set(MgHttpHandler_SRCS
    HttpHandlerBuild.cpp
)

add_library(MgHttpHandler${MG_VERSION_SUFFIX} SHARED ${MgHttpHandler_SRCS})
install(TARGETS MgHttpHandler${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgHttpHandler${MG_VERSION_SUFFIX}.so libMgHttpHandler.so ${MG_COMPONENT})

target_link_libraries(MgHttpHandler${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    ${JsonCpp_LIBRARY}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
)