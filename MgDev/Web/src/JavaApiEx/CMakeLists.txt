include_directories(${JNI_INCLUDE_DIRS}
    ${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${ACE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../HttpHandler
    ${CMAKE_CURRENT_SOURCE_DIR}/../WebSupport
    ${CMAKE_CURRENT_SOURCE_DIR}/../WebApp
    ${XERCESC_INCLUDE_DIR}
)

add_definitions(-DJAVA)
add_definitions(-fno-strict-aliasing -fno-var-tracking-assignments)

set(MapGuideJavaApiEx_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/MgApi_wrap.cpp
)

set(DISPOSE_EXPR "((MgDisposable*)arg1)->Release()")
set(RETHROW_EXPR "e->Raise()")

add_custom_command(OUTPUT ${MapGuideJavaApiEx_SRCS}
    COMMAND ${CMAKE_COMMAND} -E remove -f 
            ${CMAKE_CURRENT_BINARY_DIR}/org/osgeo/mapguide/*.class
    COMMAND ${IMAKE_TOOL} -p ../MapGuideApi/MapGuideApiGen.xml
            -l Java
            -r ${CMAKE_CURRENT_SOURCE_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/java.i
            ${CMAKE_CURRENT_BINARY_DIR}/language.i
    COMMAND ${IMAKE_TOOL} -p ${CMAKE_CURRENT_BINARY_DIR}/../MapGuideApi/Constants.xml 
            -l Java 
            -r ${CMAKE_CURRENT_SOURCE_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}/org/osgeo/mapguide
            -t
    COMMAND ${SWIG_TOOL} -dllname MapGuideJavaApiEx -c++ -java -DJAVA 
            -mgjavanothrow -mgjavacasing
            -package org.osgeo.mapguide 
            -root MgObject -baseexception MgException -clsidcode getclassid.code -clsiddata m_cls_id 
            -catchallcode catchall.code 
            -dispose \"${DISPOSE_EXPR}\" 
            -rethrow \"${RETHROW_EXPR}\;\" 
            -nodefault -noconstants -module MapGuideJavaApiEx 
            -o ${MapGuideJavaApiEx_SRCS} 
            -lib ${SWIG_LIB_PATH} 
            ${CMAKE_CURRENT_BINARY_DIR}/MapGuideApi.i
    COMMAND ${MOVE_COMMAND} 
            ${CMAKE_CURRENT_BINARY_DIR}/*.java 
            ${CMAKE_CURRENT_BINARY_DIR}/org/osgeo/mapguide
    COMMAND ${Java_JAVAC_EXECUTABLE} -classpath ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/org/osgeo/mapguide/*.java
    COMMAND ${Java_JAR_EXECUTABLE} -cf 
            ${CMAKE_CURRENT_BINARY_DIR}/../WEB-INF/lib/MapGuideApiEx.jar
            org/osgeo/mapguide/*.class
    COMMAND ${Java_JAR_EXECUTABLE} -cf 
            ${CMAKE_CURRENT_BINARY_DIR}/../WEB-INF/lib/MapGuideApiEx-sources.jar
            org/osgeo/mapguide/*.java
)

add_library(MapGuideJavaApiEx${MG_VERSION_SUFFIX} SHARED ${MapGuideJavaApiEx_SRCS})
add_dependencies(MapGuideJavaApiEx${MG_VERSION_SUFFIX} swig IMake)
install(TARGETS MapGuideJavaApiEx${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMapGuideJavaApiEx${MG_VERSION_SUFFIX}.so libMapGuideJavaApiEx.so ${MG_COMPONENT})

set(MG_JARS
    "${CMAKE_CURRENT_BINARY_DIR}/../WEB-INF/lib/MapGuideApiEx.jar"
    "${CMAKE_CURRENT_BINARY_DIR}/../WEB-INF/lib/MapGuideApiEx-sources.jar"
)
install(FILES ${MG_JARS} DESTINATION ${MG_WWWROOT}/WEB-INF/lib)

target_link_libraries(MapGuideJavaApiEx${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    ${JNI_LIBRARIES}
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

file(COPY "java.i" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "getclassid.code" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "catchall.code" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "org" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# Specific for JavaApiEx
file(COPY "javaextensions.i" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})