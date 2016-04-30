#!/bin/bash

# build_fdo.sh
#
# Builds FDO and creates a tarball from the binary output
#
# Modify setenvironment.sh to point to the correct Oracle, MySQL and PostgreSQL paths
# before building

FDO_DEBUG=0

FDO_BUILD_COMPONENT=

# FDO version. Make sure this matches your FDO build source
FDO_VER_MAJOR=${FDO_VER_MAJOR:-4}
FDO_VER_MINOR=${FDO_VER_MINOR:-0}
FDO_VER_REV=${FDO_VER_REV:-0}

FDO_DISTRO=${FDO_DISTRO:-centos6}

# FDO install directory
FDO_VER_FULL=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}

# Ubuntu distro switch
UBUNTU=${UBUNTU:-0}

if [ -z "${FDO_INST_PATH}" ]; then
    FDO_INST=/usr/local/fdo-${FDO_VER_FULL}
else
    FDO_INST=${FDO_INST_PATH}
fi

# Extra flags to pass to FDO build scripts
FDO_BUILD_CONF=
if [ $FDO_DEBUG -eq 1 ]; then
    FDO_BUILD_CONF=debug
else
    FDO_BUILD_CONF=release
fi

FDO_CPU=
FDO_BUILD_CPU=
FDO_LIB_DIRNAME=
FDO_PLATFORM=
if [ "${ARCH}" = "amd64" ] || [ $(uname -m) = "x86_64" ]; then
    FDO_CPU=x64
    FDO_BUILD_CPU=amd64
    FDO_LIB_DIRNAME=lib64
    FDO_PLATFORM=64
else
    FDO_CPU=x86
    FDO_BUILD_CPU=i386
    FDO_LIB_DIRNAME=lib
    FDO_PLATFORM=32
fi

echo "******************************************************************"
echo "FDO version:              ${FDO_VER_FULL}"
echo "FDO Platform:             ${FDO_PLATFORM}"
echo "CPU:                      ${FDO_CPU}"
echo "Arch:                     ${FDO_BUILD_CPU}"
echo "Lib Dir:                  ${FDO_LIB_DIRNAME}"
echo "FDO will be installed to: ${FDO_INST}"
echo "******************************************************************"

check_fdo_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[error]: ${FDO_BUILD_COMPONENT} - Error build failed ($error)"
        exit $error
    fi
}

check_fdo_lib()
{
    libname=$1-${FDO_VER_FULL}.so
    libpath=${FDO_INST}/lib/${libname}
    if [ ! -e ${libpath} ]; then
        echo "[error]: Error building ${libname}"
        exit 1
    fi
}

save_current_file_list()
{
    echo "[info]: Saving current FDO dir file list"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst > ${FDO_FILELIST}/orig.lst
    find . -type d -print | sort > ${FDO_FILELIST}/origdir.lst
    popd
}

update_fdocore_file_list()
{
    echo "[info]: Updating FDO core file list for deb packaging"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst > ${FDO_FILELIST}/fdocore.lst
    find . -type d -print | sort > ${FDO_FILELIST}/fdocoredir.lst
    popd
}

update_provider_file_list()
{
    PROVIDER=$1
    echo "[info]: Updating $PROVIDER file list for deb packaging"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    #mkdir -p $BUILDLIST
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    cat ${FDO_FILELIST}/orig.lst >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst | uniq -u > ${FDO_FILELIST}/${PROVIDER}.lst
    find . -type d -print | sort > ${FDO_FILELIST}/temp.lst
    cat ${FDO_FILELIST}/origdir.lst >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst | uniq -u > ${FDO_FILELIST}/${PROVIDER}dir.lst
    popd
}

BUILDROOT=`pwd`

LOCALSVN=1
PRESERVE_BUILD_ROOT=1
CMAKE=0

MY_HOME_DIR=$HOME
if [ "$FDO_HOME" != "" ]; then
    MY_HOME_DIR=$FDO_HOME
fi

FDO_SRC=${MY_HOME_DIR}/fdo/branches/${FDO_VER_MAJOR}.${FDO_VER_MINOR}
if [ "${FDO_BRANCH}" = "trunk" ]; then
    FDO_SRC=${MY_HOME_DIR}/fdo/trunk
