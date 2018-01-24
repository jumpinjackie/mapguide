project(renderers)

include_directories(${GD_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${MG_OEM_DWF_INCLUDE_DIR}
    ${MG_OEM_DWF_INCLUDE_DIR}/dwf
    ${MG_OEM_AGG_INCLUDE_DIR}
    ${MG_OEM_AGG_FREETYPE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfModel
    ${CMAKE_CURRENT_SOURCE_DIR}/../Stylization
)

set (MgRenderers_SRCS
    AGGRenderer.cpp
    AGGImageIO.cpp
    AGGW2DRewriter.cpp
    AGGFillPatterns.cpp
    ../../Oem/agg-2.4/font_freetype/agg_font_freetype.cpp
    CriticalSection.cpp
    DWFRenderer.cpp
    EPlotRenderer.cpp
    FontManager.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_curves.cpp
    ${MG_OEM_AGG_DIR}/font_freetype/agg_font_freetype.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_line_aa_basics.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_line_profile_aa.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_sqrt_tables.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_trans_affine.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_vcgen_contour.cpp
    ${MG_OEM_AGG_SOURCE_DIR}/agg_vcgen_stroke.cpp
    GDFillPatterns.cpp
    GDRenderer.cpp
    GDUtils.cpp
    GDW2DRewriter.cpp
    KmlContent.cpp
    KmlLineStyle.cpp
    KmlPolyStyle.cpp
    KmlIconStyle.cpp
    KmlRenderer.cpp
    MapQuantization.cpp
    ObservationMesh.cpp
    RenderUtil.cpp
    RS_ByteData.cpp
    SymbolTrans.cpp
    UTFGridRenderer.cpp
    UTFGridContent.cpp
    W2DRewriter.cpp
    complex_polygon_gd.cpp
)

add_definitions(-DDWFTK_BUILD_EXPAT)

add_library(MgRenderers${MG_VERSION_SUFFIX} SHARED ${MgRenderers_SRCS})

target_link_libraries(MgRenderers${MG_VERSION_SUFFIX}
    ${GD_LIBRARY}
    ${FREETYPE_LIBRARY}
    ${JPEG_LIBRARY}
    ${PNG_LIBRARY}
    ${AGG_LIBRARY}
    dwfcore-1.7.0
    dwftk-7.7.0
    MgStylization${MG_VERSION_SUFFIX}
)

install( TARGETS MgRenderers${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT} )
install_symlink( ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgRenderers${MG_VERSION_SUFFIX}.so libMgRenderers.so ${MG_COMPONENT} )