#!/bin/bash

# provision_centos.sh
#
# Entry point provisioning shell script
#
# Expected environment (set up by vagrant)
#
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
UBUNTU=0

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

BUILD_COMPONENT=
check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[error]: ${BUILD_COMPONENT} - Error build failed ($error)"
        exit $error
    fi
}
check_test()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)"
        echo "[warning]: ${BUILD_COMPONENT} - Unit Tests failed ($error)" >> ~/unit_test_status.log
    fi
}
if hash scons 2>/dev/null; then
    echo [provision]: We have scons
else
    echo [provision]: We do not have scons. Installing it
    sudo rpm -Uvh /mapguide_sources/scons-2.3.0-1.noarch.rpm
    check_build
fi
cd ~
HAVE_FDO=0
HAVE_FDO_LIBS=0
HAVE_MAPGUIDE=0
MG_UNIT_TEST=0
FDO_UNIT_TEST=0
MAKE_FDO_SDK=0
SCRIPT_ROOT=~/scripts
FDO_SRC_ROOT=~/fdo/branches
FDO_SRC_DIR=${FDO_BRANCH}
FDO_SRC=$FDO_SRC_ROOT/$FDO_SRC_DIR
if [ "${FDO_BRANCH}" = "trunk" ]; then
    FDO_SRC_ROOT=~/fdo
    FDO_SRC=$FDO_SRC_ROOT/$FDO_SRC_DIR
fi
FDO_LIB_SRC=~/fdo_rdbms_thirdparty
MG_SRC_ROOT=~/mapguide/branches/${MG_BRANCH}
if [ "${MG_BRANCH}" = "trunk" ]; then
    MG_SRC_ROOT=~/mapguide/trunk
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
echo "Build target:     centos - ${MG_ARCH}"
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
    sudo cp /mapguide_sources/atomic.h /usr/include/asm
