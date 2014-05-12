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
FDO_VER_MAJOR=3
FDO_VER_MINOR=9
FDO_VER_REV=0

# Extra flags to pass to FDO build scripts
FDO_BUILD_FLAGS=
if [ $FDO_DEBUG -eq 1 ]; then
    FDO_BUILD_FLAGS="--config debug"
else
    FDO_BUILD_FLAGS="--config release"
fi

UBUNTU=0
FDO_CPU=x86
FDO_PLATFORM=32
FDO_BUILD_CPU=i386
LIB_DIRNAME=lib

# FDO install directory
FDO_VER_FULL=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
FDO_INST=/usr/local/fdo-${FDO_VER_FULL}

check_build()
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

FDO_SRC=/home/vagrant/fdo/branches/3.9
#FDO_SRC=http://svn.osgeo.org/fdo/trunk
FDO_BUILD_AREA=${BUILDROOT}/fdo_build_area
FDO_FILELIST=${FDO_BUILD_AREA}/install/filelist

shim_thirdparty_lib_paths()
{
    echo "[info]: Shimming include/lib paths"
    # Note:
    #
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
    mkdir -p ~/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU
    # PostgreSQL include path
    if [ ! -d ~/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/include ];
    then
        ln -s /usr/include/postgresql ~/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/include
        echo "[info]: Symlinked PostgreSQL include path"
    else
        echo "[info]: PostgreSQL include path already symlinked"
    fi
    # PostgreSQL lib path
    if [ ! -d ~/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/$LIB_DIRNAME ];
    then
        ln -s /usr/lib ~/fdo_rdbms_thirdparty_system/pgsql/$FDO_CPU/$LIB_DIRNAME
        echo "[info]: Symlinked PostgreSQL lib path"
    else
        echo "[info]: PostgreSQL lib path already symlinked"
    fi
    mkdir -p ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU
    # MySQL include path
    if [ ! -d ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/include ];
    then
        ln -s /usr/include/mysql ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/include
        echo "[info]: Symlinked MySQL include path"
    else
        echo "[info]: MySQL include path already symlinked"
    fi
    # MySQL lib path
    if [ ! -d ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$LIB_DIRNAME ];
    then
        if [ ${FDO_PLATFORM} -eq 32 ]; 
        then
            ln -s /usr/lib/i386-linux-gnu ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$LIB_DIRNAME
            echo "[info]: Symlinked MySQL lib path (x86)"
        else
            ln -s /usr/lib/x86_64-linux-gnu ~/fdo_rdbms_thirdparty_system/mysql/$FDO_CPU/$LIB_DIRNAME
            echo "[info]: Symlinked MySQL lib path (x64)"
        fi
    else
        echo "[info]: MySQL lib path already symlinked"
    fi
}

