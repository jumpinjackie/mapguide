#!/bin/bash
DOWNLOAD_HOST=$1
URI_PART=$2
DIST_DIR=$3
echo ***************************************************
echo DOWNLOAD_HOST: ${DOWNLOAD_HOST}
echo URI_PART:      ${URI_PART}
echo DIST_DIR:      ${DIST_DIR}
echo ***************************************************
sudo yum install -y wget
wget "http://$DOWNLOAD_HOST/$URI_PART/Sheboygan.mgp"
sudo mv Sheboygan.mgp /tmp/
wget "http://$DOWNLOAD_HOST/$URI_PART/$DIST_DIR/mginstallcentos.sh"
REPLACE=URL="http://$DOWNLOAD_HOST/$URI_PART/$DIST_DIR"
sed -i 's#URL="\$URL_ROOT\/\$URL_PART"#'"$REPLACE"'#g' mginstallcentos.sh
cp /vagrant/smoke_test.sh .
chmod +x smoke_test.sh
chmod +x mginstallcentos.sh
sudo -E ./mginstallcentos.sh
echo Wait 10s before running smoke test
sleep 10s
sudo -E ./smoke_test.sh