include_directories(${MG_OEM_CSMAP_INCLUDE_DIR})

add_definitions(-DCSCOMPARE -DCSAPI -DMG)

set (CsMap_SRCS
    ../CsMap/CsMapDev/Source/CS_alber.c
    ../CsMap/CsMapDev/Source/CS_angle.c
    ../CsMap/CsMapDev/Source/CS_ansi.c
    ../CsMap/CsMapDev/Source/CS_ats77.c
    ../CsMap/CsMapDev/Source/CS_azmea.c
    ../CsMap/CsMapDev/Source/CS_azmed.c
    ../CsMap/CsMapDev/Source/CS_badekas.c
    ../CsMap/CsMapDev/Source/CS_bonne.c
    ../CsMap/CsMapDev/Source/CS_bpcnc.c
    ../CsMap/CsMapDev/Source/CS_bursa.c
    ../CsMap/CsMapDev/Source/CS_bynFile.c
    ../CsMap/CsMapDev/Source/CS_category.c
    ../CsMap/CsMapDev/Source/CS_csini.c
    ../CsMap/CsMapDev/Source/CS_csio.c
    ../CsMap/CsMapDev/Source/CS_csprm.c
    ../CsMap/CsMapDev/Source/CS_csvFileSupport.cpp
    ../CsMap/CsMapDev/Source/CS_csWktLoc.c
    ../CsMap/CsMapDev/Source/CS_ctio.c
    ../CsMap/CsMapDev/Source/CS_datum.c
    ../CsMap/CsMapDev/Source/CS_defaults.c
    ../CsMap/CsMapDev/Source/CS_defCmp.c
    ../CsMap/CsMapDev/Source/CS_defCmpEx.c
    ../CsMap/CsMapDev/Source/CS_dtcalc.c
    ../CsMap/CsMapDev/Source/CS_dtio.c
    ../CsMap/CsMapDev/Source/CS_dtmBridge.c
    ../CsMap/CsMapDev/Source/CS_edcnc.c
    ../CsMap/CsMapDev/Source/CS_edcyl.c
    ../CsMap/CsMapDev/Source/CS_egm96.c
    ../CsMap/CsMapDev/Source/CS_ekrt4.c
    ../CsMap/CsMapDev/Source/CS_ekrt6.c
    ../CsMap/CsMapDev/Source/CS_elCalc.c
    ../CsMap/CsMapDev/Source/CS_elio.c
    ../CsMap/CsMapDev/Source/CS_epsgStuff.cpp
    ../CsMap/CsMapDev/Source/CS_epsgSupport.cpp
    ../CsMap/CsMapDev/Source/CS_erpt.c
    ../CsMap/CsMapDev/Source/CS_error.c
    ../CsMap/CsMapDev/Source/CS_fips.c
    ../CsMap/CsMapDev/Source/CS_frame.c
    ../CsMap/CsMapDev/Source/CS_frnch.c
    ../CsMap/CsMapDev/Source/CS_gauss.c
    ../CsMap/CsMapDev/Source/CS_general.c
    ../CsMap/CsMapDev/Source/CS_geocn.c
    ../CsMap/CsMapDev/Source/CS_geoct.c
    ../CsMap/CsMapDev/Source/CS_geoid96.c
    ../CsMap/CsMapDev/Source/CS_geoid99.c
    ../CsMap/CsMapDev/Source/CS_geoidHeight.c
    ../CsMap/CsMapDev/Source/CS_gissupprt.c
    ../CsMap/CsMapDev/Source/CS_gnomc.c
    ../CsMap/CsMapDev/Source/CS_gpio.c
    ../CsMap/CsMapDev/Source/CS_gridi.c
    ../CsMap/CsMapDev/Source/CS_groups.c
    ../CsMap/CsMapDev/Source/CS_guiApi.c
    ../CsMap/CsMapDev/Source/CS_gxIndex.c
    ../CsMap/CsMapDev/Source/CS_gxio.c
    ../CsMap/CsMapDev/Source/CS_gxprm.c
    ../CsMap/CsMapDev/Source/CS_hlApi.c
    ../CsMap/CsMapDev/Source/CS_hmlsn.c
    ../CsMap/CsMapDev/Source/CS_hpApi.c
    ../CsMap/CsMapDev/Source/CS_japan.c
    ../CsMap/CsMapDev/Source/CS_krovk.c
    ../CsMap/CsMapDev/Source/CS_lmbrt.c
    ../CsMap/CsMapDev/Source/CS_lmtan.c
    ../CsMap/CsMapDev/Source/CS_mgrs.c
    ../CsMap/CsMapDev/Source/CS_millr.c
    ../CsMap/CsMapDev/Source/CS_modpc.c
    ../CsMap/CsMapDev/Source/CS_molod.c
    ../CsMap/CsMapDev/Source/CS_molwd.c
    ../CsMap/CsMapDev/Source/CS_mrcat.c
    ../CsMap/CsMapDev/Source/CS_mstro.c
    ../CsMap/CsMapDev/Source/CS_mulrg.c
    ../CsMap/CsMapDev/Source/CS_nacyl.c
    ../CsMap/CsMapDev/Source/CS_nadcn.c
    ../CsMap/CsMapDev/Source/CS_nameMapper.cpp
    ../CsMap/CsMapDev/Source/CS_nameMapperSupport.cpp
    ../CsMap/CsMapDev/Source/CS_nerth.c
    ../CsMap/CsMapDev/Source/CS_ntv1.c
    ../CsMap/CsMapDev/Source/CS_ntv2.c
    ../CsMap/CsMapDev/Source/CS_nullx.c
    ../CsMap/CsMapDev/Source/CS_nzlnd.c
    ../CsMap/CsMapDev/Source/CS_oblqm.c
    ../CsMap/CsMapDev/Source/CS_optional.c
    ../CsMap/CsMapDev/Source/CS_ortho.c
    ../CsMap/CsMapDev/Source/CS_osgm91.c
    ../CsMap/CsMapDev/Source/CS_ost02.c
    ../CsMap/CsMapDev/Source/CS_ost97.c
    ../CsMap/CsMapDev/Source/CS_ostn02.c
    ../CsMap/CsMapDev/Source/CS_ostn97.c
    ../CsMap/CsMapDev/Source/CS_ostro.c
    ../CsMap/CsMapDev/Source/CS_parm3.c
    ../CsMap/CsMapDev/Source/CS_parm4.c
    ../CsMap/CsMapDev/Source/CS_parm6.c
    ../CsMap/CsMapDev/Source/CS_parm7.c
    ../CsMap/CsMapDev/Source/CS_plycn.c
    ../CsMap/CsMapDev/Source/CS_plynm.c
    ../CsMap/CsMapDev/Source/CS_pstro.c
    ../CsMap/CsMapDev/Source/CS_rlsUpdt.c
    ../CsMap/CsMapDev/Source/CS_robin.c
    ../CsMap/CsMapDev/Source/CS_sinus.c
    ../CsMap/CsMapDev/Source/CS_sstro.c
    ../CsMap/CsMapDev/Source/CS_supprt.c
    ../CsMap/CsMapDev/Source/CS_swiss.c
    ../CsMap/CsMapDev/Source/CS_sys34.c
    ../CsMap/CsMapDev/Source/CS_system.c
    ../CsMap/CsMapDev/Source/CS_tacyl.c
    ../CsMap/CsMapDev/Source/CS_trmer.c
    ../CsMap/CsMapDev/Source/CS_trmrs.c
    ../CsMap/CsMapDev/Source/CS_units.c
    ../CsMap/CsMapDev/Source/CS_unity.c
    ../CsMap/CsMapDev/Source/CS_vdgrn.c
    ../CsMap/CsMapDev/Source/CS_vertconUS.c
    ../CsMap/CsMapDev/Source/CS_vrtcon.c
    ../CsMap/CsMapDev/Source/CS_wellknowntext.cpp
    ../CsMap/CsMapDev/Source/CS_wgs72.c
    ../CsMap/CsMapDev/Source/CS_winkelTripel.c
    ../CsMap/CsMapDev/Source/CS_wktObject.cpp
    ../CsMap/CsMapDev/Source/CS_wktObjKonstants.cpp
    ../CsMap/CsMapDev/Source/CS_zones.c
    ../CsMap/CsMapDev/Source/CScs2Wkt.cpp
    ../CsMap/CsMapDev/Source/CScscomp.c
    ../CsMap/CsMapDev/Source/CSdata.c
    ../CsMap/CsMapDev/Source/CSdataDT.c
    ../CsMap/CsMapDev/Source/CSdataPJ.c
    ../CsMap/CsMapDev/Source/CSdataU.c
    ../CsMap/CsMapDev/Source/CSdatumCatalog.c
    ../CsMap/CsMapDev/Source/CSdictDiff.c
    ../CsMap/CsMapDev/Source/CSdt2Wkt.cpp
    ../CsMap/CsMapDev/Source/CSdtcomp.c
    ../CsMap/CsMapDev/Source/CSel2Wkt.cpp
    ../CsMap/CsMapDev/Source/CSelcomp.c
    ../CsMap/CsMapDev/Source/CSgeodeticSupport.c
    ../CsMap/CsMapDev/Source/CSgpcomp.c
    ../CsMap/CsMapDev/Source/CSgxcomp.c
    ../CsMap/CsMapDev/Source/CSwktFlavors.c
    ../CsMap/CsMapDev/Source/csIoUtil.cpp
)

