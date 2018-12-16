project(platformbase)

include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../CoordinateSystem
    ${CMAKE_CURRENT_SOURCE_DIR}/../Security
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfParser
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfModel
    ${CMAKE_CURRENT_SOURCE_DIR}/../Foundation
    ${CMAKE_CURRENT_SOURCE_DIR}/../Geometry
)

set (MgPlatformBase_SRCS
    PlatformBaseBuild.cpp
)

add_library(MgPlatformBase${MG_VERSION_SUFFIX} SHARED ${MgPlatformBase_SRCS})

target_link_libraries(MgPlatformBase${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgSecurity${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
)

install( TARGETS MgPlatformBase${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT} )
install_symlink( ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgPlatformBase${MG_VERSION_SUFFIX}.so libMgPlatformBase.so ${MG_COMPONENT} )