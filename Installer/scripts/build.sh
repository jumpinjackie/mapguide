#!/bin/bash
BUILDROOT=`pwd`
INSTALLROOT=/usr/local/mapguideopensource
MGSOURCE=${BUILDROOT}/mgdev

rm -rf ${MGSOURCE}
rm -rf ${INSTALLROOT}

svn export svn://svn.bld.mgproto.net/mapguide/trunk/MgDev ${MGSOURCE}
svn export svn://svn.bld.mgproto.net/metacrs/csmap/trunk/CsMapDev ${MGSOURCE}/Oem/CsMap
svn export svn://svn.bld.mgproto.net/fusion/trunk ${MGSOURCE}/Oem/fusion

cd ${MGSOURCE}

pushd ${MGSOURCE}/Oem/LinuxApt
./build_apt.sh
popd

./build_oem.sh

pushd ${MGSOURCE}/Oem/fusion
ant prepare
ant compress
popd

aclocal
libtoolize --force
automake --add-missing --copy
autoconf
./configure --enable-optimized
make
make install
