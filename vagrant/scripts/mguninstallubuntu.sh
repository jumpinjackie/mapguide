#!/bin/bash

# Must have root
if [[ $EUID -ne 0 ]]; then
    echo "You must run this script with superuser privileges"
    exit 1
fi

do_uninstall()
{
    # Stop and un-register mapguide service
    echo "[uninstall]: Removing mapguide service"
    if [ -f /etc/init.d/mapguide ]; then
        /etc/init.d/mapguide stop
        update-rc.d -f mapguide remove
        rm -f /etc/init.d/mapguide
    fi
    echo "[uninstall]: Removing apache-mapguide service"
    if [ -f /etc/init.d/apache-mapguide ]; then
        /etc/init.d/apache-mapguide stop
        update-rc.d -f apache-mapguide remove
        rm -f /etc/init.d/apache-mapguide
    fi

    # Remove Ubuntu packages for MapGuide
    echo "[uninstall]: MapGuide packages"
    for file in server webextensions httpd coordsys coordsys-lite common platformbase
    do
        dpkg --remove mapguideopensource-${file}
    done

    # Remove Ubuntu packages for FDO
    echo "[uninstall]: FDO packages"
    for file in gdal kingoracle ogr rdbms sdf shp sqlite wfs wms arcsde core
    do
        dpkg --remove fdo-${file}
    done
    
    echo "Uninstall completed"
}

while true; do
    read -p 'This script will uninstall the MapGuide and FDO packages and un-register any installed services. Do you wish to uninstall MapGuide? [y/n]' yn
    case $yn in
        [Yy]* ) do_uninstall; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done