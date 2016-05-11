#!/bin/bash

# provision_ubuntu.sh
#
# Entry point provisioning shell script
#
# Expected environment (set up by vagrant)
#
#  - Calling vagrantfile is for a Ubuntu >= 12.04 box
#  - Vagrant folder (path of the Vagrantfile) mounted as: /vagrant
#  - Source folder mounted as: /mapguide_sources
#     - Contains tarballs for MapGuide/FDO
#
# Required arguments (to be passed in by the Vagrantfile):
#
#  $1 - FDO major version
#  $2 - FDO minor version
#  $3 - FDO patch version
#  $4 - MG major version
#  $5 - MG minor version
#  $6 - MG patch version
#  $7 - FDO branch name (trunk|major.minor)
#  $8 - MG branch name (trunk|major.minor)
#  $9 - architecture (i386|amd64)
#  $10 - release label (eg. Beta1, RC1, Final)
#  $11 - distro label (eg. centos6, ubuntu12) - Optional
#  $12 - Run FDO tests (1|0)
#  $13 - Run MapGuide tests (1|0)
#  $14 - Vagrant folder [default: /vagrant]
#  $15 - Source folder [default: /mapguide_sources]

PROVISION_START_TIME=`date +%s`

echo "*****************************************************"
echo " Arg check"
echo "  1 - ${1}"
echo "  2 - ${2}"
echo "  3 - ${3}"
echo "  4 - ${4}"
echo "  5 - ${5}"
echo "  6 - ${6}"
echo "  7 - ${7}"
echo "  8 - ${8}"
echo "  9 - ${9}"
echo " 10 - ${10}"
echo " 11 - ${11}"
echo " 12 - ${12}"
echo " 13 - ${13}"
echo " 14 - ${14:-/vagrant}"
echo " 15 - ${15:-/mapguide_sources}"
echo "*****************************************************"

# FDO version
FDO_VER_MAJOR=${1}
FDO_VER_MINOR=${2}
FDO_VER_REV=${3}

# MG version
MG_VER_MAJOR=${4}
MG_VER_MINOR=${5}
MG_VER_REV=${6}

FDO_BRANCH=${7}
MG_BRANCH=${8}

MG_ARCH=${9}
MG_RELEASE_LABEL=${10}
DISTRO=${11:-ubuntu12}
FDO_DISTRO=${DISTRO}
MG_DISTRO=${DISTRO}
UBUNTU=1

TEST_FDO_FLAG=${12}
TEST_MG_FLAG=${13}

MOUNT_DIR=${14:-/vagrant}
SOURCES_DIR=${15:-/mapguide_sources}

MG_HOME=`pwd`
FDO_HOME=`pwd`

FDO_PLATFORM=
if [ "${MG_ARCH}" = "amd64" ]; then
    FDO_PLATFORM=64
elif [ "${MG_ARCH}" = "i386" ]; then
    FDO_PLATFORM=32
else
    echo "[error]: Unknown ARCH (${MG_ARCH})"
    exit 1
fi

# Export vars for sub-shells
export FDO_VER_MAJOR
export FDO_VER_MINOR
export FDO_VER_REV
export MG_VER_MAJOR
export MG_VER_MINOR
export MG_VER_REV
export FDO_BRANCH
export MG_BRANCH
export MG_ARCH
export UBUNTU
export FDO_DISTRO
export MG_DISTRO
export FDO_HOME
export MG_HOME

BUILD_COMPONENT=
check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[error]: ${BUILD_COMPONENT} - Error build failed ($error)"
        exit $error
    fi
}
check_test_generic()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)"
        echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)" >> $MG_HOME/unit_test_status.log
    fi
}
check_server_test()
{
    # As this will be called after a test command that is piped to tee, we need to read from $PIPESTATUS
    error=$PIPESTATUS
    if [ $error -ne 0 ]; then
        if [ $error -eq 124 ]; then # Test timed out
            echo "[warning]: ${BUILD_COMPONENT} - Unit tests failed to complete within the alotted time period. Test logs/results may or may not have been saved"
            echo "[warning]: ${BUILD_COMPONENT} - Unit tests failed to complete within the alotted time period. Test logs/results may or may not have been saved" >> $MG_HOME/unit_test_status.log
        else
            echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)"
            echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)" >> $MG_HOME/unit_test_status.log
        fi
    fi
}
if hash scons 2>/dev/null; then
    echo [provision]: We have scons
else
    echo [provision]: We do not have scons. Installing it
    sudo rpm -Uvh $SOURCES_DIR/scons-2.3.0-1.noarch.rpm
    check_build
fi
cd $MG_HOME
HAVE_FDO=0
HAVE_FDO_LIBS=0
HAVE_MAPGUIDE=0
MG_UNIT_TEST=0
FDO_UNIT_TEST=0

