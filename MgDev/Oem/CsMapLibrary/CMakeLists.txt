include_directories(${MG_OEM_CSMAP_INCLUDE_DIR})

add_definitions(-DCSCOMPARE -DCSAPI -DMG)

set (CsMap_SRCS
    ../CsMap/Source/CS_alber.c
    ../CsMap/Source/CS_angle.c
    ../CsMap/Source/CS_ansi.c
    ../CsMap/Source/CS_ats77.c
    ../CsMap/Source/CS_azmea.c
    ../CsMap/Source/CS_azmed.c
    ../CsMap/Source/CS_badekas.c
    ../CsMap/Source/CS_bonne.c
    ../CsMap/Source/CS_bpcnc.c
    ../CsMap/Source/CS_bursa.c
    ../CsMap/Source/CS_bynFile.c
    ../CsMap/Source/CS_category.c
    ../CsMap/Source/CS_csini.c
    ../CsMap/Source/CS_csio.c
    ../CsMap/Source/CS_csprm.c
    ../CsMap/Source/CS_csvFileSupport.cpp
    ../CsMap/Source/CS_csWktLoc.c
    ../CsMap/Source/CS_ctio.c
    ../CsMap/Source/CS_datum.c
    ../CsMap/Source/CS_defaults.c
    ../CsMap/Source/CS_defCmp.c
    ../CsMap/Source/CS_defCmpEx.c
    ../CsMap/Source/CS_dtcalc.c
    ../CsMap/Source/CS_dtio.c
    ../CsMap/Source/CS_dtmBridge.c
    ../CsMap/Source/CS_edcnc.c
    ../CsMap/Source/CS_edcyl.c
    ../CsMap/Source/CS_egm96.c
    ../CsMap/Source/CS_ekrt4.c
    ../CsMap/Source/CS_ekrt6.c
    ../CsMap/Source/CS_elCalc.c
    ../CsMap/Source/CS_elio.c
    ../CsMap/Source/CS_epsgStuff.cpp
    ../CsMap/Source/CS_epsgSupport.cpp
    ../CsMap/Source/CS_erpt.c
    ../CsMap/Source/CS_error.c
    ../CsMap/Source/CS_fips.c
    ../CsMap/Source/CS_frame.c
    ../CsMap/Source/CS_frnch.c
    ../CsMap/Source/CS_gauss.c
    ../CsMap/Source/CS_general.c
    ../CsMap/Source/CS_geocn.c
    ../CsMap/Source/CS_geoct.c
    ../CsMap/Source/CS_geoid96.c
    ../CsMap/Source/CS_geoid99.c
    ../CsMap/Source/CS_geoidHeight.c
    ../CsMap/Source/CS_gissupprt.c
    ../CsMap/Source/CS_gnomc.c
    ../CsMap/Source/CS_gpio.c
    ../CsMap/Source/CS_gridi.c
    ../CsMap/Source/CS_groups.c
    ../CsMap/Source/CS_guiApi.c
    ../CsMap/Source/CS_gxIndex.c
    ../CsMap/Source/CS_gxio.c
    ../CsMap/Source/CS_gxprm.c
    ../CsMap/Source/CS_hlApi.c
    ../CsMap/Source/CS_hmlsn.c
    ../CsMap/Source/CS_hpApi.c
    ../CsMap/Source/CS_japan.c
    ../CsMap/Source/CS_krovk.c
    ../CsMap/Source/CS_lmbrt.c
    ../CsMap/Source/CS_lmtan.c
    ../CsMap/Source/CS_mgrs.c
    ../CsMap/Source/CS_millr.c
    ../CsMap/Source/CS_modpc.c
    ../CsMap/Source/CS_molod.c
    ../CsMap/Source/CS_molwd.c
    ../CsMap/Source/CS_mrcat.c
    ../CsMap/Source/CS_mstro.c
    ../CsMap/Source/CS_mulrg.c
    ../CsMap/Source/CS_nacyl.c
    ../CsMap/Source/CS_nadcn.c
    ../CsMap/Source/CS_nameMapper.cpp
    ../CsMap/Source/CS_nameMapperSupport.cpp
    ../CsMap/Source/CS_nerth.c
    ../CsMap/Source/CS_ntv1.c
    ../CsMap/Source/CS_ntv2.c
    ../CsMap/Source/CS_nullx.c
    ../CsMap/Source/CS_nzlnd.c
    ../CsMap/Source/CS_oblqm.c
    ../CsMap/Source/CS_optional.c
    ../CsMap/Source/CS_ortho.c
    ../CsMap/Source/CS_osgm91.c
    ../CsMap/Source/CS_ost02.c
    ../CsMap/Source/CS_ost97.c
    ../CsMap/Source/CS_ostn02.c
    ../CsMap/Source/CS_ostn97.c
    ../CsMap/Source/CS_ostro.c
    ../CsMap/Source/CS_parm3.c
    ../CsMap/Source/CS_parm4.c
    ../CsMap/Source/CS_parm6.c
    ../CsMap/Source/CS_parm7.c
    ../CsMap/Source/CS_plycn.c
    ../CsMap/Source/CS_plynm.c
    ../CsMap/Source/CS_pstro.c
    ../CsMap/Source/CS_rlsUpdt.c
    ../CsMap/Source/CS_robin.c
    ../CsMap/Source/CS_sinus.c
    ../CsMap/Source/CS_sstro.c
    ../CsMap/Source/CS_supprt.c
    ../CsMap/Source/CS_swiss.c
    ../CsMap/Source/CS_sys34.c
    ../CsMap/Source/CS_system.c
    ../CsMap/Source/CS_tacyl.c
    ../CsMap/Source/CS_trmer.c
    ../CsMap/Source/CS_trmrs.c
    ../CsMap/Source/CS_units.c
    ../CsMap/Source/CS_unity.c
    ../CsMap/Source/CS_vdgrn.c
    ../CsMap/Source/CS_vertconUS.c
    ../CsMap/Source/CS_vrtcon.c
    ../CsMap/Source/CS_wellknowntext.cpp
    ../CsMap/Source/CS_wgs72.c
    ../CsMap/Source/CS_winkelTripel.c
    ../CsMap/Source/CS_wktObject.cpp
    ../CsMap/Source/CS_wktObjKonstants.cpp
    ../CsMap/Source/CS_zones.c
    ../CsMap/Source/CScs2Wkt.cpp
    ../CsMap/Source/CScscomp.c
    ../CsMap/Source/CSdata.c
    ../CsMap/Source/CSdataDT.c
    ../CsMap/Source/CSdataPJ.c
    ../CsMap/Source/CSdataU.c
    ../CsMap/Source/CSdatumCatalog.c
    ../CsMap/Source/CSdictDiff.c
    ../CsMap/Source/CSdt2Wkt.cpp
    ../CsMap/Source/CSdtcomp.c
    ../CsMap/Source/CSel2Wkt.cpp
    ../CsMap/Source/CSelcomp.c
    ../CsMap/Source/CSgeodeticSupport.c
    ../CsMap/Source/CSgpcomp.c
    ../CsMap/Source/CSgxcomp.c
    ../CsMap/Source/CSwktFlavors.c
    ../CsMap/Source/csIoUtil.cpp
)

