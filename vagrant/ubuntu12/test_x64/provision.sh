#!/bin/bash
DOWNLOAD_HOST=$1
URI_PART=$2
DIST_DIR="ubuntu_x64"
echo Download host is: $DOWNLOAD_HOST
wget "http://$DOWNLOAD_HOST/$URI_PART/Sheboygan.mgp"
wget "http://$DOWNLOAD_HOST/$URI_PART/smoke_test.sh"
sudo mv Sheboygan.mgp /tmp/
wget "http://$DOWNLOAD_HOST/$URI_PART/$DIST_DIR/mginstallubuntu.sh"
sed -i 's/URL="$URL_ROOT\/$URL_PART"/URL="http:\/\/'"$DOWNLOAD_HOST"'\/'"$URI_PART"'\/'"$DIST_DIR"'"/g' mginstallubuntu.sh
chmod +x smoke_test.sh
chmod +x mginstallubuntu.sh
sudo ./mginstallubuntu.sh --headless --with-sdf --with-shp --with-ogr --with-gdal --with-sqlite
echo Wait 10s before running smoke test
sleep 10s
sudo ./smoke_test.sh