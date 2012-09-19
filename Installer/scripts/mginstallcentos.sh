#!/bin/bash
URL="http://download.osgeo.org/mapguide/releases/2.4.0/RC2/"
FDOVER_MAJOR_MINOR=3.7
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.0
FDOVER=${FDOVER_MAJOR_MINOR_REV}.6577
MGVER_MAJOR_MINOR=2.4
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.0
MGVER=${MGVER_MAJOR_MINOR_REV}.6989
FDO_TARBALL=fdosdk-centos5-${FDOVER}.tgz
MG_TARBALL=mapguideopensource-${MGVER}.tar.gz

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "You must run this script with superuser privileges"
	exit 1
fi

if [ ! -f ${FDO_TARBALL} ]; then
wget -N ${URL}${FDO_TARBALL}
fi

tar -C / -zxvf ${FDO_TARBALL}

if [ ! -f ${MG_TARBALL} ]; then
wget -N ${URL}${MG_TARBALL}
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
