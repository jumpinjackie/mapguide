###### Fusion #####
# TODO: Actual fusion build should be offloaded to bootstrapping with the "install"
# step just copying the build output to its final destination because right now an
# elevated install will "taint" the CMake build dir with root-owned files produced
# from the below build task
file(COPY "fusion" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "fusionMG" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(FUSION_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR}/fusion)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${FUSION_WORK_DIR} ant deploy -Ddeploy.home=${MG_WWWROOT}/fusion)")
if (UNIX)
    # Make the tcpdf cache dir
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${MG_WWWROOT}/fusion/lib/tcpdf/cache)")
    # httpd's default config is to use user/group daemon/daemon, so make it own the tcpdf cache directory in order for QuickPlot to work
    install(CODE "execute_process(COMMAND chown daemon:daemon ${MG_WWWROOT}/fusion/lib/tcpdf/cache)")
endif (UNIX)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fusionMG/templates DESTINATION ${MG_WWWROOT}/fusion)
###### Everything else ######

# TODO: We have geos in-tree, which is CMake-enabled, so if required we can use
# the internal copy of geos if required through an option like INTERNAL_GEOS
# at the moment, we are just using the system-provided version of GEOS
#set(GEOS_ENABLE_TESTS OFF)
#add_subdirectory(geos)
add_subdirectory(DWFTK)
add_subdirectory(CsMapLibrary)
add_subdirectory(zlib_minizip)
add_subdirectory(SWIGEx)
add_subdirectory(LinuxApt)