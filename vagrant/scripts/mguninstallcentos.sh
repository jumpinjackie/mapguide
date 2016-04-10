#!/bin/bash
FDOVER_MAJOR=0
FDOVER_MINOR=0
FDOVER_MAJOR_MINOR=${FDOVER_MAJOR}.${FDOVER_MINOR}
FDOVER_POINT=0
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.${FDOVER_POINT}
MGVER_MAJOR=0
MGVER_MINOR=0
MGVER_MAJOR_MINOR=${MGVER_MAJOR}.${MGVER_MINOR}
MGVER_POINT=0
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.${MGVER_POINT}

FDOINST=/usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}
MGINST=/usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}

# Must have root
if [[ $EUID -ne 0 ]]; then
    echo "You must run this script with superuser privileges"
    exit 1
fi

do_uninstall()
{
    if [ -f /etc/init.d/mapguide ]; then
        echo "[uninstall]: Remove mapguide service"
        /etc/init.d/mapguide stop
        chkconfig --del mapguide
        rm -f /etc/init.d/mapguide
    fi

    if [ -f /etc/init.d/apache-mapguide ]; then
        echo "[uninstall]: Remove apache-mapguide service"
        /etc/init.d/apache-mapguide stop
        chkconfig --del apache-mapguide
        rm -f /etc/init.d/apache-mapguide
    fi

    if [ -d $MGINST ]; then
        echo "[uninstall]: Uninstalling MapGuide"
        rm -rf $MGINST
    fi

    if [ -d $FDOINST ]; then
        echo "[uninstall]: Uninstalling FDO"
        rm -rf $FDOINST
    fi
    
    echo "Uninstall completed"
}

while true; do
    read -p 'This script will remove everything under the MapGuide and FDO installation directories and un-register any installed services. Do you wish to uninstall MapGuide? [y/n]' yn
    case $yn in
        [Yy]* ) do_uninstall; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