modify_sdk_paths()
{
    if [ ${UBUNTU} -eq 1 ];
    then
        echo "[info]: Updating setenvironment.sh"
        sed -i 's/export FDOMYSQL=$FDOTHIRDPARTY\/mysql\/rhlinux/export FDOMYSQL=\/home\/vagrant\/fdo_rdbms_thirdparty_system\/mysql\/'"$FDO_CPU"'/g' ${FDO_BUILD_AREA}/setenvironment.sh
        sed -i 's/export FDOPOSTGRESQL=$FDOTHIRDPARTY\/pgsql/export FDOPOSTGRESQL=\/home\/vagrant\/fdo_rdbms_thirdparty_system\/pgsql\/'"$FDO_CPU"'/g' ${FDO_BUILD_AREA}/setenvironment.sh

        # Also need to patch some FDO thirdparty build scripts to accept our non-default compiler
        #echo "[info]: Patching mkcatdefs build script"
        #sed -i 's/gcc -DLINUX -g/'"$CC"' -DLINUX -g/g' ${FDO_BUILD_AREA}/Thirdparty/linux/mkcatdefs/src/build
        #echo "[info]: Patching xalan/xerces build script"
        #sed -i 's/.\/runConfigure -p linux -P/.\/runConfigure -p linux -c '"$CC"' -x '"$CXX"' -P/g' ${FDO_BUILD_AREA}/Thirdparty/apache/build2.sha
        #echo "[info]: Patching boost build script"
        #sed -i 's/# using gcc : 3.2 : g++-3.2 ;/using gcc : '"$GCCVER"' : '"$CXX"' ;/g' ${FDO_BUILD_AREA}/Thirdparty/boost/tools/build/v2/user-config.jam
        #sed -i 's/.\/b2 toolset=gcc/.\/b2 toolset='"$CC"'/g' ${FDO_BUILD_AREA}/Thirdparty/boost/build.sh
    else
        echo "[info]: Updating setenvironment.sh"
        # Note: Change your paths here if they're different
        sed -i 's/export FDOMYSQL=$FDOTHIRDPARTY\/mysql\/rhlinux/export FDOMYSQL=\/home\/vagrant\/fdo_rdbms_thirdparty\/mysql\/'"$FDO_CPU"'/g' ${FDO_BUILD_AREA}/setenvironment.sh
        sed -i 's/export FDOPOSTGRESQL=$FDOTHIRDPARTY\/pgsql/export FDOPOSTGRESQL=\/home\/vagrant\/fdo_rdbms_thirdparty\/pgsql/g' ${FDO_BUILD_AREA}/setenvironment.sh
    fi
    echo "export FDOORACLE=/home/vagrant/fdo_rdbms_thirdparty/oracle/${FDO_CPU}/instantclient_11_2/sdk" >> ${FDO_BUILD_AREA}/setenvironment.sh
}

# Must have root
if [[ $EUID -ne 0 ]]; then
    echo "[error]: You must run this script with superuser privileges"
    exit 1
fi

echo "***********************************************************"
echo " FDO Source: ${FDO_SRC}"
echo " FDO Build Area: ${FDO_BUILD_AREA}"
echo " FDO Install dir: ${FDO_INST}"
echo " FDO CPU Target: ${FDO_BUILD_CPU}"
echo " CMake build: ${CMAKE}"
echo " Is Ubuntu?: ${UBUNTU}"
echo " Debug build: ${FDO_DEBUG}"
echo " Export from local SVN checkout: ${LOCALSVN}"
echo " Re-use previous build area: ${PRESERVE_BUILD_ROOT}"
echo "***********************************************************"
start_time=`date +%s`
REVISION=`svn info ${FDO_SRC} | perl revnum.pl`

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
fi

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
source ./setenvironment.sh
if [ ! -d ${FDO_INST} ];
then
    echo "[error]: FDO install directory ${FDO_INST} doesn't exist. setenvironment.sh should've created this directory. Ensure the FDO version number in this script is correct"
    exit 1;
fi

FDO_BUILD_COMPONENT="FDO Thirdparty"
./build_thirdparty.sh -b ${FDO_PLATFORM} ${FDO_BUILD_FLAGS}
check_build

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
        ./build_linux.sh --w $comp --p ${FDO_INST} --b ${FDO_PLATFORM} ${FDO_BUILD_FLAGS}
        update_fdocore_file_list
        check_build
    done
    for comp in shp sqlite gdal ogr wfs wms rdbms kingoracle sdf
    do
        save_current_file_list
        FDO_BUILD_COMPONENT="$comp (automake)"
        ./build_linux.sh --w $comp --p ${FDO_INST} --b ${FDO_PLATFORM} ${FDO_BUILD_FLAGS}
        update_provider_file_list $comp
        check_build
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
    check_build

    FDO_BUILD_COMPONENT="Strip so symbols and remove execute flag"
    # Remove unneeded symbols from files in the lib directory
    # and make them non-executable
    for file in `find ${FDO_INST}/lib/lib*.so* -type f -print`
    do
        strip --strip-unneeded ${file}
        chmod a-x ${file}
    done
    check_build

    FDO_BUILD_COMPONENT="Make tarball"
    # Create a binary tar ball for FDO
    cd ${FDO_INST}
    tar -zcf ${BUILDROOT}/fdosdk-centos6-${FDO_BUILD_CPU}-${FDO_VER_FULL}_${REVISION}.tar.gz *
    check_build

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
echo Main build execution time: `expr $end_time - $start_time` s
