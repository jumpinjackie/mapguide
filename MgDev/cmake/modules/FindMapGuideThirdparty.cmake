# This file is strictly to check and enable system or internal Thirdparty
# libraries

include(FindPackageHandleStandardArgs)

if (ALL_THIRDPARTY)
    set(INTERNAL_ACE true)
    set(INTERNAL_CPPUNIT true)
    set(INTERNAL_ZLIB true)
    set(INTERNAL_LIBPNG true)
    set(INTERNAL_JPEG true)
    set(INTERNAL_FREETYPE true)
    set(INTERNAL_GD true)
    set(INTERNAL_JSON true)
    set(INTERNAL_GEOS true)
    set(INTERNAL_XERCES true)
endif (ALL_THIRDPARTY)

if (INTERNAL_ACE)
    message(STATUS "Using internal ACE")
    #find_package(ACE REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/ACE/ACE_wrappers 
    #          ${MG_OEM_WORK_DIR}/ACE/ACE_wrappers/lib)
    find_path(ACE_INCLUDE_DIR 
        NAMES "ace/Init_ACE.h"
        PATHS ${MG_OEM_WORK_DIR}/ACE/ACE_wrappers)
    find_library(ACE_LIBRARY 
        NAMES ACE 
        PATHS ${MG_OEM_WORK_DIR}/ACE/ACE_wrappers/lib)
    message(STATUS "ACE Include Dir: ${ACE_INCLUDE_DIR}")
    message(STATUS "ACE Library: ${ACE_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(ACE DEFAULT_MSG ACE_LIBRARY ACE_INCLUDE_DIR)
    mark_as_advanced(ACE_INCLUDE_DIR ACE_LIBRARY)
else (INTERNAL_ACE)
    message(STATUS "Using system-provided ACE")
    find_package(ACE REQUIRED)
endif (INTERNAL_ACE)

if (INTERNAL_GEOS)
    message(STATUS "Using internal GEOS")
    #find_package(GEOS REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/geos/include
    #          ${MG_OEM_WORK_DIR}/geos/src/.libs)
    find_path(GEOS_INCLUDE_DIR 
        NAMES "geos/version.h"
            "geos/geom.h"
            "geos/util.h"
            "geos/io.h"
            "geos/unload.h"
        PATHS ${MG_OEM_WORK_DIR}/geos/include)
    find_library(GEOS_LIBRARY 
        NAMES geos
        PATHS ${MG_OEM_WORK_DIR}/geos/src/.libs)
    message(STATUS "GEOS Include Dir: ${GEOS_INCLUDE_DIR}")
    message(STATUS "GEOS Library: ${GEOS_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOS DEFAULT_MSG GEOS_LIBRARY GEOS_INCLUDE_DIR)
    mark_as_advanced(GEOS_INCLUDE_DIR GEOS_LIBRARY)
else (INTERNAL_GEOS)
    find_package(GEOS REQUIRED)
endif (INTERNAL_GEOS)

if (INTERNAL_XERCES)
    message(STATUS "Using internal xerces-c")
    #find_package(XercesC REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/dbxml/xerces-c-src/src
    #          ${MG_OEM_WORK_DIR}/dbxml/xerces-c-src/src/.libs)
    find_path(XERCESC_INCLUDE_DIR 
        NAMES "xercesc/dom/DOM.hpp"
        PATHS ${MG_OEM_WORK_DIR}/dbxml/xerces-c-src/src)
    find_library(XERCESC_LIBRARIES 
        NAMES xerces-c
        PATHS ${MG_OEM_WORK_DIR}/dbxml/xerces-c-src/src/.libs)
    message(STATUS "xerces-c Include Dir: ${XERCESC_INCLUDE_DIR}")
    message(STATUS "xerces-c Library: ${XERCESC_LIBRARIES}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(xerces-c DEFAULT_MSG XERCESC_LIBRARIES XERCESC_INCLUDE_DIR)
    mark_as_advanced(XERCESC_INCLUDE_DIR XERCESC_LIBRARIES)
else (INTERNAL_XERCES)
    find_package(XercesC REQUIRED)
endif (INTERNAL_XERCES)

if (INTERNAL_GD)
    message(STATUS "Using internal GD")
    #find_package(GD REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/gd/gd
    #          ${MG_OEM_WORK_DIR}/gd/gd/.libs)
    find_path(GD_INCLUDE_DIR 
        NAMES "gd.h"
        PATHS ${MG_OEM_WORK_DIR}/gd/gd)
    find_library(GD_LIBRARY 
        NAMES gd
        PATHS ${MG_OEM_WORK_DIR}/gd/gd/.libs)
    message(STATUS "GD Include Dir: ${GD_INCLUDE_DIR}")
    message(STATUS "GD Library: ${GD_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(GD DEFAULT_MSG GD_LIBRARY GD_INCLUDE_DIR)
    mark_as_advanced(GD_INCLUDE_DIR GD_LIBRARY)
else (INTERNAL_GD)
    find_package(GD REQUIRED)
endif (INTERNAL_GD)

if (INTERNAL_FREETYPE)
    message(STATUS "Using internal FreeType")
    #find_package(Freetype REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/gd/freetype
    #          ${MG_OEM_WORK_DIR}/gd/freetype/.libs)
    find_path(FREETYPE_INCLUDE_DIRS 
        NAMES "ft2build.h"
        PATHS ${MG_OEM_WORK_DIR}/gd/freetype/include)
    find_library(FREETYPE_LIBRARY
        NAMES freetype
        PATHS ${MG_OEM_WORK_DIR}/gd/freetype/objs/.libs)
    message(STATUS "FreeType Include Dir: ${FREETYPE_INCLUDE_DIRS}")
    message(STATUS "FreeType Library: ${FREETYPE_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(FreeType DEFAULT_MSG FREETYPE_LIBRARY FREETYPE_INCLUDE_DIRS)
    mark_as_advanced(FREETYPE_INCLUDE_DIRS FREETYPE_LIBRARY)
else (INTERNAL_FREETYPE)
    find_package(Freetype REQUIRED)
endif (INTERNAL_FREETYPE)

if (INTERNAL_JPEG)
    message(STATUS "Using internal libjpeg")
    #find_package(JPEG REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/gd/jpeg
    #          ${MG_OEM_WORK_DIR}/gd/jpeg/.libs)
    find_path(JPEG_INCLUDE_DIR 
        NAMES "jpeglib.h"
        PATHS ${MG_OEM_WORK_DIR}/gd/jpeg)
    find_library(JPEG_LIBRARY 
        NAMES jpeg
        PATHS ${MG_OEM_WORK_DIR}/gd/jpeg/.libs)
    message(STATUS "libjpeg Include Dir: ${JPEG_INCLUDE_DIR}")
    message(STATUS "libjpeg Library: ${JPEG_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(jpeg DEFAULT_MSG JPEG_LIBRARY JPEG_INCLUDE_DIR)
    mark_as_advanced(JPEG_INCLUDE_DIR JPEG_LIBRARY)
else (INTERNAL_JPEG)
    find_package(JPEG REQUIRED)
endif (INTERNAL_JPEG)

if (INTERNAL_PNG)
    message(STATUS "Using internal libpng")
    #find_package(PNG REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/gd/png
    #          ${MG_OEM_WORK_DIR}/gd/png/.libs)
    find_path(PNG_INCLUDE_DIRS 
        NAMES "png.h"
        PATHS ${MG_OEM_WORK_DIR}/gd/lpng)
    find_library(PNG_LIBRARY 
        NAMES png
        PATHS ${MG_OEM_WORK_DIR}/gd/lpng/.libs)
    message(STATUS "libpng Include Dir: ${PNG_INCLUDE_DIRS}")
    message(STATUS "libpng Library: ${PNG_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(libpng DEFAULT_MSG PNG_LIBRARY PNG_INCLUDE_DIRS)
    mark_as_advanced(PNG_INCLUDE_DIRS PNG_LIBRARY)
else (INTERNAL_PNG)
    find_package(PNG REQUIRED)
endif (INTERNAL_PNG)

if (INTERNAL_CPPUNIT)
    message(STATUS "Using internal cppunit")
    #find_package(CppUnit REQUIRED
    #    PATHS ${MG_OEM_WORK_DIR}/cppunit
    #          ${MG_OEM_WORK_DIR}/cppunit/.libs)
    find_path(CPPUNIT_INCLUDE_DIR 
        NAMES "cppunit/CompilerOutputter.h"
        PATHS ${MG_OEM_WORK_DIR}/CppUnit/include)
    find_library(CPPUNIT_LIBRARIES 
        NAMES cppunit
        PATHS ${MG_OEM_WORK_DIR}/CppUnit/src/cppunit/.libs)
    message(STATUS "cppunit Include Dir: ${CPPUNIT_INCLUDE_DIR}")
    message(STATUS "cppunit Library: ${CPPUNIT_LIBRARIES}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(cppunit DEFAULT_MSG CPPUNIT_LIBRARIES CPPUNIT_INCLUDE_DIR)
    mark_as_advanced(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARIES)
else (INTERNAL_CPPUNIT)
    find_package(CppUnit REQUIRED)
endif (INTERNAL_CPPUNIT)

if (INTERNAL_JSON)
    message(STATUS "Using internal jsoncpp")
    find_path(JsonCpp_INCLUDE_DIR 
        NAMES "json/json.h"
        PATHS ${MG_OEM_WORK_DIR}/jsoncpp/include)
    find_library(JsonCpp_LIBRARY 
        NAMES lib_json
        PATHS ${MG_OEM_WORK_DIR}/jsoncpp/lib)
    message(STATUS "jsoncpp Include Dir: ${JsonCpp_INCLUDE_DIR}")
    message(STATUS "jsoncpp Library: ${JsonCpp_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(jsoncpp DEFAULT_MSG JsonCpp_LIBRARY JsonCpp_INCLUDE_DIR)
    mark_as_advanced(JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)
else (INTERNAL_JSON)
    find_package(JsonCpp REQUIRED)
endif (INTERNAL_JSON)

if (INTERNAL_ZLIB)
    message(STATUS "Using internal zlib")
    find_path(ZLIB_INCLUDE_DIR 
        NAMES "zlib.h"
        PATHS ${MG_OEM_WORK_DIR}/gd/zlib)
    find_library(ZLIB_LIBRARY
        NAMES z
        PATHS ${MG_OEM_WORK_DIR}/gd/zlib)
    message(STATUS "zlib Include Dir: ${ZLIB_INCLUDE_DIR}")
    message(STATUS "zlib Library: ${ZLIB_LIBRARY}")
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(zlib DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
    mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY)
else (INTERNAL_ZLIB)
    find_package(ZLIB REQUIRED)
endif (INTERNAL_ZLIB)

find_package(FDO REQUIRED)
find_package(TCLAP REQUIRED)
find_package(LinuxApt REQUIRED)
find_package(DbXmlInternal REQUIRED)
find_package(Ant REQUIRED)