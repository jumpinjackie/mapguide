include_directories(${ACE_INCLUDE_DIR}
    ${CPPUNIT_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/Stylization
    ${MG_SERVER_COMMON_DIR}
    ${MG_SERVER_COMMON_DIR}/Manager
    ${MG_SERVER_SERVICES_DIR}/Feature
    ${MG_SERVER_SERVICES_DIR}/Mapping
    ${MG_SERVER_SERVICES_DIR}/Rendering
    ${MG_SERVER_SERVICES_DIR}/Resource
    ${MG_SERVER_SERVICES_DIR}/ServerAdmin
    ${MG_SERVER_SERVICES_DIR}/Site
    ${MG_SERVER_SERVICES_DIR}/Kml
    ${MG_SERVER_SERVICES_DIR}/Tile
    ${FDO_INCLUDE_DIR}
)

set(MgUnitTesting_SRCS
    TestCoordinateSystem.cpp
    TestDrawingService.cpp
    TestFeatureService.cpp
    TestGeometry.cpp
    TestKmlService.cpp
    TestLogManager.cpp
    TestLogManagerThread.cpp
    TestMappingService.cpp
    TestMdfModel.cpp
    TestPerformance.cpp
    TestRenderingService.cpp
    TestResourceService.cpp
    TestServerAdminService.cpp
    TestServerManager.cpp
    TestServiceManager.cpp
    TestSiteManager.cpp
    TestSiteService.cpp
    TestTileService.cpp
    TestProfilingService.cpp
    TestTransformMesh.cpp
    UnitTesting.cpp
)

add_library(MgUnitTesting SHARED ${MgUnitTesting_SRCS})
install(TARGETS MgUnitTesting DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})

target_link_libraries(MgUnitTesting
    ${CPPUNIT_LIBRARIES}
)