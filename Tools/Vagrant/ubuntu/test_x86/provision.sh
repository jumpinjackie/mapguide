#!/bin/bash
DOWNLOAD_HOST=$1
echo Download host is: $DOWNLOAD_HOST
wget "http://$DOWNLOAD_HOST/builds/Sheboygan.mgp"
wget "http://$DOWNLOAD_HOST/builds/smoke_test.sh"
sudo mv Sheboygan.mgp /tmp/
wget "http://$DOWNLOAD_HOST/builds/ubuntu_x86/mginstallubuntu.sh"
sed -i 's/URL="http:\/\/download/URL_OLD="http:\/\/download/g' mginstallubuntu.sh
sed -i 's/#URL="http:\/\/192.168.0.5\/downloads\/ubuntu12"/URL="http:\/\/'"$DOWNLOAD_HOST"'\/builds\/ubuntu_x86"/g' mginstallubuntu.sh
chmod +x smoke_test.sh
chmod +x mginstallubuntu.sh
sudo ./mginstallubuntu.sh --headless --with-sdf --with-shp --with-ogr --with-gdal --with-sqlite
echo Wait 10s before running smoke test
sleep 10s
sudo ./smoke_test.sh