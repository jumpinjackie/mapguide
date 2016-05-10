#!/bin/bash
DOWNLOAD_HOST=$1
URI_PART=$2
DIST_DIR="centos_x86"
sudo yum install -y wget
echo Download host is: $DOWNLOAD_HOST
wget "http://$DOWNLOAD_HOST/$URI_PART/Sheboygan.mgp"
wget "http://$DOWNLOAD_HOST/$URI_PART/smoke_test.sh"
sudo mv Sheboygan.mgp /tmp/
wget "http://$DOWNLOAD_HOST/$URI_PART/$DIST_DIR/mginstallcentos.sh"
sed -i 's/URL="$URL_ROOT\/$URL_PART"/URL="http:\/\/'"$DOWNLOAD_HOST"'\/'"$URI_PART"'\/'"$DIST_DIR"'"/g' mginstallcentos.sh
chmod +x smoke_test.sh
chmod +x mginstallcentos.sh
sudo -E ./mginstallcentos.sh
echo Wait 10s before running smoke test
sleep 10s
sudo -E ./smoke_test.sh