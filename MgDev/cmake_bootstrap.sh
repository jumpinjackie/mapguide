#!/bin/sh

MG_INST_PREFIX=/usr/local/mapguideopensource-3.3.0
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
        --prefix)
            MG_INST_PREFIX="$2"
            shift
            ;;
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
            echo "  --prefix [install prefix]"
            echo "  --oem-working-dir [installation directory]"
            echo "  --build [32(default)|64]"
            echo "  --config [Release(default)|Debug]"
            echo "  --have-system-xerces [Use system-installed xerces]"
            echo "  --help [Display usage]"
            echo "  --with-internal-ace [Build/Use internal ACE]"
            echo "  --with-internal-cppunit [Build/Use internal CppUnit]"
            echo "  --with-internal-zlib [Build/Use internal zlib]"
            echo "  --with-internal-jpeg [Build/Use internal jpeg]"
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

OEM_INSTALL_STAGE="${OEM_WORK_DIR}/install"
mkdir -p "$OEM_INSTALL_STAGE"

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
    sh ./configure --enable-silent-rules --prefix="${MG_INST_PREFIX}" LDFLAGS="-ldl"
    make && make install
    check_build
fi
mkdir -p "${OEM_WORK_DIR}/gd"
if [ "$INTERNAL_ZLIB" = "1" ]; then
    echo "Building zlib (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/zlib" "${OEM_WORK_DIR}/gd/zlib"
    cd "${OEM_WORK_DIR}/gd/zlib" || exit
    sh ./configure --prefix="${OEM_INSTALL_STAGE}/zlib"
    make && make install
    check_build
fi
if [ "$INTERNAL_LIBPNG" = "1" ]; then
    echo "Building libpng (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/lpng" "${OEM_WORK_DIR}/gd/lpng"
    cd "${OEM_WORK_DIR}/gd/lpng" || exit
    # Need to ensure these directories exist first for our custom prefix to work
    mkdir -p "${OEM_INSTALL_STAGE}/libpng"
    mkdir -p "${OEM_INSTALL_STAGE}/libpng/usr/local"
    cp scripts/makefile.std makefile
    if [ $BUILD_CPU -eq 64 ]; then
        #Inject -fPIC to CFLAGS for 64-bit
        sed 's/^CFLAGS=/CFLAGS= -fPIC -m64 /g' makefile > makefile64
        make -fmakefile64 DESTDIR="${OEM_INSTALL_STAGE}/libpng"
        make -fmakefile64 DESTDIR="${OEM_INSTALL_STAGE}/libpng" install
        check_build
    else
        make && make DESTDIR="${OEM_INSTALL_STAGE}/libpng" install
        check_build
    fi
fi
if [ "$INTERNAL_JPEG" = "1" ]; then
    echo "Building libjpeg (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/jpeg" "${OEM_WORK_DIR}/gd/jpeg"
    cd "${OEM_WORK_DIR}/gd/jpeg" || exit
    # Need to ensure these directories exist first for our custom prefix to work
    mkdir -p "${OEM_INSTALL_STAGE}/libjpeg/bin"
    mkdir -p "${OEM_INSTALL_STAGE}/libjpeg/man/man1"
    # A jconfig.h that is not modifed by ./configure will trip up make, so nuke it first
    rm -f jconfig.h
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --prefix="${OEM_INSTALL_STAGE}/libjpeg"
        #--with-pic does nothing (probably ancient configure script), so do some sed trickery
        #to inject this flag. Know a better way? Enlighten us :)
        sed 's/^CFLAGS=/CFLAGS= -fPIC -m64 /g' Makefile > Makefile64
        make -fMakefile64
        check_build
        make -fMakefile64 install
        check_build
    else
        sh ./configure --enable-static --disable-shared --prefix="${OEM_INSTALL_STAGE}/libjpeg"
        make
        check_build
        make install
        check_build
    fi
    # Make some symlinks so that PHP can work with this
    ln -s "${OEM_WORK_DIR}/gd/jpeg" "${OEM_INSTALL_STAGE}/libjpeg/include"
    ln -s "${OEM_WORK_DIR}/gd/jpeg/.libs" "${OEM_INSTALL_STAGE}/libjpeg/lib"
