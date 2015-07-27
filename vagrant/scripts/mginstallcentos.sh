#!/bin/bash
URL_ROOT="http://download.osgeo.org/mapguide/releases/2.6.1/Final"
URL_PART="centos"
URL="$URL_ROOT/$URL_PART"
#URL="http://192.168.0.4/downloads"
FDOVER_MAJOR_MINOR=3.9
FDOVER_POINT=1
FDOVER_REV=0
FDO_ARCH=i386
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.${FDOVER_POINT}
FDOVER=${FDOVER_MAJOR_MINOR_REV}_${FDOVER_REV}
MGVER_MAJOR_MINOR=2.6
MGVER_POINT=1
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
pushd /usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/lib > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
chcon -t textrel_shlib_t mgserver
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/lib > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/lib > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/lib > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/lib > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/modules > /dev/null
echo "[config]: Making binaries SELinux compatible"
chcon -t textrel_shlib_t *.so
popd > /dev/null

echo "[config]: Fixing permissions for certain folders"
chmod 777 /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/TempDir

echo "[install]: Registering services"
# The service control scripts as-is won't register as services as they lack chkconfig information
# We'll inject this information by prepending this information and appending the existing script bodies
# afterwards to the destination. We are not making symlinks.
echo "#!/bin/sh" > /etc/init.d/mapguide
echo "# chkconfig: 345 35 65" >> /etc/init.d/mapguide
echo "# description: MapGuide Server Daemon" >> /etc/init.d/mapguide
sed "s/\#\!\/bin\/sh//g" /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/mapguidectl >> /etc/init.d/mapguide
chmod +x /etc/init.d/mapguide
chkconfig --add mapguide
echo "#!/bin/sh" > /etc/init.d/apache-mapguide
echo "# chkconfig: 345 35 65" >> /etc/init.d/apache-mapguide
echo "# description: MapGuide Server Daemon" >> /etc/init.d/apache-mapguide
sed "s/\#\!\/bin\/sh//g" /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/bin/apachectl >> /etc/init.d/apache-mapguide
chmod +x /etc/init.d/apache-mapguide
chkconfig --add apache-mapguide

echo "[install]: Starting httpd"
/etc/init.d/mapguide start

echo "[install]: Starting mgserver"
/etc/init.d/apache-mapguide start

echo DONE
