#!/bin/sh

OEM_WORK_DIR=~/mapguide_oem_build
INTERNAL_ACE=0
INTERNAL_CPPUNIT=0
INTERNAL_ZLIB=0
INTERNAL_LIBPNG=0
INTERNAL_JPEG=0
INTERNAL_FREETYPE=0
INTERNAL_GD=0
INTERNAL_JSON=0
INTERNAL_GEOS=0
BUILD_CPU=32
BUILD_CONFIG=Release
HAVE_SYSTEM_XERCES=0
USE_CCACHE=0
while [ $# -gt 0 ]; do    # Until you run out of parameters...
    case "$1" in
        --oem-working-dir)
            OEM_WORK_DIR="$2"
            shift
            ;;
        --build)
            BUILD_CPU=$2
            shift
            ;;
        --config)
            BUILD_CONFIG=$2
            shift
            ;;
        --with-all-internal)
            INTERNAL_ACE=1
            INTERNAL_CPPUNIT=1
            INTERNAL_ZLIB=1
            INTERNAL_LIBPNG=1
            INTERNAL_JPEG=1
            INTERNAL_FREETYPE=1
            INTERNAL_GD=1
            INTERNAL_JSON=1
            INTERNAL_GEOS=1
            ;;
        --with-internal-ace)
            INTERNAL_ACE=1
            ;;
        --with-internal-cppunit)
            INTERNAL_CPPUNIT=1
            ;;
        --with-internal-zlib)
            INTERNAL_ZLIB=1
            ;;
        --with-internal-libpng)
            INTERNAL_LIBPNG=1
            ;;
        --with-internal-jpeg)
            INTERNAL_JPEG=1
            ;;
        --with-internal-freetype)
            INTERNAL_FREETYPE=1
            ;;
        --with-internal-gd)
            INTERNAL_GD=1
            ;;
        --with-internal-json)
            INTERNAL_JSON=1
            ;;
        --with-internal-geos)
            INTERNAL_GEOS=1
            ;;
        --have-system-xerces)
            HAVE_SYSTEM_XERCES=1
            ;;
        --with-ccache)
            USE_CCACHE=1
            ;;
        --help)
            echo "Usage: $0 (options)"
            echo "Options:"
            echo "  --oem-working-dir [oem working directory, default: ~/mapguide_oem_build]"
            echo "  --build [32(default)|64]"
            echo "  --config [Release(default)|Debug]"
            echo "  --have-system-xerces [Use system-installed xerces]"
            echo "  --help [Display usage]"
            echo "  --with-internal-ace [Build/Use internal ACE]"
            echo "  --with-internal-cppunit [Build/Use internal CppUnit]"
            echo "  --with-internal-zlib [Build/Use internal zlib]"
            echo "  --with-internal-jpeg [Build/Use internal jpeg]"
            echo "  --with-internal-libpng [Build/Use internal libpng]"
            echo "  --with-internal-freetype [Build/Use internal freetype]"
            echo "  --with-internal-gd [Build/Use internal GD]"
            echo "  --with-internal-json [Build/Use internal JsonCpp]"
            echo "  --with-internal-geos [Build/Use internal GEOS]"
            echo "  --with-all-internal [Build/Use all internal libs]"
            echo "  --with-ccache [Use ccache]"
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

# Validate
if test "$BUILD_CPU" != "32" -a "$BUILD_CPU" != "64"; then
    echo "--build must be 32 or 64"
    exit 1;
fi
if test "$BUILD_CONFIG" != "Release" -a "$BUILD_CONFIG" != "Debug"; then
    echo "--config must be Release or Debug"
    exit 1;
fi

echo "Current source dir is: $SOURCE_DIR"
echo "Building for: $BUILD_CPU-bit"
echo "Building in: $BUILD_CONFIG"
echo "Using system xerces: $HAVE_SYSTEM_XERCES"
echo "OEM Working Directory is: $OEM_WORK_DIR"
echo "Using ccache: $USE_CCACHE"
echo "INTERNAL_ACE      = $INTERNAL_ACE"
echo "INTERNAL_CPPUNIT  = $INTERNAL_CPPUNIT"
echo "INTERNAL_ZLIB     = $INTERNAL_ZLIB"
echo "INTERNAL_LIBPNG   = $INTERNAL_LIBPNG"
echo "INTERNAL_JPEG     = $INTERNAL_JPEG"
echo "INTERNAL_FREETYPE = $INTERNAL_FREETYPE"
echo "INTERNAL_GD       = $INTERNAL_GD"
echo "INTERNAL_JSON     = $INTERNAL_JSON"
echo "INTERNAL_GEOS     = $INTERNAL_GEOS"

if [ "$USE_CCACHE" = "1" ]; then
    export CC="ccache gcc"
    export CXX="ccache g++"
    ccache -s
