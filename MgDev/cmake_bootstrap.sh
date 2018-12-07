#!/bin/sh

MG_INST_PREFIX=/usr/local/mapguideopensource-3.3.0
OEM_WORK_DIR=~/mapguide_oem_build
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
    cp -Rf Oem/dbxml "${OEM_WORK_DIR}/dbxml"
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
    cd "${PHP_WORKDIR}" || exit
    echo "Configuring PHP"
    ./configure --prefix="${MG_INSTALL_WEB_PREFIX}/php" --with-apxs2="${MG_INSTALL_WEB_PREFIX}/apache2/bin/apxs" --with-openssl --with-curl --enable-xml --enable-wddx --enable-shared --with-zlib --enable-zip --enable-mbstring=all --with-xsl=/usr/lib --with-gd --with-png-dir=/usr/lib --with-jpeg-dir=/usr/lib --with-freetype-dir=/usr/lib 2>&1 | tee "$OEM_WORK_DIR/configure_php.log"
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
} > "$OEM_WORK_DIR/env_vars.sh"
chmod +x "$OEM_WORK_DIR/env_vars.sh"

if [ "$USE_CCACHE" = "1" ]; then
    ccache -s
fi

echo "$OEM_WORK_DIR/env_vars.sh written"
echo "Make sure to run cmake_build.sh with --oem-working-dir set to: $OEM_WORK_DIR"