if [ "${TEST_FDO_FLAG}" = "1" ]; then
    FDO_UNIT_TEST=1
fi
if [ "${TEST_MG_FLAG}" = "1" ]; then
    MG_UNIT_TEST=1
fi

MAKE_FDO_SDK=0
SCRIPT_ROOT=$MG_HOME/scripts
FDO_SRC_ROOT=$MG_HOME/fdo/branches
FDO_SRC_DIR=${FDO_BRANCH}
FDO_SRC=$FDO_SRC_ROOT/$FDO_SRC_DIR
if [ "${FDO_BRANCH}" = "trunk" ]; then
    FDO_SRC_ROOT=$MG_HOME/fdo
    FDO_SRC=$FDO_SRC_ROOT/$FDO_SRC_DIR
fi
FDO_LIB_SRC=$MG_HOME/fdo_rdbms_thirdparty
MG_SRC_ROOT=$MG_HOME/mapguide/branches/${MG_BRANCH}
if [ "${MG_BRANCH}" = "trunk" ]; then
    MG_SRC_ROOT=$MG_HOME/mapguide/trunk
fi
MG_SRC=$MG_SRC_ROOT/MgDev
FDO_TARBALL=fdo-checkout-${FDO_BRANCH}.tar.gz
MG_TARBALL=mapguide-checkout-${MG_BRANCH}.tar.gz
FDO_INST_PATH=/usr/local/fdo-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
MG_INST_PATH=/usr/local/mapguideopensource-${MG_VER_MAJOR}.${MG_VER_MINOR}.${MG_VER_REV}

echo "********************************************************************************"
echo Home directory is `pwd`
echo "FDO version:      ${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}"
echo "FDO branch:       ${FDO_BRANCH}"
echo "MG version:       ${MG_VER_MAJOR}.${MG_VER_MINOR}.${MG_VER_REV}"
echo "MG branch:        ${MG_BRANCH}"
echo "Build target:     ${MG_DISTRO} - ${MG_ARCH}"
echo "Release Label:    ${MG_RELEASE_LABEL}"
echo "FDO Distro label: ${FDO_DISTRO}"
echo "Running FDO Tests: ${FDO_UNIT_TEST}"
echo "Running MapGuide Tests: ${MG_UNIT_TEST}"
echo "Checking directories"
echo "********************************************************************************"

if [ -d $FDO_SRC ];
then
    HAVE_FDO=1
fi
if [ -d $FDO_LIB_SRC ];
then
    HAVE_FDO_LIBS=1
fi
if [ -d $MG_SRC ];
then
    HAVE_MAPGUIDE=1
fi
if [ -f /usr/include/asm/atomic.h ];
then
    echo [provision]: atomic.h exists. Doing nothing
else
    echo [provision]: Copy atomic.h
    sudo mkdir -p /usr/include/asm
    sudo cp $SOURCES_DIR/atomic.h /usr/include/asm