fi

echo "Checking if OEM working dir exists"
if [ ! -d "$OEM_WORK_DIR" ]; then
    mkdir -p "$OEM_WORK_DIR"
    echo "Created OEM working dir at: $OEM_WORK_DIR"
fi

if [ "$INTERNAL_ACE" = "1" ]; then
    echo "Building ACE (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/ACE" "${OEM_WORK_DIR}/ACE"
    ace_buildbits=32
    ace_debug=0
    ace_optimize=1
    if [ $BUILD_CONFIG = "Debug" ]; then
        ace_debug=1
        ace_optimize=0
    chmod +x bin/*.sh
    fi
    if [ $BUILD_CPU -eq 64 ]; then
        ace_buildbits=64
    fi
    cd "${OEM_WORK_DIR}/ACE/ACE_wrappers" || exit
    ACE_PATH="${PWD}"
    cd ace || exit
    env ACE_ROOT="${ACE_PATH}" make buildbits=$ace_buildbits debug=$ace_debug optimize=$ace_optimize
    check_build
fi
if [ "$INTERNAL_CPPUNIT" = "1" ]; then
    echo "Building CppUnit (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/CppUnit-1.9.14" "${OEM_WORK_DIR}/CppUnit"
    cd "${OEM_WORK_DIR}/CppUnit" || exit
    # Force regen of configure to cover our bases
    aclocal -I config
    libtoolize --copy --force
    autoconf
    automake --add-missing --copy --force-missing
    # -ldl is to prevent undefined reference to dlsym/dlopen/dlclose
    sh ./configure --enable-silent-rules LDFLAGS="-ldl"
    make && make install
    check_build
fi
mkdir -p "${OEM_WORK_DIR}/gd"
if [ "$INTERNAL_ZLIB" = "1" ]; then
    echo "Building zlib (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/zlib" "${OEM_WORK_DIR}/gd/zlib"
    cd "${OEM_WORK_DIR}/gd/zlib" || exit
    if [ "$USE_CCACHE" = "1" ]; then
        # Aliasing CC=ccache gcc breaks the gcc check in this configure script that
        # would've inserted the required -fPIC flag. So specify manually
        CFLAGS="-O3 -fPIC" sh ./configure
    else
        sh ./configure
    fi
    make && make install
    check_build
fi
if [ "$INTERNAL_LIBPNG" = "1" ]; then
    echo "Building libpng (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/lpng" "${OEM_WORK_DIR}/gd/lpng"
    cd "${OEM_WORK_DIR}/gd/lpng" || exit
    cp scripts/makefile.std makefile
    if [ $BUILD_CPU -eq 64 ]; then
        #Inject -fPIC to CFLAGS for 64-bit
        sed 's/^CFLAGS=/CFLAGS= -fPIC -m64 /g' makefile > makefile64
        make -fmakefile64
        make -fmakefile64 install
        check_build
    else
        make && make install
        check_build
    fi
fi
if [ "$INTERNAL_JPEG" = "1" ]; then
    echo "Building libjpeg (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/jpeg" "${OEM_WORK_DIR}/gd/jpeg"
    cd "${OEM_WORK_DIR}/gd/jpeg" || exit
    # This directory must exist in order for make install to work
    mkdir -p /usr/local/man/man1
    # A jconfig.h that is not modifed by ./configure will trip up make, so nuke it first
    rm -f jconfig.h
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared
        #--with-pic does nothing (probably ancient configure script), so do some sed trickery
        #to inject this flag. Know a better way? Enlighten us :)
        sed 's/^CFLAGS=/CFLAGS= -fPIC -m64 /g' Makefile > Makefile64
        make -fMakefile64
        check_build
        make -fMakefile64 install-lib
        check_build
    else
        sh ./configure --enable-static --disable-shared
        make
        check_build
        make install
        check_build
    fi
fi
if [ "$INTERNAL_FREETYPE" = "1" ]; then
    echo "Building freetype (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/freetype" "${OEM_WORK_DIR}/gd/freetype"
    cd "${OEM_WORK_DIR}/gd/freetype" || exit
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --with-pic
    else
    	sh ./configure --enable-static --disable-shared
    fi
    make && make install
    check_build
fi

# Before we build GD, set some vars for PHP that we can reuse here too
PHP_JPEG_DIR=/usr/lib
PHP_PNG_DIR=/usr/lib
PHP_FREETYPE_DIR=/usr/lib
PHP_ZLIB_DIR=/usr/lib
if [ "$INTERNAL_JPEG" = "1" ]; then
    PHP_JPEG_DIR="$OEM_WORK_DIR/install/libjpeg"
fi
if [ "$INTERNAL_LIBPNG" = "1" ]; then
    PHP_PNG_DIR="$OEM_WORK_DIR/install/libpng/usr/local"
fi    
if [ "$INTERNAL_FREETYPE" = "1" ]; then
    PHP_FREETYPE_DIR="$OEM_WORK_DIR/install/freetype"
fi
if [ "$INTERNAL_ZLIB" = "1" ]; then
    PHP_ZLIB_DIR="$OEM_WORK_DIR/install/zlib"
fi

if [ "$INTERNAL_GD" = "1" ]; then
    echo "Building GD (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/gd" "${OEM_WORK_DIR}/gd/gd"
    cd "${OEM_WORK_DIR}/gd/gd" || exit
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --without-fontconfig --with-jpeg=/usr/local --with-pic
    else
        sh ./configure --enable-static --disable-shared --without-fontconfig --with-jpeg=/usr/local
    fi
    make && make install
    check_build
fi
if [ "$INTERNAL_JSON" = "1" ]; then
    echo "Building JsonCpp (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/jsoncpp" "${OEM_WORK_DIR}/jsoncpp"
    cd "${OEM_WORK_DIR}/jsoncpp" || exit
    # Use system scons
    scons platform=linux-gcc
    check_build
fi
if [ "$INTERNAL_GEOS" = "1" ]; then
    echo "Building GEOS (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/geos" "${OEM_WORK_DIR}/geos"
    cd "${OEM_WORK_DIR}/geos" || exit
    # NOTE: We could build with CMake, but it isn't the "blessed" build system for the version we
    # have internally (3.4.2), so we're sticking with autotools
    #cmake "${SOURCE_DIR}/Oem/geos" .

    # For this version of GEOS, don't run the aclocal/libtoolize/automake/autoconf quartet as we normally do, as it
    # may produce an unusable configure script. Just run autoreconf to regenerate the configure script from configure.in
    autoreconf
    
    # Fix for error:
    # virtual void geos::geom::GeometryComponentFilter::filter_ro(const geos::geom::Geometry*): Assertion `0' failed
    #
    # Based on this GEOS ticket: https://trac.osgeo.org/geos/ticket/469
    # The fix to to set the appropriate CFLAGS/CPPFLAGS/CXXFLAGS/LDFLAGS/FFLAGS before calling the configure script
    #
    # If we upgrade our internal copy of GEOS in the future, this should be reviewed
    #
    # NOTE: We've added --disable-shared --enable-silent-rules so only the static lib gets built and CMake doesn't get any
    # strange ideas about wanting to link to the dynamic lib
    chmod +x configure
    if [ $BUILD_CPU -eq 64 ]; then
        CFLAGS="-m64" CPPFLAGS="-m64" CXXFLAGS="-m64" LDFLAGS="-m64" FFLAGS="-m64" LDFLAGS="-L/usr/lib64" ./configure --with-pic --enable-static --disable-shared --enable-silent-rules --prefix="${INSTALLDIR}"
    else
        CFLAGS="-m32" CPPFLAGS="-m32" CXXFLAGS="-m32" LDFLAGS="-m32" FFLAGS="-m32" LDFLAGS="-L/usr/lib" ./configure --enable-static --disable-shared --enable-silent-rules --prefix="${INSTALLDIR}"
    fi
    make
    # The check build is disabled as the build will fail with automake version < 2.59
    check_build
fi

echo "Checking if we need to build DBXML"
# Basically perform the same checks as FindDbXml.cmake
HAS_DBXML_HEADERS=0
HAS_DBXML_LIB=0
HAS_DBCXX_LIB=0
HAS_XQILLA_LIB=0
if [ -f "${OEM_WORK_DIR}/dbxml/install/include/dbxml/DbXml.hpp" ]; then
    HAS_DBXML_HEADERS=1
fi
if [ -f "${OEM_WORK_DIR}/dbxml/install/lib/libdb_cxx.so" ]; then
    HAS_DBCXX_LIB=1
fi
if [ -f "${OEM_WORK_DIR}/dbxml/install/lib/libdbxml.so" ]; then
    HAS_DBXML_LIB=1
fi
if [ -f "${OEM_WORK_DIR}/dbxml/install/lib/libxqilla.so" ]; then
    HAS_XQILLA_LIB=1
fi
echo "Has DBXML headers: $HAS_DBXML_HEADERS"
echo "Has DB C++ Library: $HAS_DBCXX_LIB"
echo "Has DBXML Library: $HAS_DBXML_LIB"
echo "Has XQilla Library: $HAS_XQILLA_LIB"

# (Re)build DBXML if any of the above is false
if test $HAS_DBXML_HEADERS -eq 0 -o $HAS_DBCXX_LIB -eq 0 -o $HAS_DBXML_LIB -eq 0 -o $HAS_XQILLA_LIB -eq 0; then
    echo "Re-building DBXML (${BUILD_CONFIG})"
    cp -Rf "${SOURCE_DIR}/Oem/dbxml" "${OEM_WORK_DIR}/dbxml"
    cd "${OEM_WORK_DIR}/dbxml" || exit
    if test $HAVE_SYSTEM_XERCES -eq 1; then
        echo "Building DBXML with system-installed xerces. Please wait"
        if test "$BUILD_CONFIG" == Debug; then
            ./buildall.sh --enable-debug --with-xerces=/usr --have-system-xerces 2>&1 | tee "$OEM_WORK_DIR/dbxml_build.log"
            check_build
        else
            ./buildall.sh --with-xerces=/usr --have-system-xerces 2>&1 | tee "$OEM_WORK_DIR/dbxml_build.log"
            check_build
        fi
    else
        echo "Building DBXML with internal copy of xerces. Please wait"
        if test "$BUILD_CONFIG" == Debug; then
            ./buildall.sh --enable-debug 2>&1 | tee "$OEM_WORK_DIR/dbxml_build.log"
            check_build
        else
            ./buildall.sh 2>&1 | tee "$OEM_WORK_DIR/dbxml_build.log"
            check_build
        fi
    fi
fi

OUT_INTERNAL_ACE=FALSE
if [ "$INTERNAL_ACE" = "1" ]; then
    OUT_INTERNAL_ACE=TRUE
fi
OUT_INTERNAL_CPPUNIT=FALSE
if [ "$INTERNAL_CPPUNIT" = "1" ]; then
    OUT_INTERNAL_CPPUNIT=TRUE
fi
OUT_INTERNAL_ZLIB=FALSE
if [ "$INTERNAL_ZLIB" = "1" ]; then
    OUT_INTERNAL_ZLIB=TRUE
fi
OUT_INTERNAL_LIBPNG=FALSE
if [ "$INTERNAL_LIBPNG" = "1" ]; then
    OUT_INTERNAL_LIBPNG=TRUE
fi
OUT_INTERNAL_JPEG=FALSE
if [ "$INTERNAL_JPEG" = "1" ]; then
    OUT_INTERNAL_JPEG=TRUE
fi
OUT_INTERNAL_FREETYPE=FALSE
if [ "$INTERNAL_FREETYPE" = "1" ]; then
    OUT_INTERNAL_FREETYPE=TRUE
fi
OUT_INTERNAL_GD=FALSE
if [ "$INTERNAL_GD" = "1" ]; then
    OUT_INTERNAL_GD=TRUE
fi
OUT_INTERNAL_JSON=FALSE
if [ "$INTERNAL_JSON" = "1" ]; then
    OUT_INTERNAL_JSON=TRUE
fi
OUT_INTERNAL_GEOS=FALSE
if [ "$INTERNAL_GEOS" = "1" ]; then
    OUT_INTERNAL_GEOS=TRUE
fi

# dump vars to setup script
{
    echo "#!/bin/sh"
    echo "export BUILD_CPU=$BUILD_CPU"
    echo "export BUILD_CONFIG=$BUILD_CONFIG"
    echo "export HAVE_SYSTEM_XERCES=$HAVE_SYSTEM_XERCES"
    echo "export INTERNAL_ACE=$OUT_INTERNAL_ACE"
    echo "export INTERNAL_CPPUNIT=$OUT_INTERNAL_CPPUNIT"
    echo "export INTERNAL_ZLIB=$OUT_INTERNAL_ZLIB"
    echo "export INTERNAL_LIBPNG=$OUT_INTERNAL_LIBPNG"
    echo "export INTERNAL_JPEG=$OUT_INTERNAL_JPEG"
    echo "export INTERNAL_FREETYPE=$OUT_INTERNAL_FREETYPE"
    echo "export INTERNAL_GD=$OUT_INTERNAL_GD"
    echo "export INTERNAL_JSON=$OUT_INTERNAL_JSON"
    echo "export INTERNAL_GEOS=$OUT_INTERNAL_GEOS"
    echo "export PHP_JPEG_DIR=$PHP_JPEG_DIR"
    echo "export PHP_PNG_DIR=$PHP_PNG_DIR"
    echo "export PHP_FREETYPE_DIR=$PHP_FREETYPE_DIR"
    echo "export PHP_ZLIB_DIR=$PHP_ZLIB_DIR"
} > "$OEM_WORK_DIR/env_vars.sh"
chmod +x "$OEM_WORK_DIR/env_vars.sh"
cp -f "$OEM_WORK_DIR/env_vars.sh" "$OEM_WORK_DIR/env_vars.stage1.sh"

if [ "$USE_CCACHE" = "1" ]; then
    ccache -s
fi

echo "$OEM_WORK_DIR/env_vars.sh written"
echo "Make sure to run cmake_linuxapt.sh next with --oem-working-dir set to: $OEM_WORK_DIR"