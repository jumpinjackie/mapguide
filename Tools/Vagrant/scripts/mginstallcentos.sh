#!/bin/bash
URL="http://download.osgeo.org/mapguide/releases/3.0.0/Release"
#URL="http://192.168.0.4/downloads"
FDOVER_MAJOR_MINOR=3.9
FDOVER_POINT=0
FDOVER_REV=0
FDO_ARCH=i386
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.${FDOVER_POINT}
FDOVER=${FDOVER_MAJOR_MINOR_REV}_${FDOVER_REV}
MGVER_MAJOR_MINOR=3.0
MGVER_POINT=0
MGVER_REV=0
MG_ARCH=i386
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.${MGVER_POINT}
MGVER=${MGVER_MAJOR_MINOR_REV}.${MGVER_REV}
FDO_TARBALL=fdosdk-centos6-${FDO_ARCH}-${FDOVER}.tar.xz
MG_TARBALL=mapguideopensource-${MGVER}.${MG_ARCH}.tar.xz
MG_URL=${URL}/${MG_TARBALL}
FDO_URL=${URL}/${FDO_TARBALL}

# Must make sure we have .tar.xz support and any other deps if this is a minimal install
yum install -y xz-lzma-compat wget libxslt libpng redhat-lsb-core 

# Must have root
if [[ $EUID -ne 0 ]]; then
    echo "You must run this script with superuser privileges"
    exit 1
fi

wget -N -c ${FDO_URL}

mkdir -p /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}
echo "[install]: Extracting FDO"
tar -C /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/ -Jxf ${FDO_TARBALL}

wget -N -c ${MG_URL}

echo "[install]: Extracting MapGuide"
tar -C / -Jxf ${MG_TARBALL}

# All of this is to make SELinux happy
pushd /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/lib
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
chcon -t textrel_shlib_t mgserver
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/lib
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/lib
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/lib
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/lib
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/modules
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd

echo "[config]: Fixing permissions for certain folders"
chmod 777 /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/TempDir

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/bin
echo "[install]: Starting httpd"
./apachectl start
popd

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin
echo "[install]: Starting mgserver"
./mgserverd.sh
popd

echo DONE