add_library(CsMap STATIC ${CsMap_SRCS})

set (CsMapCompiler_SRCS
    ../CsMap/CsMapDev/Dictionaries/CS_Comp.c
)

add_executable(CsMapCompiler ${CsMapCompiler_SRCS})
set_target_properties( CsMapCompiler
    PROPERTIES OUTPUT_NAME "CS_Comp" )
target_link_libraries(CsMapCompiler CsMap)
install(TARGETS CsMapCompiler DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})

set (CsMapDictDiff_SRCS
    ../CsMap/CsMapDev/Dictionaries/CS_DictDiff.c
)
add_executable(CsMapDictDiff ${CsMapDictDiff_SRCS})
set_target_properties( CsMapDictDiff
    PROPERTIES OUTPUT_NAME "CS_DictDiff" )
target_link_libraries(CsMapDictDiff CsMap)
install(TARGETS CsMapDictDiff DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})

set(CSMAP_DICT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../CsMap/CsMapDev/Dictionaries")
set(CSMAP_DICT_STAGING_DIR "${CMAKE_CURRENT_BINARY_DIR}/CsMap_Install")

add_custom_command(TARGET CsMapCompiler
    POST_BUILD
    COMMAND ./CS_Comp -b ${CSMAP_DICT_STAGING_DIR}/Dictionaries ${CSMAP_DICT_STAGING_DIR}/Dictionaries
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Compile dictionaries"
)

file(COPY ${CSMAP_DICT_SRC_DIR} DESTINATION ${CSMAP_DICT_STAGING_DIR})
# Remove junk we know to not be related to dictionary data
if (CSMAP_NO_GRID)
    file(GLOB_RECURSE CSMAP_JUNK_FILES
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.c"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.cnt"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.HLP"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.mak"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.nmk"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/WW15MGH.GRD"
        # FIXME: Though this removes the files it still leaves the empty directories
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Geoid03/*.*"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Geoid96/*.*"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Geoid99/*.*"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/NSRS2007/*.*"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/NSRS2011/*.*"
    )
else (CSMAP_NO_GRID)
    file(GLOB_RECURSE CSMAP_JUNK_FILES
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.c"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.cnt"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.HLP"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.mak"
        "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.nmk"
    )
endif (CSMAP_NO_GRID)
#message(STATUS "Removing CS-Map junk: ${CSMAP_JUNK_FILES}")
file(REMOVE ${CSMAP_JUNK_FILES})

file(GLOB CS_MAP_ROOT_FILES "${CSMAP_DICT_STAGING_DIR}/Dictionaries/*.*")
#message(STATUS "CS-Map root files: ${CS_MAP_ROOT_FILES}")
install(FILES ${CS_MAP_ROOT_FILES} DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
# NOTE: These .csd files below are generated as a post-build so they aren't caught by the initial GLOB above
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Category.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Coordsys.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Datums.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Elipsoid.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/GeodeticPath.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(FILES ${CSMAP_DICT_STAGING_DIR}/Dictionaries/GeodeticTransform.CSD DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Australia DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Brazil DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Canada DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/France DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Germany DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Japan DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/NewZealand DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Portugal DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Spain DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Switzerland DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/UK DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
if (CSMAP_NO_GRID)
    install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Harn DESTINATION ${MG_INSTALL_COORDSYS_PREFIX}/Usa COMPONENT ${MG_COMPONENT_COMMON})
    install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Nadcon DESTINATION ${MG_INSTALL_COORDSYS_PREFIX}/Usa COMPONENT ${MG_COMPONENT_COMMON})
    install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa/Vertcon DESTINATION ${MG_INSTALL_COORDSYS_PREFIX}/Usa COMPONENT ${MG_COMPONENT_COMMON})
else (CSMAP_NO_GRID)
    install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Usa DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
endif (CSMAP_NO_GRID)
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Venezuela DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})