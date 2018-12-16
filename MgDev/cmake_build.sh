#!/bin/sh

CMAKE_BUILD_DIR=~/mapguide_build_area
OEM_WORK_DIR=~/mapguide_oem_build
USE_NINJA=0
USE_ASAN=OFF
USE_LD_GOLD=OFF
while [ $# -gt 0 ]; do    # Until you run out of parameters...
    case "$1" in
        --cmake-build-dir)
            CMAKE_BUILD_DIR="$2"
            shift
            ;;
        --oem-working-dir)
            OEM_WORK_DIR="$2"
            shift
            ;;
        --ninja)
            USE_NINJA=1
            ;;
        --with-asan)
            USE_ASAN=ON
            ;;
        --with-gold)
            USE_LD_GOLD=ON
            ;;
        --help)
            echo "Usage: $0 (options)"
            echo "Options:"
            echo "  --cmake-build-dir [CMake build directory]"
            echo "  --oem-working-dir [installation directory]"
            echo "  --with-asan [build with ASAN (if available)]"
            echo "  --with-gold [link with gold instead of ld (if available)]"
            echo "  --ninja [Use ninja build system]"
            echo "  --help [Display usage]"
            exit
            ;;
    esac
    shift   # Check next set of parameters.
done

SOURCE_DIR=$(pwd)

check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "$LIB_NAME: Error build failed ($error)................."
        # Return back to this dir before bailing
        cd "$SOURCE_DIR" || exit
        exit $error
    fi
}

if [ ! -f "$OEM_WORK_DIR/env_vars.sh" ]; then
    echo "ERROR: Could not find env_vars.sh in $OEM_WORK_DIR"
    echo "       This file should exist if you ran cmake_bootstrap.sh with (--oem_working_dir $OEM_WORK_DIR)"
    exit 1;
fi

. "$OEM_WORK_DIR/env_vars.sh"

# Validate
if test "$BUILD_CPU" != "32" -a "$BUILD_CPU" != "64"; then
    echo "--build must be 32 or 64"
    exit 1;
fi
if test "$BUILD_CONFIG" != "Release" -a "$BUILD_CONFIG" != "Debug"; then
    echo "--config must be Release or Debug"
    exit 1;
fi

echo "Install prefix is: $MG_INST_PREFIX"
echo "Current source dir is: $SOURCE_DIR"
echo "Building for: $BUILD_CPU-bit"
echo "Building in: $BUILD_CONFIG"
echo "Using Ninja build: $USE_NINJA"
echo "Using ld.gold: $USE_LD_GOLD"
echo "CMake build directory is: $CMAKE_BUILD_DIR" 
echo "OEM Working Directory is: $OEM_WORK_DIR"

echo "Creating CMake build directory"
mkdir -p "$CMAKE_BUILD_DIR"

INTERNAL_XERCES=TRUE
if test $HAVE_SYSTEM_XERCES -eq 1; then
    INTERNAL_XERCES=FALSE
fi

# Now for the main event
echo "Setting up CMake in: $CMAKE_BUILD_DIR"
cd "$CMAKE_BUILD_DIR" || exit
if test $USE_NINJA -eq 1; then
    cmake -G Ninja "$SOURCE_DIR" -DMG_INSTALL_PREFIX=$MG_INST_PREFIX -DMG_CPU=$BUILD_CPU -DWITH_JAVA=TRUE -DUSE_LD_GOLD=$USE_LD_GOLD -DSANITIZE_ADDRESS=$USE_ASAN -DPHP_VER=$PHP_VER -DHTTPD_VER=$HTTPD_VER -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DMG_OEM_WORK_DIR=$OEM_WORK_DIR -DINTERNAL_XERCES=$INTERNAL_XERCES -DINTERNAL_ACE=$INTERNAL_ACE -DINTERNAL_CPPUNIT=$INTERNAL_CPPUNIT -DINTERNAL_ZLIB=$INTERNAL_ZLIB -DINTERNAL_JPEG=$INTERNAL_JPEG -DINTERNAL_PNG=$INTERNAL_PNG -DINTERNAL_FREETYPE=$INTERNAL_FREETYPE -DINTERNAL_GD=$INTERNAL_GD -DINTERNAL_JSON=$INTERNAL_JSON -DINTERNAL_GEOS=$INTERNAL_GEOS
    check_build
    ninja
    check_build
else
    cmake "$SOURCE_DIR" -DMG_INSTALL_PREFIX=$MG_INST_PREFIX -DMG_CPU=$BUILD_CPU -DWITH_JAVA=TRUE -DUSE_LD_GOLD=$USE_LD_GOLD -DSANITIZE_ADDRESS=$USE_ASAN -DPHP_VER=$PHP_VER -DHTTPD_VER=$HTTPD_VER -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DMG_OEM_WORK_DIR=$OEM_WORK_DIR -DINTERNAL_XERCES=$INTERNAL_XERCES -DINTERNAL_ACE=$INTERNAL_ACE -DINTERNAL_CPPUNIT=$INTERNAL_CPPUNIT -DINTERNAL_ZLIB=$INTERNAL_ZLIB -DINTERNAL_JPEG=$INTERNAL_JPEG -DINTERNAL_PNG=$INTERNAL_PNG -DINTERNAL_FREETYPE=$INTERNAL_FREETYPE -DINTERNAL_GD=$INTERNAL_GD -DINTERNAL_JSON=$INTERNAL_JSON -DINTERNAL_GEOS=$INTERNAL_GEOS
    check_build
    make
    check_build
fi