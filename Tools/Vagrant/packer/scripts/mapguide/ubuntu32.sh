#!/bin/bash
# Set bash as the default shell
echo "dash    dash/sh boolean false" | sudo debconf-set-selections
sudo dpkg-reconfigure --frontend=noninteractive dash
# Guard against (http://askubuntu.com/questions/41605/trouble-downloading-updates-due-to-a-hash-sum-mismatch-error)
sudo rm -rf /var/lib/apt/lists/*
sudo apt-get update
sudo apt-get install -y build-essential bison automake libtool libpng-dev libjpeg62-dev libxslt-dev libfreetype6-dev libpcre3-dev openjdk-7-jdk subversion lintian ant scons dos2unix liboss4-salsa-dev libldap-dev libssl-dev libcurl4-openssl-dev libexpat-dev libmysqlclient-dev unixODBC-dev python-dev libpq-dev libcppunit-dev
# For producing .tar.xz archives
sudo apt-get install -y xz-utils
# For deb packaging
sudo apt-get install -y rsync
echo "JAVA_HOME=/usr/lib/jvm/java-1.7.0-openjdk-i386" | sudo tee --append /etc/environment