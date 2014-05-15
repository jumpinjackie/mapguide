#!/bin/bash

PLATFORMINI=/usr/local/mapguideopensource-2.6.0/desktop/conf/Platform.ini
echo "Running post-install operations"
if [ ! -f /usr/local/mapguideopensource-2.6.0/desktop/lib/libMgDesktop.so ]; then
    ln -s /usr/local/mapguideopensource-2.6.0/desktop/lib/libMgDesktop-2.6.0.so /usr/local/mapguideopensource-2.6.0/desktop/lib/libMgDesktop.so
fi
echo "Updating $PLATFORMINI"
dos2unix $PLATFORMINI
sed -i 's/= Schema\//= \/usr\/local\/mapguideopensource\-2.6.0\/desktop\/Schema/g' $PLATFORMINI
sed -i 's/= Resources\//= \/usr\/local\/mapguideopensource\-2.6.0\/desktop\/Resources/g' $PLATFORMINI
sed -i 's/= FDO\//= \/usr\/local\/fdo\-3.9.0\/lib/g' $PLATFORMINI
sed -i 's/= Dictionaries\//= \/usr\/local\/mapguideopensource\-2.6.0\/share\/gis\/coordsys/g' $PLATFORMINI
echo "Updated $PLATFORMINI"
