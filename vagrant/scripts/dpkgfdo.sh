#!/bin/bash
#ARCH=`arch`
#REPO=http://svn.osgeo.org/fdo/branches/3.5
#BUILDNUM=`svn info ${REPO} | awk '/Changed Rev/ {print $4}'`
ARCH="$1"
BUILDNUM="$2"
echo Building FDO arch=${ARCH} buildnum=${BUILDNUM}
./dpkgfdocore.sh $ARCH $BUILDNUM
./dpkgfdoprovider.sh $ARCH $BUILDNUM sdf
./dpkgfdoprovider.sh $ARCH $BUILDNUM shp
./dpkgfdoprovider.sh $ARCH $BUILDNUM wfs
./dpkgfdoprovider.sh $ARCH $BUILDNUM wms
# ArcSDE provider currently disabled due to missing libraries
#./dpkgfdoprovider.sh $ARCH $BUILDNUM arcsde
./dpkgfdoprovider.sh $ARCH $BUILDNUM rdbms
./dpkgfdoprovider.sh $ARCH $BUILDNUM gdal
./dpkgfdoprovider.sh $ARCH $BUILDNUM ogr
./dpkgfdoprovider.sh $ARCH $BUILDNUM postgis
./dpkgfdoprovider.sh $ARCH $BUILDNUM kingoracle
./dpkgfdoprovider.sh $ARCH $BUILDNUM sqlite
