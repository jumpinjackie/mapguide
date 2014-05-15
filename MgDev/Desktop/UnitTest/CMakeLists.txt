find_package( Threads REQUIRED )
find_package( CppUnit REQUIRED )
find_package( ACE REQUIRED )
find_package( FDO REQUIRED )
find_package( DWFCore REQUIRED )
find_package( DWFToolkit REQUIRED )
find_package( DWFEMap REQUIRED )
find_package( MgGwsCommon REQUIRED )
find_package( MgGwsQueryEngine REQUIRED )
find_package( MgMdfModel REQUIRED )
find_package( MgMdfParser REQUIRED )
find_package( MgFoundation REQUIRED )
find_package( MgGeometry REQUIRED )
find_package( MgPlatformBase REQUIRED )
find_package( MgRenderers REQUIRED )
find_package( MgStylization REQUIRED )
find_package( MgSecurity REQUIRED )
find_package( XercesC REQUIRED )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../MgDesktop
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${CPPUNIT_INCLUDE_DIR}
    ${MgMdfModel_INCLUDE_DIR}
    ${MgMdfParser_INCLUDE_DIR}
    ${MgFoundation_INCLUDE_DIR}
    ${MgGeometry_INCLUDE_DIR}
    ${MgPlatformBase_INCLUDE_DIR}
    ${MgRenderers_INCLUDE_DIR}
    ${MgStylization_INCLUDE_DIR}
    ${ACE_INCLUDE_DIR}
    ${XercesC_INCLUDE_DIR}
    ${DWFCore_INCLUDE_DIR}
    ${DWFToolkit_INCLUDE_DIR}
    ${DWFToolkit_INCLUDE_DIR}/dwf
    ${DWFEMap_INCLUDE_DIR}
    ${MgGwsCommon_INCLUDE_DIR}    
    ${MgGwsQueryEngine_INCLUDE_DIR}
    ${MgGwsQueryEngine_INCLUDE_DIR}/../GwsQueryEngine/inc
    ${MgSecurity_INCLUDE_DIR}
)

set( MgDesktop_SRCS
    main.cpp
    TestFeatureService.cpp
    TestLogManager.cpp
    TestLogManagerThread.cpp
    TestMappingService.cpp
    TestProfilingService.cpp
    TestRenderingService.cpp
    TestResourceService.cpp
    TestTileService.cpp
)

add_executable(UnitTest ${MgDesktop_SRCS})

target_link_libraries(
    UnitTest
    MgDesktop-${MG_VERSION}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CPPUNIT_LIBRARIES}
    ${ACE_LIBRARIES}
    ${FDO_LIBRARY}
    ${FDO_EXPRESSIONENGINE_LIBRARY}
    ${DWFCore_LIBRARIES}
    ${DWFToolkit_LIBRARIES}
    ${DWFEMap_LIBRARIES}
    ${MgGwsCommon_LIBRARIES}
    ${MgGwsQueryEngine_LIBRARIES}
    ${MgMdfModel_LIBRARIES}
    ${MgMdfParser_LIBRARIES}
    ${MgFoundation_LIBRARIES}
    ${MgGeometry_LIBRARIES}
    ${MgPlatformBase_LIBRARIES}
    ${MgRenderers_LIBRARIES}
    ${MgStylization_LIBRARIES}
    ${MgSecurity_LIBRARIES}
    ${XercesC_LIBRARIES}
)

add_custom_command(TARGET UnitTest
    POST_BUILD
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/postbuild.sh ${MG_SRC_ROOT}
)

#install(TARGETS UnitTest DESTINATION ${INSTALL_PREFIX}/lib)
#install(CODE "execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/postinstall.sh)")