fi
echo [provision]: Clean line endings
sudo dos2unix $MOUNT_DIR/*.sh
sudo dos2unix $MOUNT_DIR/*.pl
sudo dos2unix $MOUNT_DIR/*.txt
echo [provision]: Copying shell scripts
mkdir -p $SCRIPT_ROOT
cp -f $MOUNT_DIR/*.sh $SCRIPT_ROOT
cp -f $MOUNT_DIR/*.pl $SCRIPT_ROOT
cp -f $MOUNT_DIR/*.txt $SCRIPT_ROOT
cd $SCRIPT_ROOT
if [ $HAVE_FDO_LIBS -eq 0 ];
then
    echo [provision]: Extracting FDO thirdparty libs
    tar -zxf $SOURCES_DIR/fdo_rdbms_thirdparty.tar.gz -C $MG_HOME
fi
if [ $HAVE_FDO -eq 0 ];
then
    echo [provision]: Extracting FDO working copy
    mkdir -p $FDO_SRC_ROOT
    tar -zxf $SOURCES_DIR/$FDO_TARBALL -C $FDO_SRC_ROOT
fi
if [ $HAVE_MAPGUIDE -eq 0 ];
then
    echo [provision]: Extracting MapGuide working copy
    mkdir -p $MG_SRC_ROOT
    tar -zxf $SOURCES_DIR/$MG_TARBALL -C $MG_SRC_ROOT
fi
if [ -d $SOURCES_DIR/patches/fdo ];
then
    echo [provision]: Applying FDO source patches
    cp -R $SOURCES_DIR/patches/fdo/* $FDO_SRC
fi
if [ -d $SOURCES_DIR/patches/mapguide ];
then
    echo [provision]: Applying MapGuide source patches
    cp -R $SOURCES_DIR/patches/mapguide/* $MG_SRC
fi
# If Ubuntu > 12, we need to svn upgrade our working copies first
UBUNTU_MAJOR_VERSION=`lsb_release -r | awk '{print $2}' | sed 's/\./ /g' | awk '{print $1}'`
if [ $UBUNTU_MAJOR_VERSION -gt 12 ]; then
    echo [provison]: We are on Ubuntu >= 12 [`lsb_release -r`]
    svn upgrade $FDO_SRC
    svn upgrade $MG_SRC
fi
echo [provision]: Making sure swig is executable
chmod +x $MG_SRC/Oem/SWIGEx/Linux/swig
cd $MG_HOME/scripts
BUILD_COMPONENT="FDO"
sudo -E ./build_fdo.sh 2>&1 | tee $MG_HOME/fdo_build.log
check_build
BUILD_COMPONENT="MapGuide"
sudo -E ./build.sh 2>&1 | tee $MG_HOME/mapguide_build.log
check_build
FDO_REV=`svn info $FDO_SRC | perl revnum.pl`
MG_REV=`svn info $MG_SRC | perl revnum.pl`
BUILD_COMPONENT="MapGuide deb packages"
sudo -E ./dpkgmapguide.sh $MG_ARCH $MG_REV 2>&1 | tee $MG_HOME/dpkg_mapguide.log
check_build
> $MG_HOME/unit_test_status.log
if [ $FDO_UNIT_TEST -eq 1 ];
then
    echo [provision]: Unit test FDO
    BUILD_COMPONENT="Unit Test FDO Core"
    cd $SCRIPT_ROOT/fdo_build_area/Fdo/UnitTest
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_core_unit_test.log
    check_test_generic
    BUILD_COMPONENT="Unit Test SHP Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SHP/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_shp_unit_test.log
    check_test_generic
    BUILD_COMPONENT="Unit Test SDF Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SDF/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_sdf_unit_test.log
    check_test_generic
    BUILD_COMPONENT="Unit Test SQLite Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SQLite/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_sqlite_unit_test.log
    check_test_generic
    BUILD_COMPONENT="Unit Test GDAL Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/GDAL/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_gdal_unit_test.log
    check_test_generic
    # Depending on what version we're building, OGR unit tests may not exist
    if [ -d "$SCRIPT_ROOT/fdo_build_area/Providers/OGR/Src/UnitTest" ];
    then
        BUILD_COMPONENT="Unit Test OGR Provider"
        cd $SCRIPT_ROOT/fdo_build_area/Providers/OGR/Src/UnitTest
        sudo rm providers.xml
        sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_ogr_unit_test.log
        check_test_generic
    fi
    # BUILD_COMPONENT="Unit Test WMS Provider"
    # cd $SCRIPT_ROOT/fdo_build_area/Providers/WMS/Src/UnitTest
    # sudo rm providers.xml
    # sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee $MG_HOME/fdo_wms_unit_test.log
    # check_test_generic
fi
if [ $MG_UNIT_TEST -eq 1 ];
then
    echo [provision]: Unit test MapGuide
    BUILD_COMPONENT="Unit Test MapGuide Server"
    # HACK: Due to unforeseen threading issues or gremlins, none of the server unit tests here
    # are guaranteed to complete and have a possibility of hanging

    # build the individual targets that would comprise "make check" sans execution of unittest.sh
    cd $SCRIPT_ROOT/mgdev/Server/src/UnitTesting
    sudo make libMgUnitTesting.la
    cd $SCRIPT_ROOT/mgdev/Server/src/Core
    sudo make unittest.sh
    # As CoordinateSystem will run longer than the others, run this separately with its own timeout
    for comp in CoordinateSystem
    do
        BUILD_COMPONENT="Unit Test MapGuide Server ($comp)"
        sudo -E timeout 40m ./mgserver test $comp UnitTestResults_${comp}.xml 2>&1 | tee $MG_HOME/mapguide_${comp}_unit_test.log
        check_server_test
        if [ -f UnitTestResults_${comp}.xml ]; then
            sudo mv UnitTestResults_${comp}.xml $MG_HOME/UnitTestResults_${comp}.xml
        fi
    done
    # Run the rest
    for comp in CoordinateSystem FeatureService Geometry KmlService LogManager MappingService TileService ResourceService MdfModel Misc Performance RenderingService ServerAdminService ServerManager ServiceManager SiteManager SiteService ProfilingService TransformMesh
    do
        BUILD_COMPONENT="Unit Test MapGuide Server ($comp)"
        sudo -E timeout 5m ./mgserver test $comp UnitTestResults_${comp}.xml 2>&1 | tee $MG_HOME/mapguide_${comp}_unit_test.log
        check_server_test
        if [ -f UnitTestResults_${comp}.xml ]; then
            sudo mv UnitTestResults_${comp}.xml $MG_HOME/UnitTestResults_${comp}.xml
        fi
    done
    cd $SCRIPT_ROOT/mgdev
    BUILD_COMPONENT="Unit Test MapGuide Web Tier ($comp)"
    sudo -E ./run_tests.sh --prefix ${MG_INST_PATH} --with-php --with-java --logpath $MG_HOME
    check_test_generic
fi
cd $SCRIPT_ROOT
echo [provision]: Make output dir
sudo mkdir -p $MOUNT_DIR/build
echo [provision]: Copy log files to output dir
sudo mv -f $MG_HOME/*.log $MOUNT_DIR/build
echo [provision]: Copy tarballs to output dir
# sudo cp bin/*.tar.xz $MOUNT_DIR/build
sudo cp fdosdk-${FDO_DISTRO}-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz $MOUNT_DIR/build
if [ $MAKE_FDO_SDK -eq 1 ]; then
    echo [provision]: Copy FDO SDK for Ubuntu builds
    # Copy SDK also to sources, so Ubuntu can pick it up
    sudo rm -f $SOURCES_DIR/fdosdk*.tar.xz
    sudo cp fdosdk-${FDO_DISTRO}-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz $SOURCES_DIR
    # Record file name so Ubuntu knows what tarball to extract from
    sudo echo fdosdk-${FDO_DISTRO}-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz > $SOURCES_DIR/fdosdk_filename
    sudo echo ${FDO_REV} > $SOURCES_DIR/fdosdk_rev
    # Export filelist to Ubuntu can deb package this SDK if needed
    sudo cp -R $SCRIPT_ROOT/fdo_build_area/install/filelist $SOURCES_DIR
fi
echo [provision]: Copy FDO deb packages
sudo cp fdo_build_area/install/bin/*.deb $MOUNT_DIR/build
echo [provision]: Copy MapGuide deb packages
sudo cp bin/*.deb $MOUNT_DIR/build
echo [provision]: Copy unit test logs to output dir
if [ -f "$MG_HOME/unit_test_status.log" ]; then
    sudo mv $MG_HOME/unit_test_status.log $MOUNT_DIR/build/unit_test_status.log
fi
if ls $MG_HOME/fdo_*_unit_test.log 1> /dev/null 2>&1; then
    sudo mv $MG_HOME/fdo_*_unit_test.log $MOUNT_DIR/build
else
    echo [provision]: No FDO unit test result files found. If FDO_UNIT_TEST is 0 [value is ${FDO_UNIT_TEST}], this can be safely ignored
fi
if ls $MG_HOME/UnitTestResults_*.xml 1> /dev/null 2>&1; then
    sudo mv $MG_HOME/UnitTestResults_*.xml $MOUNT_DIR/build
else
    echo [provision]: No MapGuide unit test result files found. If MG_UNIT_TEST is 0 [value is ${MG_UNIT_TEST}], this can be safely ignored
fi
echo [provision]: Copy install/uninstall scripts to output dir
DISTNAME=${MG_DISTRO}_x86
if [ "${MG_ARCH}" = "amd64" ]; then
    DISTNAME=${MG_DISTRO}_x64
fi

echo [provision]: Stamping install script
sudo cp $MOUNT_DIR/mginstallubuntu.sh $MOUNT_DIR/build/mginstallubuntu.sh
sudo cp $MOUNT_DIR/mguninstallubuntu.sh $MOUNT_DIR/build/mguninstallubuntu.sh
sudo sed -i 's/URL_PART="ubuntu12"/URL_PART='"$DISTNAME"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGRELEASELABEL="ReleaseLabel"/MGRELEASELABEL='"$MG_RELEASE_LABEL"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/FDOARCH=i386/FDOARCH='"$MG_ARCH"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGARCH=i386/MGARCH='"$MG_ARCH"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/FDOBUILD=0/FDOBUILD='"$FDO_REV"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGBUILD=0/MGBUILD='"$MG_REV"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/FDOVER_MAJOR=0/FDOVER_MAJOR='"$FDO_VER_MAJOR"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/FDOVER_MINOR=0/FDOVER_MINOR='"$FDO_VER_MINOR"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/FDOVER_POINT=0/FDOVER_POINT='"$FDO_VER_REV"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGVER_MAJOR=0/MGVER_MAJOR='"$MG_VER_MAJOR"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGVER_MINOR=0/MGVER_MINOR='"$MG_VER_MINOR"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
sudo sed -i 's/MGVER_POINT=0/MGVER_POINT='"$MG_VER_REV"'/g' $MOUNT_DIR/build/mginstallubuntu.sh
PROVISION_END_TIME=`date +%s`
echo [provision]: Build complete
echo [provision]: Overall build duration: `expr $PROVISION_END_TIME - $PROVISION_START_TIME` s