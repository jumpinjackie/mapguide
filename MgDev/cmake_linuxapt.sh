#!/bin/sh

# TODO: The MapGuide build system is not aware of FDO (beyond the built FDO binaries), so there is nothing here 
# that knows that FDO's build system has an internally built copy of openssl/curl that can be passed on to PHP 
# here.

# BOGUS:
#
# All I wanted was to adhere to the CMake philsophy as much as possible: Build everything non-elevated, outside of the source tree. Only needing
# elevation when it came time to actually "install" the various built binaries.
#
# But noooooooooooooooooo ...
#
# Building PHP and Tomcat Connector requires the apxs tool from httpd, which will only work when I make install httpd first, which on most Linux distros
# (that aren't inside docker containers) will generally require elevation from non-root users, and because this is done within this script, it means
# that the root-elevation will "infect" *all other binaries" built from this script!!!
#
# So in the interest of minimizing the possible damage, this triplet of sources (httpd/php/tomcat-connector) are built by this very script and 
# thus this is the only script that requires elevation if not running as root (and/or you are not running this from within a docker container)
#
# All other cmake_*.sh scripts can remain being run as-is without elevation

MG_INST_PREFIX=/usr/local/mapguideopensource
OEM_WORK_DIR=~/mapguide_oem_build
LINUXAPT_WORK_DIR=~/mapguide_linuxapt_build
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
        --working-dir)
            LINUXAPT_WORK_DIR="$2"
            shift
            ;;
        # "infecting" our ccache with root-elevated cached object files is probably a bad look
        # especially if we need to non-elevated ccache builds further down the road
        #--with-ccache)
        #    USE_CCACHE=1
        #    ;;
        --help)
            echo "Usage: $0 (options)"
            echo "Options:"
            echo "  --prefix [install prefix, default: /usr/local/mapguideopensource]"
            echo "  --oem-working-dir [working directory, default: ~/mapguide_oem_build]"
            echo "  --working-dir [working directory, default: ~/mapguide_linuxapt_build]"
            echo "  --help [Display usage]"
            #echo "  --with-ccache [Use ccache]"
            exit
            ;;
    esac
    shift   # Check next set of parameters.
done

SOURCE_DIR=$(pwd)

check_build()
{
    error=$?
    echo "Error code is currently: $error"
    if [ $error -ne 0 ]; then
        echo "$LIB_NAME: Error build failed ($error)................."
        # Return back to this dir before bailing
        cd "$SOURCE_DIR" || exit
        exit $error
    fi
}

echo "Checking if OEM working dir exists"
if [ ! -d "$OEM_WORK_DIR" ]; then
    echo "OEM working dir not found"
    exit 1
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

echo "Current source dir is: $SOURCE_DIR"
echo "Building for: $BUILD_CPU-bit"
echo "Building in: $BUILD_CONFIG"
echo "Using system xerces: $HAVE_SYSTEM_XERCES"
echo "Oem Working Directory is: $OEM_WORK_DIR"
echo "LinuxApt Working Directory is: $LINUXAPT_WORK_DIR"
echo "Using ccache: $USE_CCACHE"

echo "PHP_JPEG_DIR      = $PHP_JPEG_DIR"
echo "PHP_PNG_DIR       = $PHP_PNG_DIR"
echo "PHP_FREETYPE_DIR  = $PHP_FREETYPE_DIR"
echo "PHP_ZLIB_DIR      = $PHP_ZLIB_DIR"

LIBNAME=lib
if [ "$BUILD_CPU" = "64" ]; then
    LIBNAME=lib64
fi

if [ "$USE_CCACHE" = "1" ]; then
    export CC="ccache gcc"
    export CXX="ccache g++"
    ccache -s
fi

echo "Checking if LinuxApt working dir exists"
if [ ! -d "$LINUXAPT_WORK_DIR" ]; then
    mkdir -p "$LINUXAPT_WORK_DIR"
    echo "Created LinuxApt working dir at: $LINUXAPT_WORK_DIR"
