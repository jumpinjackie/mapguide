set(MG_COMPONENT "common")
set(CMAKE_INSTALL_PREFIX "${MG_INSTALL_PREFIX}")

set_install_subdir("")

add_subdirectory(GwsCommon)
add_subdirectory(GwsQueryEngine)
add_subdirectory(GwsResource)