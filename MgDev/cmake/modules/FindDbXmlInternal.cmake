# FindDbXml.cmake
#
# Looks for DBXML headers and supporting libraries. If none found, the internal copy
# is built and the check is done again
#
# This module defines (if found, or found after build):
#
# DBXML_INCLUDE_DIR - The DBXML headers
# DBXML_LIBRARY - The DBXML library
# DB_CXX_LIBRARY - The BerkeleyDB C++ library
# XQILLA_LIBRARY - The XQilla library
# XERCESC_LIBRARIES - The xerces C++ library (only if INTERNAL_XERCES is set)

message(STATUS "Checking for DbXml in ${MG_OEM_WORK_DIR}")

find_path(DBXML_INCLUDE_DIR
    NAMES "dbxml/DbXml.hpp" 
    PATHS ${MG_OEM_WORK_DIR}/dbxml/install/include)
find_library(DBXML_LIBRARY 
    NAMES dbxml 
    PATHS ${MG_OEM_WORK_DIR}/dbxml/install/lib)
find_library(DB_CXX_LIBRARY 
    NAMES db_cxx 
    PATHS ${MG_OEM_WORK_DIR}/dbxml/install/lib)
find_library(XQILLA_LIBRARY 
    NAMES xqilla 
    PATHS ${MG_OEM_WORK_DIR}/dbxml/install/lib)

if (NOT INTERNAL_XERCES)
    
    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set DbXml_FOUND to TRUE if 
    # all listed variables are TRUE
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(DbXmlInternal DEFAULT_MSG DBXML_LIBRARY DB_CXX_LIBRARY XQILLA_LIBRARY DBXML_INCLUDE_DIR)

    if (DbXmlInternal_FOUND)
        message(STATUS "Found DbXml: ${DBXML_INCLUDE_DIR}, ${DBXML_LIBRARY}, ${DB_CXX_LIBRARY}, ${XQILLA_LIBRARY}")
    endif (DbXmlInternal_FOUND)

    mark_as_advanced(DBXML_INCLUDE_DIR DBXML_LIBRARY DB_CXX_LIBRARY XQILLA_LIBRARY)

else (NOT INTERNAL_XERCES)

    find_library(XERCESC_LIBRARIES
        NAMES xerces-c
        PATHS ${MG_OEM_WORK_DIR}/dbxml/install/lib)

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set DbXml_FOUND to TRUE if 
    # all listed variables are TRUE
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(DbXmlInternal DEFAULT_MSG DBXML_LIBRARY DB_CXX_LIBRARY XQILLA_LIBRARY XERCESC_LIBRARIES DBXML_INCLUDE_DIR)

    if (DbXmlInternal_FOUND)
        message(STATUS "Found DbXml: ${DBXML_INCLUDE_DIR}, ${DBXML_LIBRARY}, ${DB_CXX_LIBRARY}, ${XQILLA_LIBRARY}, ${XERCESC_LIBRARIES}")
    endif (DbXmlInternal_FOUND)

    mark_as_advanced(DBXML_INCLUDE_DIR DBXML_LIBRARY DB_CXX_LIBRARY XQILLA_LIBRARY XERCESC_LIBRARIES)

endif (NOT INTERNAL_XERCES)

