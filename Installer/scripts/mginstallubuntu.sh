#!/bin/bash
TEMPDIR=/tmp/build_mapguide
URL="http://download.osgeo.org/mapguide/testing"
FDOVER=3.5.0-5343_i386
MGVER=2.2.0-4724_i386
# Create temporary download directory
mkdir -p ${TEMPDIR}
pushd ${TEMPDIR}

# Download Ubuntu packages for FDO
for file in core gdal kingoracle ogr postgis rdbms sdf shp sqlite wfs wms
do
  wget -N ${URL}/fdo-${file}_${FDOVER}.deb
done

# Download Ubuntu packages for MapGuide
for file in common server webextensions httpd
do
  wget -N ${URL}/mapguideopensource-${file}_${MGVER}.deb
done

# Install Ubuntu packages for FDO
for file in core gdal kingoracle ogr postgis rdbms sdf shp sqlite wfs wms
do
  dpkg -E -G --install fdo-${file}_${FDOVER}.deb
done

# Install Ubuntu packages for MapGuide
for file in common server webextensions httpd
do
  dpkg -E -G --install mapguideopensource-${file}_${MGVER}.deb
done

popd

# Install required packages missed due CentOS 5.4 / Ubuntu 9.10 differences
apt-get install expat libssl0.9.8

# Resolve CentOS 5.4 / Ubuntu 9.10 shared lib differences with symlinks
if [ ! -e /lib/libcrypto.so.6 ]; then
  ln -s /lib/libcrypto.so.0.9.8 /lib/libcrypto.so.6
fi

if [ ! -e /lib/libssl.so.6 ]; then
  ln -s /lib/libssl.so.0.9.8 /lib/libssl.so.6
fi

if [ ! -e /lib/libexpat.so.0 ]; then
  ln -s /lib/libexpat.so.1 /lib/libexpat.so.0
fi