fi
echo [provision]: Clean line endings
sudo dos2unix /vagrant/*.sh
sudo dos2unix /vagrant/*.pl
sudo dos2unix /vagrant/*.txt
echo [provision]: Copying shell scripts
mkdir -p $SCRIPT_ROOT
cp -f /vagrant/*.sh $SCRIPT_ROOT
cp -f /vagrant/*.pl $SCRIPT_ROOT
cp -f /vagrant/*.txt $SCRIPT_ROOT
cd $SCRIPT_ROOT
if [ $HAVE_FDO_LIBS -eq 0 ];
then
    echo [provision]: Extracting FDO thirdparty libs
    tar -zxf /mapguide_sources/fdo_rdbms_thirdparty.tar.gz -C ~
fi
if [ $HAVE_FDO -eq 0 ];
then
    echo [provision]: Extracting FDO working copy
    mkdir -p $FDO_SRC_ROOT
    tar -zxf /mapguide_sources/$FDO_TARBALL -C $FDO_SRC_ROOT
fi
if [ $HAVE_MAPGUIDE -eq 0 ];
then
    echo [provision]: Extracting MapGuide working copy
    mkdir -p $MG_SRC_ROOT
    tar -zxf /mapguide_sources/$MG_TARBALL -C $MG_SRC_ROOT
fi
if [ -d /mapguide_sources/patches/fdo ];
then
    echo [provision]: Applying FDO source patches
    cp -R /mapguide_sources/patches/fdo/* $FDO_SRC
fi
if [ -d /mapguide_sources/patches/mapguide ];
then
    echo [provision]: Applying MapGuide source patches
    cp -R /mapguide_sources/patches/mapguide/* $MG_SRC
fi
echo [provision]: Making sure swig is executable
chmod +x $MG_SRC/Oem/SWIGEx/Linux/swig
cd ~/scripts
BUILD_COMPONENT="FDO"
sudo -E ./build_fdo.sh 2>&1 | tee ~/fdo_build.log
check_build
BUILD_COMPONENT="MapGuide"
sudo -E ./build.sh 2>&1 | tee ~/mapguide_build.log
check_build
FDO_REV=`svn info $FDO_SRC | perl revnum.pl`
MG_REV=`svn info $MG_SRC | perl revnum.pl`
> ~/unit_test_status.log
if [ $FDO_UNIT_TEST -eq 1 ];
then
    echo [provision]: Unit test FDO
    BUILD_COMPONENT="Unit Test FDO Core"
    cd $SCRIPT_ROOT/fdo_build_area/Fdo/UnitTest
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_core_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test SHP Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SHP/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_shp_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test SDF Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SDF/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_sdf_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test SQLite Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/SQLite/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_sqlite_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test GDAL Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/GDAL/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_gdal_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test OGR Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/OGR/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_ogr_unit_test.log
    check_test
    BUILD_COMPONENT="Unit Test WMS Provider"
    cd $SCRIPT_ROOT/fdo_build_area/Providers/WMS/Src/UnitTest
    sudo rm providers.xml
    sudo -E NLSPATH=${FDO_INST_PATH}/nls/%N ./UnitTest 2>&1 | tee ~/fdo_wms_unit_test.log
    check_test
fi
if [ $MG_UNIT_TEST -eq 1 ];
then
    echo [provision]: Unit test MapGuide
    BUILD_COMPONENT="Unit Test MapGuide Server"
    # HACK: Tile and Resource Service tests are known to freeze our VMs when driven by vagrant (why? who knows? 
    # All we know is that those tests involve threads)
    # So test everything *but* the above services. If required, this can be tested under a manually controlled
    # VM
    #
    # build the individual targets that would comprise "make check" sans execution of unittest.sh
    cd $SCRIPT_ROOT/mgdev/Server/src/UnitTesting
    sudo make libMgUnitTesting.la
    cd $SCRIPT_ROOT/mgdev/Server/src/Core
    sudo make unittest.sh
    # Run the test suites individually except for TileService/ResourceService for reasons already stated
    for comp in CoordinateSystem FeatureService Geometry KmlService LogManager MappingService MdfModel Misc Performance RenderingService ServerAdminService ServerManager ServiceManager SiteManager SiteService ProfilingService TransformMesh
    do
        sudo -E ./mgserver test $comp UnitTestResults_${comp}.xml 2>&1 | tee ~/mapguide_${comp}_unit_test.log
        check_test
        if [ -f UnitTestResults_${comp}.xml ]; then
            sudo mv UnitTestResults_${comp}.xml ~/UnitTestResults_${comp}.xml
        fi
    done
    cd $SCRIPT_ROOT/mgdev
    sudo -E ./run_tests.sh --prefix ${MG_INST_PATH} --with-php --with-java --logpath ~
fi
cd $SCRIPT_ROOT
echo [provision]: Make output dir
sudo mkdir -p /vagrant/build
echo [provision]: Copy log files to output dir
sudo mv -f ~/*.log /vagrant/build
echo [provision]: Copy tarballs to output dir
sudo cp bin/*.tar.xz /vagrant/build
sudo cp fdosdk-centos6-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz /vagrant/build
if [ $MAKE_FDO_SDK -eq 1 ]; then
    echo [provision]: Copy FDO SDK for Ubuntu builds
    # Copy SDK also to sources, so Ubuntu can pick it up
    sudo rm -f /mapguide_sources/fdosdk*.tar.xz
    sudo cp fdosdk-centos6-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz /mapguide_sources
    # Record file name so Ubuntu knows what tarball to extract from
    sudo echo fdosdk-centos6-${MG_ARCH}-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}_${FDO_REV}.tar.xz > /mapguide_sources/fdosdk_filename
    sudo echo ${FDO_REV} > /mapguide_sources/fdosdk_rev
    # Export filelist to Ubuntu can deb package this SDK if needed
    sudo cp -R $SCRIPT_ROOT/fdo_build_area/install/filelist /mapguide_sources
fi
echo [provision]: Copy unit test logs to output dir
sudo mv ~/unit_test_status.log /vagrant/build/unit_test_status.log
sudo mv ~/fdo_*_unit_test.log /vagrant/build
sudo mv ~/UnitTestResults_*.xml /vagrant/build
echo [provision]: Copy install/uninstall scripts to output dir
DISTNAME=centos_x86
if [ "${MG_ARCH}" -eq "amd64" ]; then
    DISTNAME=centos_x64
fi
sudo sed -i 's/URL_PART="centos"/URL_PART='"$DISTNAME"'/g' /vagrant/build/mginstallcentos.sh
sudo cp /vagrant/mginstallcentos.sh /vagrant/build/mginstallcentos.sh
sudo cp /vagrant/mguninstallcentos.sh /vagrant/build/mguninstallcentos.sh
sudo sed -i 's/FDOVER_REV=0/FDOVER_REV='"$FDO_REV"'/g' /vagrant/build/mginstallcentos.sh
sudo sed -i 's/MGVER_REV=0/MGVER_REV='"$MG_REV"'/g' /vagrant/build/mginstallcentos.sh
echo [provision]: Build complete