fi

PHP_VER=5.6.40
HTTPD_VER=2.4.37
TC_VER=1.2.46
APR_VER=1.6.5
APR_UTIL_VER=1.6.1

HTTPD_PORT=8008
TOMCAT_PORT=8009

MG_INSTALL_WEB_PREFIX=$MG_INST_PREFIX/webserverextensions

LA_WORKDIR=${LINUXAPT_WORK_DIR}/LinuxApt
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
    echo "Extracting httpd tarballs"
    tar -jxf httpd-${HTTPD_VER}.tar.bz2 -C "${LA_WORKDIR}"
    #tar -jxf httpd-${HTTPD_VER}-deps.tar.bz2 -C "${LA_WORKDIR}"
    if [ ! -d "${LA_WORKDIR}/httpd-$HTTPD_VER/srclib/apr" ]; then
        echo "Extracting apr tarball"
        tar -jxf apr-$APR_VER.tar.bz2 -C "${LA_WORKDIR}"
        mv "${LA_WORKDIR}/apr-$APR_VER" "${LA_WORKDIR}/httpd-$HTTPD_VER/srclib/apr"
    fi
    if [ ! -d "${LA_WORKDIR}/httpd-$HTTPD_VER/srclib/aprutil" ]; then
        echo "Extracting apr-util tarball"
        tar -jxf apr-util-$APR_UTIL_VER.tar.bz2 -C "${LA_WORKDIR}"
        mv "${LA_WORKDIR}/apr-util-$APR_UTIL_VER" "${LA_WORKDIR}/httpd-$HTTPD_VER/srclib/apr-util"
    fi
    cd "${HTTPD_WORKDIR}" || exit
    echo "Configuring HTTPD"
    ./configure --prefix="${MG_INSTALL_WEB_PREFIX}/apache2" --enable-mods-shared-all --with-included-apr --with-port=${HTTPD_PORT} 2>&1 | tee "$LINUXAPT_WORK_DIR/configure_httpd_fake.log"
    check_build
    echo "Building HTTPD. Please wait"
    make 2>&1 | tee "$LINUXAPT_WORK_DIR/make_httpd_fake.log"
    check_build
fi

cd "${HTTPD_WORKDIR}" || exit
echo "Installing HTTPD"
make install 2>&1 | tee "$LINUXAPT_WORK_DIR/make_install_httpd_fake.log"
check_build
echo "Include conf/mapguide/*.conf" >> "${MG_INSTALL_WEB_PREFIX}/apache2/conf/httpd.conf"
# Activate mod_rewrite
sed -i "s/#LoadModule rewrite_module modules\/mod_rewrite.so/LoadModule rewrite_module modules\/mod_rewrite.so/g" "${MG_INSTALL_WEB_PREFIX}/apache2/conf/httpd.conf"
# Make sure lib dirs are in envvars
cat >> "${MG_INSTALL_WEB_PREFIX}/apache2/bin/envvars" <<END-OF-ENVVARS
export LD_LIBRARY_PATH=${MG_INSTALL_WEB_PREFIX}/${LIBNAME}:${MG_INSTALL_WEB_PREFIX}/php/lib:${MG_INST_PREFIX}/${LIBNAME}
export MENTOR_DICTIONARY_PATH=${MG_INST_PREFIX}/share/gis/coordsys
END-OF-ENVVARS

