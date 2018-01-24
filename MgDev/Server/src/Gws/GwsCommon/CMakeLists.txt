include_directories(${FDO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../GwsResource
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${ACE_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
)

set(GwsCommon_SRCS
    GwsCommonFactories.cpp
    GwsCommonFdoUtils.cpp
    GwsCsName.cpp
    GwsDebugUtils.cpp
    GwsException.cpp
    GwsExtendedFeatureId.cpp
    GwsFeatureId.cpp
    GwsObjectImpl.cpp
    GwsQualifiedName.cpp
    GwsQualifiedNames.cpp
    GwsResourceUtil.cpp
    GwsXmlUtils.cpp
    IGwsException.cpp
)

add_library(MgGwsCommon${MG_VERSION_SUFFIX} SHARED ${GwsCommon_SRCS})
install(TARGETS MgGwsCommon${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgGwsCommon${MG_VERSION_SUFFIX}.so libMgGwsCommon.so ${MG_COMPONENT})