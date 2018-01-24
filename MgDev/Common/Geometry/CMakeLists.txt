project(geometry)

include_directories(${ACE_INCLUDE_DIR}
    ${GEOS_INCLUDE_DIR}
    ${MG_OEM_CSMAP_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../CoordinateSystem
    ${CMAKE_CURRENT_SOURCE_DIR}/../Security
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfPaser
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfModel
    ${CMAKE_CURRENT_SOURCE_DIR}/../Foundation
    ${CMAKE_CURRENT_SOURCE_DIR}/../Geometry)

set (MgGeometry_SRCS
    GeometryBuild.cpp
    ../CoordinateSystem/ArbitraryCoordsys.cpp
    ../CoordinateSystem/CoordSysDictionaryBase.cpp
    ../CoordinateSystem/CoordSys.cpp
    ../CoordinateSystem/CoordSysCatalog.cpp
    ../CoordinateSystem/CoordSysCategory.cpp
    ../CoordinateSystem/CoordSysCategoryDictionary.cpp
    ../CoordinateSystem/CoordSysDatum.cpp
    ../CoordinateSystem/CoordSysDatumDictionary.cpp
    ../CoordinateSystem/CoordSysDictionary.cpp
    ../CoordinateSystem/CoordSysEllipsoid.cpp
    ../CoordinateSystem/CoordSysEllipsoidDictionary.cpp
    ../CoordinateSystem/CoordSysEnum.cpp
    ../CoordinateSystem/CoordSysEnumCategory.cpp
    ../CoordinateSystem/CoordSysEnumCoordinateSystemInCategory.cpp
    ../CoordinateSystem/CoordSysEnumDatum.cpp
    ../CoordinateSystem/CoordSysEnumEllipsoid.cpp
    ../CoordinateSystem/CoordSysFormatConverter.cpp
    ../CoordinateSystem/CoordSysGeodeticAnalyticalTransformDefParams.cpp
    ../CoordinateSystem/CoordSysGeodeticInterpolationTransformDefParams.cpp
    ../CoordinateSystem/CoordSysGeodeticMultipleRegressionTransformDefParams.cpp
    ../CoordinateSystem/CoordSysGeodeticStandaloneTransformDefParams.cpp
    ../CoordinateSystem/CoordSysGeodeticPath.cpp
    ../CoordinateSystem/CoordSysGeodeticPathDictionary.cpp
    ../CoordinateSystem/CoordSysGeodeticPathElement.cpp
    ../CoordinateSystem/CoordSysGeodeticTransformation.cpp
    ../CoordinateSystem/CoordSysGeodeticTransformDef.cpp
    ../CoordinateSystem/CoordSysGeodeticTransformDefDictionary.cpp
    ../CoordinateSystem/CoordSysGeodeticTransformDefParams.cpp
    ../CoordinateSystem/CoordSysGeodeticTransformGridFile.cpp
    ../CoordinateSystem/CoordSysMeasure.cpp
    ../CoordinateSystem/CoordSysTransform.cpp
    ../CoordinateSystem/CoordSysDictionaryUtility.cpp
    ../CoordinateSystem/CoordSysEnumInteger32.cpp
    ../CoordinateSystem/CoordSysMathComparator.cpp
    ../CoordinateSystem/CoordSysProjectionInformation.cpp
    ../CoordinateSystem/CoordSysUnitInformation.cpp
    ../CoordinateSystem/CoordSysUtil.cpp
    ../CoordinateSystem/MentorDictionary.cpp
    ../CoordinateSystem/MentorUtil.cpp
    ../CoordinateSystem/CoordSysWktFailureCache.cpp
    ../CoordinateSystem/CoordSysGridGeneric.cpp
    ../CoordinateSystem/CoordSysGrids.cpp
    ../CoordinateSystem/CoordSysMgrsZone.cpp
    ../CoordinateSystem/CoordSysMgrs.cpp
    ../CoordinateSystem/CoordSysMgrsMajorRegion.cpp
    ../CoordinateSystem/CoordSysOneGrid.cpp
    ../CoordinateSystem/namestruct.cpp
    ../CoordinateSystem/CriticalSection.cpp
    ../CoordinateSystem/SmartCriticalClass.cpp
)

add_library(MgGeometry${MG_VERSION_SUFFIX} SHARED ${MgGeometry_SRCS})

target_link_libraries(MgGeometry${MG_VERSION_SUFFIX}
    MgFoundation${MG_VERSION_SUFFIX}
    CsMap
    ${ACE_LIBRARY}
    ${GEOS_LIBRARY}
    pthread
)

install( TARGETS MgGeometry${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT} )
install_symlink( ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgGeometry${MG_VERSION_SUFFIX}.so libMgGeometry.so ${MG_COMPONENT} )