if test $HAS_PHP -eq 0; then
    cd "$SOURCE_DIR/Oem/LinuxApt" || exit
    echo "Extracting PHP tarball"
    tar -jxf php-${PHP_VER}.tar.bz2 -C "${LA_WORKDIR}"

    echo "Configuring PHP"
    cd "${PHP_WORKDIR}" || exit
    # TODO: Patch out the use of meta_ccld so ccache can work: https://bugs.php.net/bug.php?id=75940
    # if/when we finally move to PHP 7, this patch is already applied
    ./configure --prefix="${MG_INSTALL_WEB_PREFIX}/php" --with-apxs2="${MG_INSTALL_WEB_PREFIX}/apache2/bin/apxs" --with-openssl --with-curl --enable-xml --enable-wddx --enable-shared --with-zlib-dir="$PHP_ZLIB_DIR" --enable-zip --enable-mbstring=all --with-xsl=/usr/lib --with-gd --with-png-dir="$PHP_PNG_DIR" --with-jpeg-dir="$PHP_JPEG_DIR" --with-freetype-dir="$PHP_FREETYPE_DIR" 2>&1 | tee "$LINUXAPT_WORK_DIR/configure_php.log"
    check_build
    echo "Building PHP. Please wait"
    make 2>&1 | tee "$LINUXAPT_WORK_DIR/make_php.log"
    check_build
fi

cd "${PHP_WORKDIR}" || exit
echo "Installing PHP"
make install 2>&1 | tee "$LINUXAPT_WORK_DIR/make_install_php.log"
check_build

if test $HAS_TOMCAT_CONNECTOR -eq 0; then
    cd "$SOURCE_DIR/Oem/LinuxApt" || exit
    echo "Extracting Tomcat Connector tarball"
    tar -zxf tomcat-connectors-${TC_VER}-src.tar.gz -C "${LA_WORKDIR}"
    cd "${TC_WORKDIR}/native" || exit
    echo "Configuring Tomcat Connector"
    ./configure --with-apxs="${MG_INSTALL_WEB_PREFIX}/apache2/bin/apxs" 2>&1 | tee "$LINUXAPT_WORK_DIR/configure_tc.log"
    check_build
    echo "Building Tomcat Connector. Please wait"
    make 2>&1 | tee "$LINUXAPT_WORK_DIR/make_tc.log"
    check_build
fi

echo "Installing Tomcat Connector"
cd "${TC_WORKDIR}/native" || exit
make install 2>&1 | tee "$LINUXAPT_WORK_DIR/make_install_tc.log"
check_build

echo "Installing Fusion"
cd "${SOURCE_DIR}/Oem/fusion" || exit
ant deploy -Ddeploy.home=${MG_INSTALL_WEB_PREFIX}/www/fusion
chown daemon:daemon "${MG_INSTALL_WEB_PREFIX}/www/fusion/lib/tcpdf/cache"
cp -R "${SOURCE_DIR}/Oem/fusionMG/templates" "${MG_INSTALL_WEB_PREFIX}/www/fusion"
# Since we're here, set up the temp dir and chown that as well
mkdir -p "${MG_INSTALL_WEB_PREFIX}/Temp"
chown daemon:daemon "${MG_INSTALL_WEB_PREFIX}/Temp"

cp -f "$OEM_WORK_DIR/env_vars.stage1.sh" "$OEM_WORK_DIR/env_vars.sh"
# append vars to setup script
{
    echo "export LINUXAPT_WORK_DIR=$LINUXAPT_WORK_DIR"
    echo "export HTTPD_WORKDIR=$HTTPD_WORKDIR"
    echo "export PHP_WORKDIR=$PHP_WORKDIR"
    echo "export TC_WORKDIR=$TC_WORKDIR"
    echo "export MG_INST_PREFIX=$MG_INST_PREFIX"
    echo "export PHP_VER=$PHP_VER"
    echo "export HTTPD_VER=$HTTPD_VER"
    echo "export TC_VER=$TC_VER"
    echo "export HTTPD_PORT=$HTTPD_PORT"
    echo "export TOMCAT_PORT=$TOMCAT_PORT"
} >> "$OEM_WORK_DIR/env_vars.sh"
cp -f "$OEM_WORK_DIR/env_vars.sh" "$OEM_WORK_DIR/env_vars.stage2.sh"

if [ "$USE_CCACHE" = "1" ]; then
    ccache -s
fi

echo "$OEM_WORK_DIR/env_vars.sh updated"
echo "Make sure to run cmake_build.sh with --oem-working-dir set to: $OEM_WORK_DIR"