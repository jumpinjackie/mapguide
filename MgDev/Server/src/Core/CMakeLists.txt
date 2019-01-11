if( CMAKE_COMPILER_IS_GNUCXX )
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
endif( CMAKE_COMPILER_IS_GNUCXX )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${DBXML_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${MG_AUTO_GENERATED_HEADER_DIR}
    ${MG_COMMON_DIR}/CoordinateSystem
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/Renderers
    ${MG_COMMON_DIR}/Stylization
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_SERVER_GWS_DIR}/Include
    ${MG_SERVER_GWS_DIR}/GwsCommon/inc
    ${MG_SERVER_GWS_DIR}/GwsQueryEngine/inc
    ${MG_SERVER_COMMON_DIR}/Base
    ${MG_SERVER_COMMON_DIR}/Cache
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_COMMON_DIR}/Thread
    ${MG_SERVER_SERVICES_DIR}/Drawing
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Kml
    ${MG_SERVER_SERVICES_DIR}/Mapping
    ${MG_SERVER_SERVICES_DIR}/Profiling
    ${MG_SERVER_SERVICES_DIR}/Rendering
    ${MG_SERVER_SERVICES_DIR}/Resource
    ${MG_SERVER_SERVICES_DIR}/ServerAdmin
    ${MG_SERVER_SERVICES_DIR}/Site
    ${MG_SERVER_SERVICES_DIR}/Tile
)

add_definitions(-DACE_HAS_SVC_DLL=1)

set (mgserver_SRCS
    ServerCoreBuild.cpp
)

add_executable(mgserver ${mgserver_SRCS})
add_sanitizers(mgserver)
target_link_libraries(mgserver
    ${ACE_LIBRARY}
    ${DB_CXX_LIBRARY}
    ${DBXML_LIBRARY}
    ${XERCESC_LIBRARIES}
    ${FDO_LIBRARY}
    ${FDO_EXPRESSIONENGINE_LIBRARY}
    MgGwsCommon${MG_VERSION_SUFFIX}
    MgGwsQueryEngine${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    MgServerAdminService${MG_VERSION_SUFFIX}
    MgServerBase${MG_VERSION_SUFFIX}
    MgServerCache${MG_VERSION_SUFFIX}
    MgServerDrawingService${MG_VERSION_SUFFIX}
    MgServerFeatureService${MG_VERSION_SUFFIX}
    MgServerKmlService${MG_VERSION_SUFFIX}
    MgServerManager${MG_VERSION_SUFFIX}
    MgServerMappingService${MG_VERSION_SUFFIX}
    MgServerProfilingService${MG_VERSION_SUFFIX}
    MgServerRenderingService${MG_VERSION_SUFFIX}
    MgServerResourceService${MG_VERSION_SUFFIX}
    MgServerSiteService${MG_VERSION_SUFFIX}
    MgServerThread${MG_VERSION_SUFFIX}
    MgServerTileService${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgStylization${MG_VERSION_SUFFIX}
    MgRenderers${MG_VERSION_SUFFIX}
)

########### BEGIN Development/Testing environment setup #############
file(GLOB_RECURSE UNIT_TEST_FILES "${MG_UNIT_TEST_DATA_DIR}/*.*")
file(COPY ${UNIT_TEST_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../UnitTestFiles)
# Awkward, can't copy a file to a target file name
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../UnitTestFiles/Shuttle.zip DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../UnitTestFilesTemp)
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/../UnitTestFilesTemp/Shuttle.zip ${CMAKE_CURRENT_BINARY_DIR}/../UnitTestFiles/World.mgp)

# Symlink the built copy to facilitate inner loop testing/debugging
# The actual symlink installation is done in the UnitTesting CMakeLists.txt
execute_process(COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/../UnitTesting/libMgUnitTesting.so ${CMAKE_CURRENT_BINARY_DIR}/libMgUnitTesting.so)
set(ORIG_MG_INSTALL_SERVER_PREFIX_ABS "${MG_INSTALL_SERVER_PREFIX_ABS}")
set(ORIG_MG_INSTALL_COORDSYS_PREFIX_ABS "${MG_INSTALL_COORDSYS_PREFIX_ABS}")
set(MG_INSTALL_SERVER_PREFIX_ABS "${CMAKE_CURRENT_BINARY_DIR}")
set(MG_INSTALL_COORDSYS_PREFIX_ABS "${MG_OEM_ROOT_BUILD_DIR}/CsMapLibrary/CsMap_Install/Dictionaries")
configure_file(${CMAKE_MODULE_PATH}/../configs/serverconfig.ini.in ${CMAKE_CURRENT_BINARY_DIR}/serverconfig.ini)
set(MG_INSTALL_SERVER_PREFIX_ABS "${ORIG_MG_INSTALL_SERVER_PREFIX_ABS}")
set(MG_INSTALL_COORDSYS_PREFIX_ABS "${ORIG_MG_INSTALL_COORDSYS_PREFIX_ABS}")
file(COPY ${MG_COMMON_DIR}/MapGuideCommon/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${MG_COMMON_DIR}/Schema DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

########### END Development/Testing environment setup #############

install(TARGETS mgserver DESTINATION ${MG_SERVER_BIN_DIR} COMPONENT ${MG_COMPONENT})
if (UNIX)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mapguidectl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND sed -i "s#/usr/local/mapguideopensource/server#${MG_INSTALL_SERVER_PREFIX}#g" ${CMAKE_CURRENT_BINARY_DIR}/mapguidectl)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/mapguidectl DESTINATION ${MG_SERVER_BIN_DIR} COMPONENT ${MG_COMPONENT})
endif (UNIX)

