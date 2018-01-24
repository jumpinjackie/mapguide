include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../MgDesktop
    ${CMAKE_CURRENT_SOURCE_DIR}/../MgDesktop/Services
    ${CPPUNIT_INCLUDE_DIR}
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/CoordinateSystem
    ${MG_COMMON_DIR}/Security
    ${MG_COMMON_DIR}/MdfParser
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/Stylization
)

set (MgDesktopUnitTest_SRCS
    main.cpp
    TestFeatureService.cpp
    #TestKmlService.cpp
    TestLogManager.cpp
    TestLogManagerThread.cpp
    TestMappingService.cpp
    #TestPerformance.cpp
    TestProfilingService.cpp
    TestRenderingService.cpp
    TestResourceService.cpp
    TestTileService.cpp
)

add_executable(MgDesktopUnitTest ${MgDesktopUnitTest_SRCS})
add_sanitizers(MgDesktopUnitTest)
set_target_properties( MgDesktopUnitTest
    PROPERTIES OUTPUT_NAME "UnitTest" )

target_link_libraries(MgDesktopUnitTest
    ${ACE_LIBRARY}
    ${FDO_LIBRARY}
    ${XERCESC_LIBRARIES}
    ${CPPUNIT_LIBRARIES}
    MgMdfParser${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgSecurity${MG_VERSION_SUFFIX}
    MgDesktop${MG_VERSION_SUFFIX}
)