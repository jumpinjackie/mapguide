#
#! /bin/bash
#

#
# MapGuide Open Source build script.
#

#**********************************************************
# Initialization
#**********************************************************

echo "MapGuide Open Source build script for OEM components"
INSTALLDIR=/usr/local/mapguideopensource
CLEAN_FLAG=0
BUILD_CPU=32
BUILD_CONFIG=release
while [ $# -gt 0 ]; do    # Until you run out of parameters...
    case "$1" in
        -prefix|--prefix)
            INSTALLDIR="$2"
            shift
            ;;
        -clean|--clean)
            CLEAN_FLAG=1
            shift
            ;;
        -build|--build)
            BUILD_CPU=$2
            shift
            ;;
        -config|--config)
            BUILD_CONFIG=$2
            shift
            ;;
        -help|--help)
            echo "Usage: $0 (options)"
            echo "Options:"
            echo "  --prefix [installation directory]"
            echo "  --clean [clean all objects and binaries in Oem]"
            echo "  --build [32(default)|64]"
            echo "  --config [release(default)|debug]"
            echo "  --help [Display usage]"
            exit
            ;;
    esac
    shift   # Check next set of parameters.
done

echo "OEM Build Initialization ............................"
echo "Oem Libraries will be installed at: ${INSTALLDIR}"

LIB_NAME=""

#**********************************************************
# Generic Functions
#**********************************************************

check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "$LIB_NAME: Error build failed ($error)................."
        exit $error
    fi
}

check_clean()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "$LIB_NAME: Error clean failed ($error)................."
        exit $error
    fi
}

#**********************************************************
# Build ACE 5.7
# Notes: none
#**********************************************************

init_ace()
{
    LIB_NAME="ACE 5.7"
}

build_ace()
{
    pushd ACE/ACE_wrappers
    ACE_PATH="${PWD}"
    pushd ace
    env ACE_ROOT="${ACE_PATH}" make
    check_build
    popd
    popd
}

clean_ace()
{
    pushd ACE/ACE_wrappers
    ACE_PATH="${PWD}"
    pushd ace
    env ACE_ROOT="${ACE_PATH}" make clean
    check_clean
    popd
    popd
}

#**********************************************************
# Build DWFCORE 1.0
# Notes: none
#**********************************************************

init_dwfcore()
{
    LIB_NAME="DWFCORE 1.0"
    pushd DWFTK7.1/develop/global/build/gnu/dwfcore
    if [ $CLEAN_FLAG -eq 0 ]; then
    	sh ./build_setup.sh
    fi
    popd
}

build_dwfcore()
{
    pushd DWFTK7.1/develop/global/src/dwfcore
    aclocal
    libtoolize --copy --force
    automake --add-missing --copy
    autoconf
    if [ $BUILD_CONFIG = "debug" ]; then
        sh ./configure --disable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    else
        sh ./configure --enable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    fi
    make
    check_build
    popd
}

clean_dwfcore()
{
    pushd DWFTK7.1/develop/global/src/dwfcore
    make clean
    check_clean
    popd
}

#**********************************************************
# Build DWFTOOLKIT 7.1
# Notes: none
#**********************************************************

init_dwftk()
{
    LIB_NAME="DWFTK 7.1"
    pushd DWFTK7.1/develop/global/build/gnu/dwftoolkit
    if [ $CLEAN_FLAG -eq 0 ]; then
    	sh ./build_setup.sh
    fi
    popd
}

build_dwftk()
{
    pushd DWFTK7.1/develop/global/src/dwf
    aclocal
    libtoolize --copy --force
    automake --add-missing --copy
    autoconf
    if [ $BUILD_CONFIG = "debug" ]; then
        sh ./configure --disable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    else
        sh ./configure --enable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    fi
    make
    check_build
    popd
}

clean_dwftk()
{
    pushd DWFTK7.1/develop/global/src/dwf
    make clean
    check_clean
    popd
}

#**********************************************************
# Build DWFEMAP 1.0
# Notes: none
#**********************************************************

init_dwfemap()
{
    LIB_NAME="DWFEMAP 1.0"
    pushd DWFTK7.1/develop/global/build/gnu/dwfemap
    if [ $CLEAN_FLAG -eq 0 ]; then
    	sh ./build_setup.sh
    fi
    popd
}