fi
if [ "$INTERNAL_FREETYPE" = "1" ]; then
    echo "Building freetype (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/freetype" "${OEM_WORK_DIR}/gd/freetype"
    cd "${OEM_WORK_DIR}/gd/freetype" || exit
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --with-pic --prefix="${OEM_INSTALL_STAGE}/freetype"
    else
    	sh ./configure --enable-static --disable-shared --prefix="${OEM_INSTALL_STAGE}/freetype"
    fi
    make && make install
    check_build
fi
if [ "$INTERNAL_GD" = "1" ]; then
    echo "Building GD (internal)"
    cp -Rf "${SOURCE_DIR}/Oem/gd/gd" "${OEM_WORK_DIR}/gd/gd"
    cd "${OEM_WORK_DIR}/gd/gd" || exit
    if [ $BUILD_CPU -eq 64 ]; then
        sh ./configure --enable-static --disable-shared --without-fontconfig --enable-silent-rules --with-pic --prefix="${OEM_INSTALL_STAGE}/gd"
    else
        sh ./configure --enable-static --disable-shared --without-fontconfig --enable-silent-rules --prefix="${OEM_INSTALL_STAGE}/gd"
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

PHP_VER=5.6.33
HTTPD_VER=2.4.29
TC_VER=1.2.42

HTTPD_PORT=8008
TOMCAT_PORT=8009

MG_INSTALL_WEB_PREFIX=$MG_INST_PREFIX/webserverextensions

LA_WORKDIR=${OEM_WORK_DIR}/LinuxApt
HTTPD_WORKDIR=${LA_WORKDIR}/httpd-${HTTPD_VER}
PHP_WORKDIR=${LA_WORKDIR}/php-${PHP_VER}
TC_WORKDIR=${LA_WORKDIR}/tomcat-connectors-${TC_VER}-src

if [ ! -d "${LA_WORKDIR}" ]; then
    mkdir -p "$LA_WORKDIR"
fi

echo "Checking if we need to build httpd"
HAS_HTTPD=0
if [ -d "${HTTPD_WORKDIR}" ]; then
    HAS_HTTPD=1
fi
echo "Checking if we need to build php"
HAS_PHP=0
if [ -d "${PHP_WORKDIR}" ]; then
    HAS_PHP=1
fi
echo "Checking if we need to build tomcat connector"
HAS_TOMCAT_CONNECTOR=0
if [ -d "${TC_WORKDIR}" ]; then
    HAS_TOMCAT_CONNECTOR=1
fi
echo "Has HTTPD: ${HAS_HTTPD}"
echo "Has PHP: ${HAS_PHP}"
echo "Has Tomcat Connector: ${HAS_TOMCAT_CONNECTOR}"
if test $HAS_HTTPD -eq 0; then
    cd "$SOURCE_DIR/Oem/LinuxApt" || exit
    echo "Extracting HTTPD tarballs"
    tar -jxf httpd-${HTTPD_VER}.tar.bz2 -C "${LA_WORKDIR}"
    tar -jxf httpd-${HTTPD_VER}-deps.tar.bz2 -C "${LA_WORKDIR}"
    cd "${HTTPD_WORKDIR}" || exit
    echo "Configuring HTTPD"
    ./configure --prefix="${MG_INSTALL_WEB_PREFIX}/apache2" --enable-mods-shared-all --with-included-apr --with-port=${HTTPD_PORT} 2>&1 | tee "$OEM_WORK_DIR/configure_httpd_fake.log"
    check_build
    echo "Building HTTPD. Please wait"
    make 2>&1 | tee "$OEM_WORK_DIR/make_httpd_fake.log"
    check_build
fi
cd "${HTTPD_WORKDIR}" || exit
echo "Installing HTTPD"
make install 2>&1 | tee "$OEM_WORK_DIR/make_install_httpd_fake.log"
check_build
echo "Include conf/mapguide/*.conf" >> "${MG_INSTALL_WEB_PREFIX}/apache2/conf/httpd.conf"
# Activate mod_rewrite
sed -i "s/#LoadModule rewrite_module modules\/mod_rewrite.so/LoadModule rewrite_module modules\/mod_rewrite.so/g" "${MG_INSTALL_WEB_PREFIX}/apache2/conf/httpd.conf"
# Make sure lib dirs are in envvars
cat >> "${MG_INSTALL_WEB_PREFIX}/apache2/bin/envvars" <<END-OF-ENVVARS
export LD_LIBRARY_PATH=${MG_INSTALL_WEB_PREFIX}/lib:${MG_INSTALL_WEB_PREFIX}/php/lib:${MG_INST_PREFIX}/lib
export MENTOR_DICTIONARY_PATH=${MG_INST_PREFIX}/share/gis/coordsys
END-OF-ENVVARS

