#!/bin/bash
# Set bash as the default shell
echo "dash    dash/sh boolean false" | sudo debconf-set-selections
sudo dpkg-reconfigure --frontend=noninteractive dash
sudo apt-get install -y build-essential bison automake libtool libpng-dev libjpeg62-dev libxslt-dev libfreetype6-dev libpcre3-dev openjdk-7-jdk subversion lintian ant scons dos2unix libldap-dev libssl-dev libcurl4-openssl-dev libexpat-dev libmysqlclient-dev unixODBC-dev python-dev libpq-dev libcppunit-dev
# For producing .tar.xz archives
sudo apt-get install -y xz-utils
# For deb packaging
sudo apt-get install -y rsync
# For 32-bit swig binary in MapGuide source
sudo apt-get install -y g++-multilib
# To support Java test suite
sudo apt-get install -y ant-contrib
echo "JAVA_HOME=/usr/lib/jvm/java-1.7.0-openjdk-amd64" | sudo tee --append /etc/environment