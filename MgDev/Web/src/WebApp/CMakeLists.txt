include_directories(${MG_COMMON_DIR}/MdfModel
    ${MG_COMMON_DIR}/Foundation
    ${MG_COMMON_DIR}/Geometry
    ${MG_COMMON_DIR}/PlatformBase
    ${MG_COMMON_DIR}/MapGuideCommon
    ${XERCESC_INCLUDE_DIR}
    ${ACE_INCLUDE_DIR}
)

set(MgWebApp_SRCS
    WebBufferCommand.cpp
    WebCommand.cpp
    WebCommandCollection.cpp
    WebCommandWidget.cpp
    WebContextMenu.cpp
    WebFlyoutWidget.cpp
    WebGettingStarted.cpp
    WebGetPrintablePageCommand.cpp
    WebHelpCommand.cpp
    WebInformationPane.cpp
    WebInvokeScriptCommand.cpp
    WebInvokeUrlCommand.cpp
    WebLayout.cpp
    WebMeasureCommand.cpp
    WebPrintCommand.cpp
    WebSearchCommand.cpp
    WebSelectWithinCommand.cpp
    WebSeparatorWidget.cpp
    WebTaskBarWidget.cpp
    WebTaskBar.cpp
    WebTaskPane.cpp
    WebToolbar.cpp
    WebUiPane.cpp
    WebUiSizablePane.cpp
    WebUiTargetCommand.cpp
    WebViewOptionsCommand.cpp
    WebWidget.cpp
    WebWidgetCollection.cpp
)

add_library(MgWebApp${MG_VERSION_SUFFIX} SHARED ${MgWebApp_SRCS})
install(TARGETS MgWebApp${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgWebApp${MG_VERSION_SUFFIX}.so libMgWebApp.so ${MG_COMPONENT})

target_link_libraries(MgWebApp${MG_VERSION_SUFFIX}
    ${XERCESC_LIBRARIES}
    MgFoundation${MG_VERSION_SUFFIX}
    MgGeometry${MG_VERSION_SUFFIX}
    MgPlatformBase${MG_VERSION_SUFFIX}
    MgMapGuideCommon${MG_VERSION_SUFFIX}
)