configure_file(${CMAKE_MODULE_PATH}/../configs/serverconfig.ini.in ${CMAKE_CURRENT_BINARY_DIR}/staging/serverconfig.ini)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/staging/serverconfig.ini DESTINATION ${MG_SERVER_BIN_DIR} COMPONENT ${MG_COMPONENT})
configure_file(${CMAKE_MODULE_PATH}/../configs/mgserver.sh.in ${CMAKE_CURRENT_BINARY_DIR}/mgserver.sh)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/mgserver.sh DESTINATION ${MG_SERVER_BIN_DIR} COMPONENT ${MG_COMPONENT})
configure_file(${CMAKE_MODULE_PATH}/../configs/mgserverd.sh.in ${CMAKE_CURRENT_BINARY_DIR}/mgserverd.sh)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/mgserverd.sh DESTINATION ${MG_SERVER_BIN_DIR} COMPONENT ${MG_COMPONENT})
file(GLOB DBXML_LIBS "${MG_OEM_WORK_DIR}/dbxml/install/lib/libdb*.so")
install(FILES ${DBXML_LIBS} DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${MG_OEM_WORK_DIR}/dbxml/xqilla/build/.libs/libxqilla.so DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${MG_OEM_WORK_DIR}/dbxml/xqilla/build/.libs/libxqilla.so.5 DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${MG_OEM_WORK_DIR}/dbxml/xqilla/build/.libs/libxqilla.so.5.0.4 DESTINATION ${LIB_INSTALL_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/dbxml DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/dbxml_dump DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/dbxml_load DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_archive DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_checkpoint DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_dump DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_hotbackup DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_load DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_recover DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_upgrade DESTINATION ${MG_SERVER_BIN_DIR})
install(PROGRAMS ${MG_OEM_WORK_DIR}/dbxml/install/bin/db_verify DESTINATION ${MG_SERVER_BIN_DIR})
# When installed into $PREFIX/server/bin, it will auto find libraries under $PREFIX/lib and $PREFIX/server/lib
if(MG_CPU EQUAL 64)
    set_target_properties(mgserver PROPERTIES INSTALL_RPATH "$ORIGIN/../lib64:$ORIGIN/../../lib64")
else(MG_CPU EQUAL 64)
    set_target_properties(mgserver PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../../lib")
endif(MG_CPU EQUAL 64)
