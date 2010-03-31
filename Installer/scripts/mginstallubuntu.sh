#!/bin/bash
TEMPDIR=/tmp/build_mapguide
URL="http://download.osgeo.org/mapguide/testing"
FDOVER=3.5.0-5343_i386
# Create temporary download directory
mkdir -p ${TEMPDIR}
pushd ${TEMPDIR}

# Download Ubuntu packages
for file in core gdal kingoracle ogr postgis rdbms sdf shp sqlite wfs wms
do
  wget -N ${URL}/fdo-${file}_${FDOVER}.deb
done

# Install Ubuntu packages
for file in core gdal kingoracle ogr postgis rdbms sdf shp sqlite wfs wms
do
  dpkg -E -G --install fdo-${file}_${FDOVER}.deb
done


pushd 
