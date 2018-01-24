if (CMAKE_COMPILER_IS_GNUCXX)
    message ("Restoring default linker flags for mod_mgmapagent")
    set ( CMAKE_SHARED_LINKER_FLAGS "${WEB_CMAKE_SHARED_LINKER_FLAGS}")
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/MdfModel
    ${ACE_INCLUDE_DIR}
    ${HTTPD_INCLUDE_DIR}/include
    ${HTTPD_INCLUDE_DIR}/srclib/apr/include
    ${HTTPD_INCLUDE_DIR}/srclib/apr-util/include
    ${HTTPD_INCLUDE_DIR}/os/unix
    ${CMAKE_CURRENT_SOURCE_DIR}/../HttpHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/../WebSupport
    ${CMAKE_CURRENT_SOURCE_DIR}/../MapAgentCommon
)

set(mod_mgmapagent_SRCS
    ApacheAgent.cpp
    ApachePostParser.cpp
    ApacheReaderStreamer.cpp
    ApacheResponseHandler.cpp
    ../MapAgentCommon/MapAgentCommon.cpp
    ../MapAgentCommon/MapAgentGetParser.cpp
    ../MapAgentCommon/MapAgentStrings.cpp
    ../WebSupport/InitializeWebTier.cpp
)

add_library(mod_mgmapagent${MG_VERSION_SUFFIX} SHARED ${mod_mgmapagent_SRCS})
add_dependencies(mod_mgmapagent${MG_VERSION_SUFFIX} httpd)
install(TARGETS mod_mgmapagent${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
# Not using install_symlink macro here as install requirements are unique
# The link target doesn't have to exist at this point in time, the symlink will be valid at install time
execute_process(COMMAND ln -sf ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/mod_mgmapagent${MG_VERSION_SUFFIX}.so ${CMAKE_CURRENT_BINARY_DIR}/mod_mgmapagent.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mod_mgmapagent.so DESTINATION ${MG_INSTALL_WEB_PREFIX}/apache2/modules COMPONENT ${MG_COMPONENT})

target_link_libraries(mod_mgmapagent${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgHttpHandler${MG_VERSION_SUFFIX}
    MgWebSupport${MG_VERSION_SUFFIX}
)

# Make sure this doesn't have the "lib" prefix
set_target_properties(mod_mgmapagent${MG_VERSION_SUFFIX} PROPERTIES PREFIX "")