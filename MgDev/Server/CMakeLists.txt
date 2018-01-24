add_definitions( -DACE_HAS_THREAD_SAFE_ACCEPT )

set(MG_COMPONENT ${MG_COMPONENT_SERVER})
set_install_subdir("server/")

add_subdirectory(src)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RepositoryAdmin DESTINATION ${MG_INSTALL_SERVER_PREFIX} COMPONENT ${MG_COMPONENT})
install_empty_directory("Logs" ${MG_INSTALL_SERVER_PREFIX})
install_empty_directory("Packages" ${MG_INSTALL_SERVER_PREFIX})
install_empty_directory("Repositories" ${MG_INSTALL_SERVER_PREFIX})
install_empty_directory("Temp" ${MG_INSTALL_SERVER_PREFIX})
install_empty_directory("Trash" ${MG_INSTALL_SERVER_PREFIX})