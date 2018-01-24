if (CMAKE_COMPILER_IS_GNUCXX)
    message ("Restoring default linker flags for libphp_MapGuideApi")
    set ( CMAKE_SHARED_LINKER_FLAGS "${WEB_CMAKE_SHARED_LINKER_FLAGS}")
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${PHP_INCLUDE_DIR}/TSRM
    ${PHP_INCLUDE_DIR}/Zend
    ${PHP_INCLUDE_DIR}/main
    ${PHP_INCLUDE_DIR}
    ${ACE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../HttpHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/../WebSupport
    ${CMAKE_CURRENT_SOURCE_DIR}/../WebApp
    ${XERCESC_INCLUDE_DIR}
)

add_definitions(-DPHP -DPHPPROXYAPI_EXPORTS)
add_definitions(-fno-var-tracking-assignments)

set(php_MapGuideApi_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/MgApi_wrap.cpp
)

set(DISPOSE_EXPR "((MgDisposable*)arg1)->Release()")
set(RETHROW_EXPR "e->Raise()")

add_custom_command(OUTPUT ${php_MapGuideApi_SRCS}
    COMMAND ${IMAKE_TOOL} -p ../MapGuideApi/MapGuideApiGen.xml
            -l PHP
            -r ${CMAKE_CURRENT_SOURCE_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/php.i
            ${CMAKE_CURRENT_BINARY_DIR}/language.i
    COMMAND ${IMAKE_TOOL} -p ${CMAKE_CURRENT_BINARY_DIR}/../MapGuideApi/Constants.xml 
            -l PHP 
            -r ${CMAKE_CURRENT_SOURCE_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}/constants.php
            -t
    COMMAND ${SWIG_TOOL} -c++ -php -DPHPSWIG
            -baseexception MgException -clsidcode getclassid.code -clsiddata m_cls_id 
            -catchallcode catchall.code 
            -dispose \"${DISPOSE_EXPR}\" 
            -rethrow \"${RETHROW_EXPR}\;\" 
            -nodefault
            -o ${php_MapGuideApi_SRCS} 
            -lib ${SWIG_LIB_PATH} 
            ${CMAKE_CURRENT_BINARY_DIR}/MapGuideApi.i
)

add_library(php_MapGuideApi${MG_VERSION_SUFFIX} SHARED ${php_MapGuideApi_SRCS})
add_dependencies(php_MapGuideApi${MG_VERSION_SUFFIX} swig IMake php)
install(TARGETS php_MapGuideApi${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libphp_MapGuideApi${MG_VERSION_SUFFIX}.so libphp_MapGuideApi.so ${MG_COMPONENT})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/constants.php DESTINATION ${MG_WWWROOT}/mapadmin COMPONENT ${MG_COMPONENT})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/constants.php DESTINATION ${MG_WWWROOT}/mapviewerphp COMPONENT ${MG_COMPONENT})

target_link_libraries(php_MapGuideApi${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    MgFoundation${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
    MgHttpHandler${MG_VERSION_SUFFIX}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
    ${XERCESC_LIBRARIES}
    MgWebApp${MG_VERSION_SUFFIX}
)

file(COPY "php.i" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "getclassid.code" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "catchall.code" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})