fi
#FDO_SRC=http://svn.osgeo.org/fdo/trunk
FDO_BUILD_AREA=${BUILDROOT}/fdo_build_area
FDO_FILELIST=${FDO_BUILD_AREA}/install/filelist

shim_thirdparty_lib_paths()
{
    # Note: This is an Ubuntu-only code path
    echo "[info]: Shimming include/lib paths"
    # FDO assumes you're going to be linking against an SDK whose directory structure
    # is different from how system dev libraries are installed on Ubuntu, so we leverage the
    # power of symlinks to set up the expected directory structure that points to the system
    # headers and libraries, and modify setenvironment.sh to point to this shimmed directory
    # structure
    #
    # This structure assumes you've apt-get installed the following:
    #
    #   libmysqlclient-dev libpq-dev
    #
    mkdir -p ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU
    # PostgreSQL include path
    if [ ! -d ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/include ];
    then
        ln -s /usr/include/postgresql ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/include
        echo "[info]: Symlinked PostgreSQL include path"
    else
        echo "[info]: PostgreSQL include path already symlinked"
    fi
    # PostgreSQL lib path
    if [ ! -d ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/$FDO_LIB_DIRNAME ];
    then
        if [ ${FDO_PLATFORM} -eq 32 ];
        then 
            ln -s /usr/lib ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/$FDO_LIB_DIRNAME
            echo "[info]: Symlinked PostgreSQL lib path (x86)"
        else
            ln -s /usr/lib ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/$FDO_LIB_DIRNAME
            echo "[info]: Symlinked PostgreSQL lib path (x64)"
        fi
    else
        echo "[info]: PostgreSQL lib path already symlinked"
    fi
    mkdir -p ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU
    # MySQL include path
    if [ ! -d ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/include ];
    then
        ln -s /usr/include/mysql ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/include
        echo "[info]: Symlinked MySQL include path"
    else
        echo "[info]: MySQL include path already symlinked"
    fi
    # MySQL lib path
    if [ ! -d ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$FDO_LIB_DIRNAME ];
    then
        if [ ${FDO_PLATFORM} -eq 32 ]; 
        then
            ln -s /usr/lib/i386-linux-gnu ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$FDO_LIB_DIRNAME
            echo "[info]: Symlinked MySQL lib path (x86)"
        else
            ln -s /usr/lib/x86_64-linux-gnu ${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$FDO_LIB_DIRNAME
            echo "[info]: Symlinked MySQL lib path (x64)"
        fi
    else
        echo "[info]: MySQL lib path already symlinked"
    fi
}

modify_sdk_paths()
{
    rm -f $FDO_BUILD_AREA/setenvironment.sh

    if [ ${UBUNTU} -eq 1 ];
    then
        # GCC 4.8 is causing too much instability, so downgrade one version
        #GCCVER=4.7
        #export GCCVER
        #CC=gcc-$GCCVER
        #export CC
        #CXX=g++-$GCCVER
        #export CXX
        #echo "[info]: Using GCC $GCCVER for Ubuntu"
        shim_thirdparty_lib_paths
        
        # Nuke the existing copies of openssl and libcurl and replace them with directories
        # that symlink to system-installed headers/libs
        rm -rf $FDO_BUILD_AREA/Thirdparty/openssl
        rm -rf $FDO_BUILD_AREA/Thirdparty/libcurl
        
        # symlink libcurl to system installed copy
        mkdir -p $FDO_BUILD_AREA/Thirdparty/libcurl/include
        if [ ! -e $FDO_BUILD_AREA/Thirdparty/libcurl/include/curl ];
        then
            ln -s /usr/include/curl $FDO_BUILD_AREA/Thirdparty/libcurl/include/curl
        fi
        # Stub build.sh for libcurl
        echo "#!/bin/bash" > $FDO_BUILD_AREA/Thirdparty/libcurl/build.sh
        echo "exit 0" >> $FDO_BUILD_AREA/Thirdparty/libcurl/build.sh
        mkdir -p $FDO_BUILD_AREA/Thirdparty/libcurl/lib
        if [ ! -e $FDO_BUILD_AREA/Thirdparty/libcurl/lib/linux ];
        then
            if [ ${FDO_PLATFORM} -eq 32 ]; 
            then
                ln -s /usr/lib/i386-linux-gnu $FDO_BUILD_AREA/Thirdparty/libcurl/lib/linux
            else
                ln -s /usr/lib/x86_64-linux-gnu $FDO_BUILD_AREA/Thirdparty/libcurl/lib/linux
            fi
        fi

        # symlink openssl to system installed copy
        mkdir -p $FDO_BUILD_AREA/Thirdparty/openssl/include
        if [ ! -e $FDO_BUILD_AREA/Thirdparty/openssl/include/openssl ];
        then
            ln -s /usr/include/openssl $FDO_BUILD_AREA/Thirdparty/openssl/include/openssl
        fi
        # Stub openssl for libcurl
        echo "#!/bin/bash" > $FDO_BUILD_AREA/Thirdparty/openssl/build.sh
        echo "exit 0" >> $FDO_BUILD_AREA/Thirdparty/openssl/build.sh
        mkdir -p $FDO_BUILD_AREA/Thirdparty/openssl/lib
        if [ ! -e $FDO_BUILD_AREA/Thirdparty/openssl/lib/linux ];
        then
            if [ ${FDO_PLATFORM} -eq 32 ]; 
            then
                ln -s /usr/lib/i386-linux-gnu $FDO_BUILD_AREA/Thirdparty/openssl/lib/linux
            else
                ln -s /usr/lib/x86_64-linux-gnu $FDO_BUILD_AREA/Thirdparty/openssl/lib/linux
            fi
        fi
        echo "[info]: Replace internal openssl/libcurl with symlinks to Ubuntu-installed copies"
    fi

    # Rather than going through the hassle of modifying setenvironment.sh
    # Let's just inline the logic here and make the distro-specific changes
    echo "[info]: Setting environment variables for FDO"

    # Fully-qualfied location of the FDO files
    export FDO=$FDO_BUILD_AREA/Fdo
    if test ! -e "$FDO"; then
       echo ""
       echo "Invalid FDO path provided. "
       echo "The setenvironment script sets the default value to: "
       echo "$FDO"
       echo "Please modify the setenvironment.sh script with a valid path."
       echo ""
    fi

    # Fully-qualfied location of the FDO Utility files
    export FDOUTILITIES=$FDO_BUILD_AREA/Utilities
    if test ! -e "$FDOUTILITIES"; then
       echo ""
       echo "Invalid FDO Utilities path provided. "
       echo "The setenvironment script sets the default value to: "
       echo "$FDOUTILITIES" 
       echo ""
    fi

    # Fully-qualfied location of the FDO Thirdparty files
    #
    # Note: This value is completely disregarded and rewritten by FDO's configure script
    # but we still set it here as it forms the basis of other env vars below
    export FDOTHIRDPARTY=$FDO_BUILD_AREA/Thirdparty
    if test ! -e "$FDOTHIRDPARTY"; then
       echo ""
       echo "Invalid FDO Thirdparty path provided. "
       echo "The setenvironment script sets the default value to: "
       echo "$FDOTHIRDPARTY"
       echo ""
    fi

    # Fully-qualfied location of the ESRI ArcSDE SDK
    export SDEHOME=$FDOTHIRDPARTY/ESRI/ArcSDEClient931/Linux
    if test ! -e "$SDEHOME"; then
       echo ""
       echo "NOTE: The default location for the ArcSDE client SDK files"
       echo "was not found. The setenvironment script sets the default value to: "
       echo "$FDOTHIRDPARTY/ESRI/ArcSDEClient91/Linux. "
       echo ""
    fi

    # Fully-qualfied location of the GDAL Installation
    export FDOGDAL=$FDOTHIRDPARTY/gdal
       echo ""
       echo "NOTE: The setenvironment.sh script sets the installation location for "
       echo "the GDAL SDK files to $FDOTHIRDPARTY/gdal. "
       echo "If this value remains unchanged, the FDO build process will"
       echo "build the version of GDAL located in Thirdparty/gdal and will "
       echo "install the resulting libraries in $FDO_INST. The FDO build"
       echo "process will then use that location when building the GDAL and"
       echo "WMS providers. If you wish to build the FDO GDAL or WMS Providers"
       echo "using a previously installed version of GDAL, modify the setenvironment.sh "
       echo "script and set FDOGDAL to point to the existing GDAL installation."
       echo "For example: /user/local (The default GDAL installation path)."
    echo ""

    # Fully-qualfied location of the ODBC SDK
    export FDOODBC=/usr
    if test ! -e "$FDOODBC"; then
       echo ""
       echo "NOTE: The default path for the ODBC SDK files was not found. "
       echo "The setenvironment script sets the default value to: "
       echo "$FDOODBC"
       echo ""
    fi

    # Location of the PYTHON lib files. Typically in /usr/lib/pythonXXX
    export PYTHON_LIB_PATH=/usr/lib/python2.4
    if test ! -e "$PYTHON_LIB_PATH"; then
       echo ""
       echo "NOTE: The default path for the Python SDK lib files was not found. "
       echo "The setenvironment script sets the default value to: "
       echo "$PYTHON_LIB_PATH"
       echo "lib files."
       echo ""
    fi

    # Location of the PYTHON include files. Typically in /usr/include/pythonXXX
    export PYTHON_INCLUDE_PATH=/usr/include/python2.4
    if test ! -e "$PYTHON_INCLUDE_PATH"; then
       echo ""
       echo "NOTE: The default path for the Python SDK header files was not found. "
       echo "The setenvironment script sets the default value to: "
       echo "$PYTHON_INCLUDE_PATH"
       echo "include files."
       echo ""
    fi

    # Buildbot hack (mloskot): if the script is called with single dummy
    # parameter no installation directory is created, ie.:
    # $ source ./setenvironment.sh --noinstall
    if test ! $# -eq 1; then
        mkdir -p "$FDO_INST/lib"
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$FDO_INST/lib:$SDEHOME/lib
    fi

    export XERCESCROOT=$FDOTHIRDPARTY/apache/xerces
    export XALANCROOT=$FDOTHIRDPARTY/apache/xalan
    export NLSDIR=$XALANCROOT/src/xalanc/NLS
    export FDOORACLE=${MY_HOME_DIR}/fdo_rdbms_thirdparty/oracle/${FDO_CPU}/instantclient_11_2/sdk

    # Depending on distro, MySQL and PostgreSQL take different paths here
    if [ ${UBUNTU} -eq 1 ];
    then
        export FDOMYSQL=${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/mysql/${FDO_CPU}
        export FDOPOSTGRESQL=${MY_HOME_DIR}/fdo_rdbms_thirdparty_system/pgsql/${FDO_CPU}
        
        # Also need to patch some FDO thirdparty build scripts to accept our non-default compiler
        #echo "[info]: Patching mkcatdefs build script"
        #sed -i 's/gcc -DLINUX -g/'"$CC"' -DLINUX -g/g' ${FDO_BUILD_AREA}/Thirdparty/linux/mkcatdefs/src/build
        #echo "[info]: Patching xalan/xerces build script"
        #sed -i 's/.\/runConfigure -p linux -P/.\/runConfigure -p linux -c '"$CC"' -x '"$CXX"' -P/g' ${FDO_BUILD_AREA}/Thirdparty/apache/build2.sha
        #echo "[info]: Patching boost build script"
        #sed -i 's/# using gcc : 3.2 : g++-3.2 ;/using gcc : '"$GCCVER"' : '"$CXX"' ;/g' ${FDO_BUILD_AREA}/Thirdparty/boost/tools/build/v2/user-config.jam
        #sed -i 's/.\/b2 toolset=gcc/.\/b2 toolset='"$CC"'/g' ${FDO_BUILD_AREA}/Thirdparty/boost/build.sh
    else
        # Note: Change your paths here if they're different
        export FDOMYSQL=${MY_HOME_DIR}/fdo_rdbms_thirdparty/mysql/${FDO_CPU}
        export FDOPOSTGRESQL=${MY_HOME_DIR}/fdo_rdbms_thirdparty/pgsql
    fi

    # Check MySQL path
    if test ! -e "$FDOMYSQL"; then
       echo ""
       echo "NOTE: The default location for the MySQL SDK files "
       echo "was not found. The setenvironment script sets the default value to: "
       echo "$FDOTHIRDPARTY/mysql/rhlinux "
       echo "Your configured path was: $FDOMYSQL"
       echo ""
    fi

    # Check PostgreSQL path
    if test ! -e "$FDOPOSTGRESQL"; then
       echo ""
       echo "NOTE: The default path for the PostgreSQL SDK files was not found. "
       echo "The setenvironment script sets the default value to: "
       echo "$FDOPOSTGRESQL"
       echo "Your configured path was: $FDOPOSTGRESQL"
       echo ""
    fi

    echo "******* Environment variable summary *********"
    echo "FDO:                 $FDO"
    echo "FDOUTILITIES:        $FDOUTILITIES"
    echo "FDOTHIRDPARTY:       $FDOTHIRDPARTY"
    echo "SDEHOME:             $SDEHOME"
    echo "FDOGDAL:             $FDOGDAL"
    echo "FDOODBC:             $FDOODBC"
    echo "PYTHON_LIB_PATH:     $PYTHON_LIB_PATH"
    echo "PYTHON_INCLUDE_PATH: $PYTHON_INCLUDE_PATH"
    echo "XERCESCROOT:         $XERCESCROOT"
    echo "XALANCROOT:          $XALANCROOT"
    echo "NLSDIR:              $NLSDIR"
    echo "FDOORACLE:           $FDOORACLE"
    echo "FDOMYSQL:            $FDOMYSQL"
    echo "FDOPOSTGRESQL:       $FDOPOSTGRESQL"
    echo "**********************************************"
    echo ""
}

# Must have root
if [[ $EUID -ne 0 ]]; then
    echo "[error]: You must run this script with superuser privileges"
    exit 1
fi

echo "***********************************************************"
echo " FDO Source:                     ${FDO_SRC}"
echo " FDO Build Area:                 ${FDO_BUILD_AREA}"
echo " FDO Install dir:                ${FDO_INST}"
echo " FDO CPU Target:                 ${FDO_BUILD_CPU}"
echo " FDO build config:               ${FDO_BUILD_CONF}"
echo " CMake build:                    ${CMAKE}"
echo " Is Ubuntu?:                     ${UBUNTU}"
echo " Debug build:                    ${FDO_DEBUG}"
echo " Export from local SVN checkout: ${LOCALSVN}"
echo " Re-use previous build area:     ${PRESERVE_BUILD_ROOT}"
echo "***********************************************************"
start_time=`date +%s`
REVISION=`svn info ${FDO_SRC} | perl revnum.pl`

if [ -d ${FDO_INST} ];
then
    echo "[info]: Deleting directory ${FDO_INST} before build"
    rm -rf ${FDO_INST}
else
    echo "[info]: ${FDO_INST} doesn't exist. Continuing build"
fi

if [ ${CMAKE} -eq 1 ];
then
    echo "[error]: CMake build of FDO not supported yet"
    exit 1
else
    echo "[info]: Using automake build"
    if [ -d ${FDO_BUILD_AREA} ];
    then
        if [ ${PRESERVE_BUILD_ROOT} -eq 1 ];
        then
            echo "[info]: FDO build area ${FDO_BUILD_AREA} exists. Going straight to build"
            modify_sdk_paths
        else
            echo "[info]: Removing old FDO build area at ${FDO_BUILD_AREA}"
            rm -rf ${FDO_BUILD_AREA}
            if [ ${LOCALSVN} -eq 1 ] 
            then
                svn export -q ${FDO_SRC} ${FDO_BUILD_AREA}
                modify_sdk_paths
            else
                echo "[info]: Performing fresh SVN export of ${FDO_SRC} (r${REVISION}) to ${FDO_BUILD_AREA}"
                svn export -q -r ${REVISION} ${FDO_SRC} ${FDO_BUILD_AREA}
                modify_sdk_paths
            fi
        fi
    else
        echo "[info]: FDO build area ${FDO_BUILD_AREA} does not exist. Doing svn export"
        echo "[info]: Exporting svn revision ${REVISION}"
        if [ ${LOCALSVN} -eq 1 ] 
        then
            svn export -q ${FDO_SRC} ${FDO_BUILD_AREA}
            modify_sdk_paths
        else
            echo "[info]: Performing fresh SVN export of ${FDO_SRC} (r${REVISION}) to ${FDO_BUILD_AREA}"
            svn export -q -r ${REVISION} ${FDO_SRC} ${FDO_BUILD_AREA}
            modify_sdk_paths
        fi
    fi
fi

echo "[info]: Building FDO (${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}) rev (${REVISION})"
cd ${FDO_BUILD_AREA}

FDO_BUILD_COMPONENT="FDO Thirdparty"
./build_thirdparty.sh -b ${FDO_PLATFORM} --c ${FDO_BUILD_CONF} --p ${FDO_INST}
check_fdo_build

if [ ${CMAKE} -eq 1 ];
then
    FDO_BUILD_COMPONENT="FDO (cmake)"
    echo "[error]: CMake build of FDO not supported yet"
    exit 1;
else
    #NOTE: We never build ArcSDE provider because we haven't paid the ESRI tax for their ArcSDE SDK
    for comp in fdocore fdo utilities
    do
        FDO_BUILD_COMPONENT="$comp (automake)"
        ./build_linux.sh --w $comp --p ${FDO_INST} --b ${FDO_PLATFORM} --c ${FDO_BUILD_CONF}
        update_fdocore_file_list
        check_fdo_build
    done
    for comp in shp sqlite gdal ogr wfs wms rdbms kingoracle sdf
    do
        save_current_file_list
        FDO_BUILD_COMPONENT="$comp (automake)"
        ./build_linux.sh --w $comp --p ${FDO_INST} --b ${FDO_PLATFORM} --c ${FDO_BUILD_CONF}
        update_provider_file_list $comp
        check_fdo_build
    done
fi
check_fdo_lib libFDO
check_fdo_lib libExpressionEngine
check_fdo_lib libSDFProvider
check_fdo_lib libSHPProvider
check_fdo_lib libSHPOverrides
check_fdo_lib libWFSProvider
check_fdo_lib libWMSProvider
check_fdo_lib libWMSOverrides
# ArcSDE provider currently disabled due to missing libraries
#./buildfdoprovider.sh arcsde
#check_fdo_lib libArcSDEProvider
check_fdo_lib libFdoMySQL
check_fdo_lib libFdoPostgreSQL
check_fdo_lib libFdoODBC
check_fdo_lib libSchemaMgr_OV
check_fdo_lib libGRFPProvider
check_fdo_lib libGRFPOverrides
check_fdo_lib libOGRProvider
check_fdo_lib libKingOracleProvider
check_fdo_lib libKingOracleOverrides
check_fdo_lib libSQLiteProvider

end_time=`date +%s`

if [ $FDO_DEBUG -eq 0 ];
then
    FDO_BUILD_COMPONENT="Remove .la files from ${FDO_INST}"
    # Remove .la files from lib directory
    rm -f ${FDO_INST}/lib/*.la
    check_fdo_build

    FDO_BUILD_COMPONENT="Strip so symbols and remove execute flag"
    # Remove unneeded symbols from files in the lib directory
    # and make them non-executable
    for file in `find ${FDO_INST}/lib/lib*.so* -type f -print`
    do
        strip --strip-unneeded ${file}
        chmod a-x ${file}
    done
    check_fdo_build

    FDO_BUILD_COMPONENT="Make tarball"
    # Create a binary tar ball for FDO
    cd ${FDO_INST}
    tar -Jcf ${BUILDROOT}/fdosdk-${FDO_DISTRO}-${FDO_BUILD_CPU}-${FDO_VER_FULL}_${REVISION}.tar.xz *
    check_fdo_build

    if [ ${UBUNTU} -eq 1 ];
    then
        cd ${FDO_BUILD_AREA}/install
        dos2unix *
        ./dpkgall.sh ${FDO_BUILD_CPU} ${REVISION}
    fi
else
    echo "[info]: Not packaging FDO in debug mode"
fi

echo "[info]: FDO build complete!"
echo FDO main build execution time: `expr $end_time - $start_time` s