add_library(CsMap STATIC ${CsMap_SRCS})

set (CsMapCompiler_SRCS
    ../CsMap/Dictionaries/CS_Comp.c
)
add_executable(CsMapCompiler ${CsMapCompiler_SRCS})
set_target_properties( CsMapCompiler
    PROPERTIES OUTPUT_NAME "CS_Comp" )
target_link_libraries(CsMapCompiler CsMap)
install(TARGETS CsMapCompiler DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})

set (CsMapDictDiff_SRCS
    ../CsMap/Dictionaries/CS_DictDiff.c
)
add_executable(CsMapDictDiff ${CsMapDictDiff_SRCS})
set_target_properties( CsMapDictDiff
    PROPERTIES OUTPUT_NAME "CS_DictDiff" )
target_link_libraries(CsMapDictDiff CsMap)
install(TARGETS CsMapDictDiff DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})

set(CSMAP_DICT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../CsMap/Dictionaries")
set(CSMAP_DICT_STAGING_DIR "${CMAKE_CURRENT_BINARY_DIR}/CsMap_Install")
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
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Australia DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Brazil DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Canada DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/France DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Germany DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/Japan DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
install(DIRECTORY ${CSMAP_DICT_STAGING_DIR}/Dictionaries/NewZealand DESTINATION ${MG_INSTALL_COORDSYS_PREFIX} COMPONENT ${MG_COMPONENT_COMMON})
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
install(CODE "execute_process(COMMAND ${MG_INSTALL_COORDSYS_PREFIX}/CS_Comp -b ${MG_INSTALL_COORDSYS_PREFIX} ${MG_INSTALL_COORDSYS_PREFIX})")