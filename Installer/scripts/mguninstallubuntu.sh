#!/bin/bash

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "You must run this script with superuser privileges"
	exit 1
fi

# Remove Ubuntu packages for MapGuide
for file in server webextensions httpd common
do
  dpkg --remove mapguideopensource-${file}
done

# Remove Ubuntu packages for FDO
for file in gdal kingoracle ogr odbc mysql postgis rdbms sdf shp sqlite wfs wms arcsde core
do
  dpkg --remove fdo-${file}
done


