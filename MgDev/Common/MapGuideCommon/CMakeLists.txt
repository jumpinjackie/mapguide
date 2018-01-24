project(mapguidecommon)

include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../PlatformBase
    ${CMAKE_CURRENT_SOURCE_DIR}/../CoordinateSystem
    ${CMAKE_CURRENT_SOURCE_DIR}/../Security
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfParser
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfModel
    ${CMAKE_CURRENT_SOURCE_DIR}/../Foundation
    ${CMAKE_CURRENT_SOURCE_DIR}/../Geometry
)

set (MgMapGuideCommon_SRCS
    MapGuideCommonBuild.cpp
)

add_library(MgMapGuideCommon${MG_VERSION_SUFFIX} SHARED ${MgMapGuideCommon_SRCS})

target_link_libraries(MgMapGuideCommon${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgSecurity${MG_VERSION_SUFFIX}
)

install(TARGETS MgMapGuideCommon${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgMapGuideCommon${MG_VERSION_SUFFIX}.so libMgMapGuideCommon.so ${MG_COMPONENT})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${MG_WWWROOT} COMPONENT ${MG_COMPONENT})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${MG_INSTALL_SERVER_PREFIX} COMPONENT ${MG_COMPONENT})