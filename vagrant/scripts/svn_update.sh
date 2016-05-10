#!/bin/bash

# svn_update.sh
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
#  $1 - FDO branch name (trunk|major.minor)
#  $2 - MG branch name (trunk|major.minor)

echo "*****************************************************"
echo " Arg check"
echo "  1 - ${1}"
echo "  2 - ${2}"
echo "*****************************************************"

FDO_BRANCH=${1}
MG_BRANCH=${2}

BUILD_COMPONENT=
check_build()
{
	error=$?
	if [ $error -ne 0 ]; then
		echo "[error]: ${BUILD_COMPONENT} - Error build failed ($error)"
		exit $error
	fi
}
cd ~
echo [provision]: Home directory is `pwd`
echo [provision]: Checking directories

HAVE_FDO=0
HAVE_MAPGUIDE=0
SCRIPT_ROOT=~/scripts
FDO_SRC_ROOT=~/fdo
FDO_SRC_DIR=${FDO_BRANCH}
FDO_SRC=$FDO_SRC_ROOT/$FDO_SRC_DIR
FDO_LIB_SRC=~/fdo_rdbms_thirdparty
MG_SRC_ROOT=~/mapguide/branches/${MG_BRANCH}
if [ "${MG_BRANCH}" = "trunk" ]; then
    MG_SRC_ROOT=~/mapguide/trunk
fi
MG_SRC=$MG_SRC_ROOT/MgDev
FDO_TARBALL=fdo-checkout-${FDO_BRANCH}.tar.gz
MG_TARBALL=mapguide-checkout-${MG_BRANCH}.tar.gz

if [ -d $FDO_SRC ];
then
    HAVE_FDO=1
fi
if [ -d $MG_SRC ];
then
    HAVE_MAPGUIDE=1
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
echo [provision]: svn update FDO
BUILD_COMPONENT="svn update FDO"
svn update $FDO_SRC
check_build
echo [provision]: svn update MapGuide
BUILD_COMPONENT="svn update MapGuide"
svn update $MG_SRC
check_build
if [ ! -d /mapguide_sources/updated ]; then
    sudo mkdir -p /mapguide_sources/updated
    cd $MG_SRC_ROOT
    echo [provision]: Updating mapguide source tarball
    tar -zcf $MG_TARBALL MgDev
    sudo mv $MG_TARBALL /mapguide_sources/updated
    cd $FDO_SRC_ROOT
    echo [provision]: Updating fdo source tarball
    tar -zcf $FDO_TARBALL $FDO_SRC_DIR
    sudo mv $FDO_TARBALL /mapguide_sources/updated
else
    echo [provision]: Updated tarballs already exist. Doing nothing
fi
echo [provision]: Build complete