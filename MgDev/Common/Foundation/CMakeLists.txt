project(foundation)

include_directories(${ACE_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Data
    ${CMAKE_CURRENT_SOURCE_DIR}/Exception
    ${CMAKE_CURRENT_SOURCE_DIR}/System
    ${CMAKE_CURRENT_SOURCE_DIR}/../Security
    ${CMAKE_CURRENT_SOURCE_DIR}/../MdfModel)

set (MgFoundation_SRCS
    FoundationBuild.cpp
)

add_library(MgFoundation${MG_VERSION_SUFFIX} SHARED ${MgFoundation_SRCS})

target_link_libraries(MgFoundation${MG_VERSION_SUFFIX}
    ${ACE_LIBRARY}
    ${XERCESC_LIBRARIES}
    MgMdfModel${MG_VERSION_SUFFIX}
    MgMdfParser${MG_VERSION_SUFFIX}
)

install( TARGETS MgFoundation${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT} )
install_symlink( ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgFoundation${MG_VERSION_SUFFIX}.so libMgFoundation.so ${MG_COMPONENT} )
