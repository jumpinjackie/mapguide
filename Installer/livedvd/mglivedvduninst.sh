#!/bin/bash

#
#  Copyright (C) 2010 by Autodesk, Inc.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of version 2.1 of the GNU Lesser
#  General Public License as published by the Free Software Foundation.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

# Remove Ubuntu packages for MapGuide
for file in server webextensions httpd common
do
  dpkg --remove mapguideopensource-${file}
done

# Remove Ubuntu packages for FDO
for file in wms wfs sqlite shp sdf postgis ogr kingoracle gdal core
do
  dpkg --remove fdo-${file}
done

dpkg --remove mapguideopensource-maestro

