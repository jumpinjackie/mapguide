project(dwftk)

add_definitions(-DDWFCORE_BUILD_ZLIB -DDWFCORE_BUILD_EXPAT)

find_library(pthread REQUIRED)
find_library(uuid REQUIRED)
if (NOT uuid-NOTFOUND)
    #add_definitions(-DHAVE_LIBUUID)
    set (HAVE_LIBUUID 1)
else (NOT uuid-NOTFOUND)
    message(STATUS "Could not find libuuid")
endif (NOT uuid-NOTFOUND)

check_include_file(bits/stl_algobase.h HAVE_ALGOBASE_H)
check_include_file(asm/atomic.h HAVE_ATOMIC_H)
check_include_file(alsa/iatomic.h HAVE_IATOMIC_H)
check_include_file(netinet/ether.h HAVE_NETINET_ETHER_H)

#if (HAVE_NETINET_ETHER_H)
#    add_definitions(-DHAVE_NETINET_ETHER_H)
#endif (HAVE_NETINET_ETHER_H)

if (HAVE_ATOMIC_H)
    check_cxx_source_compiles(
        "#include <asm/atomic.h>
        int main() {
            int i = 0;
            atomic_t tValue;
            atomic_set( &tValue, i );
            atomic_inc( &tValue );
            atomic_dec( &tValue );
            i = atomic_read( &tValue );
        }" 
        CAN_COMPILE_WITH_ATOMIC_H)
    if (CAN_COMPILE_WITH_ATOMIC_H)
        #add_definitions(-D_DWFCORE_USE_ASM_ATOMIC_H)
        set (_DWFCORE_USE_ASM_ATOMIC_H 1)
    else (CAN_COMPILE_WITH_ATOMIC_H)
        message(WARNING "Found asm/atomic.h but could not compile with it")
    endif (CAN_COMPILE_WITH_ATOMIC_H)
endif (HAVE_ATOMIC_H)

if (HAVE_IATOMIC_H)
check_cxx_source_compiles(
    "#include <alsa/iatomic.h>
    int main() {
        int i = 0;
        atomic_t tValue;
        atomic_set( &tValue, i );
        atomic_inc( &tValue );
        atomic_dec( &tValue );
        i = atomic_read( &tValue );
    }" 
    CAN_COMPILE_WITH_IATOMIC_H)
    if (CAN_COMPILE_WITH_IATOMIC_H)
        #add_definitions(-D_DWFCORE_USE_ALSA_IATOMIC_H)
        set (_DWFCORE_USE_ALSA_IATOMIC_H 1)
    else (CAN_COMPILE_WITH_IATOMIC_H)
    message(WARNING "Found alsa/atomic.h but could not compile with it")
    endif (CAN_COMPILE_WITH_IATOMIC_H)
endif (HAVE_IATOMIC_H)

check_function_exists(max HAVE_MAX)
check_function_exists(min HAVE_MIN)
check_function_exists(memmove HAVE_MEMMOVE)

#if (HAVE_MEMMOVE)
#   add_definitions(-DHAVE_MEMMOVE)
#else (HAVE_MEMMOVE)
#   message(ERROR "Could not find required function memmove")
#endif (HAVE_MEMMOVE)

if (NOT HAVE_MEMMOVE)
    message(ERROR "Could not find required function memmove")
endif (NOT HAVE_MEMMOVE)

# DWFCore's CMakeLists.txt will generate a config.h for us
add_definitions(-DHAVE_CONFIG_H)

# This is to ensure dwfcore/config.h will properly resolve not only for
# DWFCore, but DWFToolkit and downstream dependencies
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Autotools does not define these in debug, and neither shall we
remove_definitions(-DDEBUG -D_DEBUG)

# We don't use W3D, so we don't care about needing to build the internal libjpeg.
# This will also help us avoid unexpected surprises if a system libjpeg is also loaded
set (DISABLE_STREAM_JPEG 1)
set (DWFTK_EXPORT_API 1)
set (WHIP_USE_WCHAR_STRINGS 1)
set (WHIPTOOL_CLIENT 1)
set (DWFTK_USE_DWFCORE_ZLIB 1)

set (DWF_VERSION_MAJOR 7)
set (DWF_VERSION_MINOR 7)
set (DWF_VERSION_REV 0)
set (DWF_VERSION "${DWF_VERSION_MAJOR}.${DWF_VERSION_MINOR}.${DWF_VERSION_REV}")

set (DWFCORE_VERSION_MAJOR 1)
set (DWFCORE_VERSION_MINOR 7)
set (DWFCORE_VERSION_REV 0)
set (DWFCORE_VERSION "${DWFCORE_VERSION_MAJOR}.${DWFCORE_VERSION_MINOR}.${DWFCORE_VERSION_REV}")

set (W3DTK_VERSION_MAJOR 1)
set (W3DTK_VERSION_MINOR 7)
set (W3DTK_VERSION_REV 1555)
set (W3DTK_VERSION "${W3DTK_VERSION_MAJOR}.${W3DTK_VERSION_MINOR}.${W3DTK_VERSION_REV}")

set (W2DTK_VERSION_MAJOR 7)
set (W2DTK_VERSION_MINOR 13)
set (W2DTK_VERSION_REV 1601)
set (W2DTK_VERSION "${W3DTK_VERSION_MAJOR}.${W3DTK_VERSION_MINOR}.${W3DTK_VERSION_REV}")

add_subdirectory(dwf)
add_subdirectory(dwfcore)