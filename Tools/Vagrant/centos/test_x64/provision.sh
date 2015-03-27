#!/bin/bash
DOWNLOAD_HOST=$1
sudo yum install -y wget
echo Download host is: $DOWNLOAD_HOST
wget "http://$DOWNLOAD_HOST/builds/Sheboygan.mgp"
wget "http://$DOWNLOAD_HOST/builds/smoke_test.sh"
sudo mv Sheboygan.mgp /tmp/
wget "http://$DOWNLOAD_HOST/builds/centos_x64/mginstallcentos.sh"
sed -i 's/URL="http:\/\/download/URL_OLD="http:\/\/download/g' mginstallcentos.sh
sed -i 's/#URL="http:\/\/192.168.0.4\/downloads"/URL="http:\/\/'"$DOWNLOAD_HOST"'\/builds\/centos_x64"/g' mginstallcentos.sh
chmod +x smoke_test.sh
chmod +x mginstallcentos.sh
sudo -E ./mginstallcentos.sh
sudo -E ./smoke_test.sh