#!/bin/bash
URL="http://download.osgeo.org/mapguide/releases/2.5.2"
#URL="http://192.168.0.10/downloads"
FDOVER_MAJOR_MINOR=3.8
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.0
FDOVER=${FDOVER_MAJOR_MINOR_REV}_6957
MGVER_MAJOR_MINOR=2.5
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.2
MGVER=${MGVER_MAJOR_MINOR_REV}.7949
FDO_TARBALL=fdosdk-centos6-${FDOVER}.tar.gz
MG_TARBALL=mapguideopensource-${MGVER}.tar.gz
MG_URL=${URL}/${MG_TARBALL}
#FDO_URL="http://download.osgeo.org/fdo/${FDOVER_MAJOR_MINOR_REV}/release/binaries/linux32/${FDO_TARBALL}"
FDO_URL=${URL}/${FDO_TARBALL}

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "You must run this script with superuser privileges"
	exit 1
fi

if [ ! -f ${FDO_TARBALL} ]; then
wget -N ${FDO_URL}
fi

# Our FDO tarball requires extraction to root
tar -C / -zxvf ${FDO_TARBALL}
mkdir -p /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}
# The FDO tarball requires extraction to the expected dir
#tar -C /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/ -zxvf ${FDO_TARBALL}

if [ ! -f ${MG_TARBALL} ]; then
wget -N ${MG_URL}
fi

tar -C / -zxvf ${MG_TARBALL}

# All of this is to make SELinux happy
pushd /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/lib
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin
chcon -t textrel_shlib_t *.so
chcon -t textrel_shlib_t mgserver
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/lib
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/lib
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/lib
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/lib
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/modules
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/bin
./apachectl start
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin
./mgserverd.sh
popd

echo DONE
