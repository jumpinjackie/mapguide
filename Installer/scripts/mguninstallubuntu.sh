#!/bin/bash

# Remove Ubuntu packages for MapGuide
for file in server webextensions httpd common
do
  dpkg --remove mapguideopensource-${file}
done

# Remove Ubuntu packages for FDO
for file in gdal kingoracle ogr postgis rdbms sdf shp sqlite wfs wms core
do
  dpkg --remove fdo-${file}
done