build_dwfemap()
{
    pushd DWFTK7.1/develop/global/src/dwfemap
    aclocal
    libtoolize --copy --force
    automake --add-missing --copy
    autoconf
    if [ $BUILD_CONFIG = "debug" ]; then
        sh ./configure --disable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    else
        sh ./configure --enable-optimized --enable-silent-rules --prefix="${INSTALLDIR}"
    fi
    make
    check_build
    popd
}

clean_dwfemap()
{
    pushd DWFTK7.1/develop/global/src/dwfemap
    make clean
    check_clean
    popd
}

#**********************************************************
# Build GEOS 2.2.0
# Notes: none
#**********************************************************

init_geos()
{
    LIB_NAME="GEOS 2.2.0"
}

build_geos()
{
    pushd geos-2.2.0
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --with-pic --prefix="${INSTALLDIR}"
    else
        sh ./configure --prefix="${INSTALLDIR}"
    fi
    make
    # The check build is disabled as the build will fail with automake version < 2.59
    # check_build
    popd
}

clean_geos()
{
    pushd geos-2.2.0
    make clean
    check_clean
    popd
}

#**********************************************************
# Build SWIGEx 1.0
# Notes: none
#**********************************************************

init_swigex()
{
    LIB_NAME="SWIGEx 1.0"
}

build_swigex()
{
    pushd SWIGEx
    mkdir -p Linux/obj/release
    make
    check_build
    popd
}

clean_swigex()
{
    pushd SWIGEx
    # Note: the qmake system is required to regenerate the Makefile.
    #       This is disabled.
    # make clean
    # check_clean
    popd
}

#**********************************************************
# Build BDBXML
# Notes: none
#**********************************************************

init_bdbxml()
{
    LIB_NAME="BDBXML"
}

build_bdbxml()
{
    # Need to force regen of the correct xerces config header before building dbxml proper
    pushd dbxml/xerces-c-src
    sh ./configure
    popd
    # Also need to check and set executable flag for s_paths
    pushd dbxml/dbxml/dist
    chmod +x s_paths
    popd
    pushd dbxml
    sh ./buildall.sh
    check_build
    popd
}

clean_bdbxml()
{
    pushd dbxml
    sh ./buildall.sh --clean
    check_clean
    popd
}

#**********************************************************
# Build CPPUNIT 1.9.14
# Notes: none
#**********************************************************

init_cppunit()
{
    LIB_NAME="CPPUNIT 1.9.14"
}

build_cppunit()
{
    pushd CppUnit-1.9.14
    # Force regen of configure to cover our bases
    aclocal -I config
    libtoolize --copy --force
    autoconf
    automake --add-missing --copy --force-missing
    # -ldl is to prevent undefined reference to dlsym/dlopen/dlclose
    sh ./configure --enable-silent-rules --prefix="${INSTALLDIR}" LDFLAGS="-ldl"
    make
    check_build
    popd
}

clean_cppunit()
{
    pushd CppUnit-1.9.14
    make clean
    check_clean
    popd
}

#**********************************************************
# Build IMake 1.0
# Notes: none
#**********************************************************

init_imake()
{
    LIB_NAME="IMake 1.0"
}

build_imake()
{
    pushd ../BuildTools/WebTools/IMake
    mkdir -p Linux/obj/debug
    make
    check_build
    popd
}

clean_imake()
{
    pushd ../BuildTools/WebTools/IMake
    # Note: does not support clean as qmake is needed to regenerate the Makefile.
    # make clean
    # check_clean
    popd
}

#**********************************************************
# Build ZLIB 1.2.3
# Notes: none
#**********************************************************

init_zlib()
{
    LIB_NAME="ZLIB 1.2.3"
}

build_zlib()
{
    pushd gd/zlib
    sh ./configure
    make
    check_build
    popd
}

clean_zlib()
{
    pushd gd/zlib
    make clean
    check_clean
    popd
}

#**********************************************************
# Build LIBPNG 1.2.8
# Notes: none
#**********************************************************

init_libpng()
{
    LIB_NAME="LIBPNG 1.2.8"
}

build_libpng()
{
    pushd gd/lpng
    cp scripts/makefile.std makefile
    if [ $BUILD_CPU -eq 64 ]; then
        #Inject -fPIC to CFLAGS for 64-bit
        sed 's/^CFLAGS=/CFLAGS= -fPIC /g' makefile > makefile64
        make -fmakefile64
    else
        make
    fi
    check_build
    popd
}

clean_libpng()
{
    pushd gd/lpng
    make clean
    check_clean
    popd
}

#**********************************************************
# Build JPEG 6b
# Notes: none
#**********************************************************