if test $HAS_PHP -eq 0; then
    cd "$SOURCE_DIR/Oem/LinuxApt" || exit
    echo "Extracting PHP tarball"
    tar -jxf php-${PHP_VER}.tar.bz2 -C "${LA_WORKDIR}"

    echo "Configuring PHP"
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
    cd "${PHP_WORKDIR}" || exit
    # TODO: Patch out the use of meta_ccld so ccache can work: https://bugs.php.net/bug.php?id=75940
    # if/when we finally move to PHP 7, this patch is already applied
    ./configure --prefix="${MG_INSTALL_WEB_PREFIX}/php" --with-apxs2="${MG_INSTALL_WEB_PREFIX}/apache2/bin/apxs" --with-openssl --with-curl --enable-xml --enable-wddx --enable-shared --with-zlib-dir="$PHP_ZLIB_DIR" --enable-zip --enable-mbstring=all --with-xsl=/usr/lib --with-gd --with-png-dir="$PHP_PNG_DIR" --with-jpeg-dir="$PHP_JPEG_DIR" --with-freetype-dir="$PHP_FREETYPE_DIR" 2>&1 | tee "$OEM_WORK_DIR/configure_php.log"
    check_build
    echo "Building PHP. Please wait"
    make 2>&1 | tee "$OEM_WORK_DIR/make_php.log"
    check_build
fi

cd "${PHP_WORKDIR}" || exit
echo "Installing PHP"
make install 2>&1 | tee "$OEM_WORK_DIR/make_install_php.log"
check_build

if test $HAS_TOMCAT_CONNECTOR -eq 0; then
    cd "$SOURCE_DIR/Oem/LinuxApt" || exit
    echo "Extracting Tomcat Connector tarball"
    tar -zxf tomcat-connectors-${TC_VER}-src.tar.gz -C "${LA_WORKDIR}"
    cd "${TC_WORKDIR}/native" || exit
    echo "Configuring Tomcat Connector"
    ./configure --with-apxs="${MG_INSTALL_WEB_PREFIX}/apache2/bin/apxs" 2>&1 | tee "$OEM_WORK_DIR/configure_tc.log"
    check_build
    echo "Building Tomcat Connector. Please wait"
    make 2>&1 | tee "$OEM_WORK_DIR/make_tc.log"
    check_build
fi
echo "Installing Tomcat Connector"
cd "${TC_WORKDIR}/native" || exit
make install 2>&1 | tee "$OEM_WORK_DIR/make_install_tc.log"
check_build

# dump vars to setup script
{
    echo "#!/bin/sh"
    echo "export MG_INST_PREFIX=$MG_INST_PREFIX"
    echo "export BUILD_CPU=$BUILD_CPU"
    echo "export BUILD_CONFIG=$BUILD_CONFIG"
    echo "export PHP_VER=$PHP_VER"
    echo "export HTTPD_VER=$HTTPD_VER"
    echo "export TC_VER=$TC_VER"
    echo "export HTTPD_PORT=$HTTPD_PORT"
    echo "export TOMCAT_PORT=$TOMCAT_PORT"
    echo "export HAVE_SYSTEM_XERCES=$HAVE_SYSTEM_XERCES"
    echo "export INTERNAL_ACE=$INTERNAL_ACE"
    echo "export INTERNAL_CPPUNIT=$INTERNAL_CPPUNIT"
    echo "export INTERNAL_ZLIB=$INTERNAL_ZLIB"
    echo "export INTERNAL_LIBPNG=$INTERNAL_LIBPNG"
    echo "export INTERNAL_JPEG=$INTERNAL_JPEG"
    echo "export INTERNAL_FREETYPE=$INTERNAL_FREETYPE"
    echo "export INTERNAL_GD=$INTERNAL_GD"
    echo "export INTERNAL_JSON=$INTERNAL_JSON"
    echo "export INTERNAL_GEOS=$INTERNAL_GEOS"
} > "$OEM_WORK_DIR/env_vars.sh"
chmod +x "$OEM_WORK_DIR/env_vars.sh"

if [ "$USE_CCACHE" = "1" ]; then
    ccache -s
fi

echo "$OEM_WORK_DIR/env_vars.sh written"
echo "Make sure to run cmake_build.sh with --oem-working-dir set to: $OEM_WORK_DIR"