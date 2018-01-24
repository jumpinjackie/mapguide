# FindDbXml.cmake
#
# Looks for HTTPD/PHP headers and supporting libraries. If none found, the internal copy
# is built and the check is done again
#
# This module defines (if found, or found after build):
#
# PHP_INCLUDE_DIR - The PHP headers
# HTTPD_INCLUDE_DIR - The HTTPD headers

message(STATUS "Checking for PHP in ${MG_OEM_WORK_DIR}/LinuxApt/php-${PHP_VER}")
message(STATUS "Checking for HTTPD in ${MG_OEM_WORK_DIR}/LinuxApt/httpd-${HTTPD_VER}")

find_path(PHP_INCLUDE_DIR
    NAMES "Zend/zend_API.h" 
    PATHS ${MG_OEM_WORK_DIR}/LinuxApt/php-${PHP_VER})

find_path(HTTPD_INCLUDE_DIR
    NAMES "include/ap_config.h" 
    PATHS ${MG_OEM_WORK_DIR}/LinuxApt/httpd-${HTTPD_VER})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LinuxApt_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LinuxApt DEFAULT_MSG PHP_INCLUDE_DIR HTTPD_INCLUDE_DIR)

if (LinuxApt_FOUND)
    message(STATUS "Found PHP: ${PHP_INCLUDE_DIR}")
    message(STATUS "Found HTTPD: ${HTTPD_INCLUDE_DIR}")
endif (LinuxApt_FOUND)

mark_as_advanced(PHP_INCLUDE_DIR HTTPD_INCLUDE_DIR)