init_jpeg()
{
    LIB_NAME="JPEG 6b"
}

build_jpeg()
{
    pushd gd/jpeg
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared
        #--with-pic does nothing (probably ancient configure script), so do some sed trickery
        #to inject this flag. Know a better way? Enlighten us :)
        sed 's/^CFLAGS=/CFLAGS= -fPIC/g' Makefile > Makefile64
        make -fMakefile64
    else
        sh ./configure --enable-static --disable-shared
        make
    fi
    check_build
    popd
}

clean_jpeg()
{
    pushd gd/jpeg
    make clean
    check_clean
    popd
}

#**********************************************************
# Build FREETYPE 2.1.10
# Notes: none
#**********************************************************

init_freetype()
{
    LIB_NAME="FREETYPE 2.1.10"
}

build_freetype()
{
    pushd gd/freetype
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --with-pic
    else
    	sh ./configure --enable-static --disable-shared
    fi
    make
    check_build
    popd
}

clean_freetype()
{
    pushd gd/freetype
    make clean
    check_clean
    popd
}

#**********************************************************
# Build GD 2.0.33
# Notes: none
#**********************************************************

init_gd()
{
    LIB_NAME="GD 2.0.33"
}

build_gd()
{
    pushd gd/gd
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --without-fontconfig --enable-silent-rules --with-pic
    else
        sh ./configure --enable-static --disable-shared --without-fontconfig --enable-silent-rules
    fi
    make
    check_build
    popd
}

clean_gd()
{
    pushd gd/gd
    make clean
    check_clean
    popd
}

#**********************************************************
# Build AGG 2.4
# Notes: none
#**********************************************************

init_agg()
{
    LIB_NAME="AGG 2.4"
}

build_agg()
{
    pushd agg-2.4
    if [ $BUILD_CPU -eq 64 ]; then
        make EXTRACXXFLAGS=-fPIC
    else
        make
    fi
    check_build
    popd
}

clean_agg()
{
    pushd agg-2.4
    make clean
    check_clean
    popd
}

#**********************************************************
# Build JSON
# Notes: none
#**********************************************************

init_json()
{
    LIB_NAME="JSON"
}

build_json()
{
    pushd jsoncpp
    # Use system scons
    scons platform=linux-gcc
    popd
}

clean_json()
{
    pushd jsoncpp
    rm -rf lib buildscons dist
    popd
}

#**********************************************************
# Build CSMAP
# Notes: none
#**********************************************************

init_csmap()
{
    LIB_NAME="CSMAP"
}

build_csmap()
{
    pushd CsMap
    mkdir -p .libs
    pushd Source
    if [ $BUILD_CPU -eq 64 ]; then
        #Need to build CS-Map with -fPIC because linking libraries will be built with -fPIC
        sed 's/^C_FLG =/C_FLG = -fPIC/g' Library.mak | sed 's/CPP_FLG =/CPP_FLG = -fPIC/g' > Library64.mak
        make -fLibrary64.mak
    else
        make -fLibrary.mak
    fi
    cp CsMap.a ../.libs/libCsmap.a
    popd
    pushd Dictionaries
    if [ $BUILD_CPU -eq 64 ]; then
        #Need to build CS-Map with -fPIC because linking libraries will be built with -fPIC
        sed 's/^C_FLG =/C_FLG = -fPIC/g' Library.mak | sed 's/CPP_FLG =/CPP_FLG = -fPIC/g' > Compiler64.mak
        make -fCompiler64.mak
    else
        make -fCompiler.mak
    fi
    ./CS_Comp -b . .
    popd
    check_build
    popd
}

clean_csmap()
{
    pushd CsMap/Source
    if [ $BUILD_CPU -eq 64 ]; then
        make clean -fLibrary64.mak
    else
        make clean -fLibrary.mak
    fi
    check_clean
    popd
}

#**********************************************************
# Script loop
#**********************************************************

pushd Oem
for lib in ace dwfcore dwftk dwfemap geos bdbxml cppunit imake zlib libpng jpeg freetype gd agg json csmap;
do
    echo "$lib: Initialization..........................."
    init_"$lib"

    if test "$CLEAN_FLAG" = "1"; then
        echo "$lib: Clean ....................................."
        clean_"$lib"
        echo "$lib: Clean Successful .........................."
    else
        echo "$lib: Configure/Build ..........................."
        build_"$lib"
        echo "$lib: Build Successful .........................."
    fi
done
popd
