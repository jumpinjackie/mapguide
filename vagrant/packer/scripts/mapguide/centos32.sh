#!/bin/bash
sudo yum install -y gcc make gcc-c++ gd-* automake bison byacc flex doxygen expat expat-devel libtool libjpeg-devel libpng libpng-devel libxml2 libxml2-devel openssl curl curl-devel libxslt libxslt-devel subversion java-1.7.0-openjdk java-1.7.0-openjdk-devel ant dos2unix openssh-server openldap-devel alsa-lib-devel pcre-devel unixODBC-devel libcom_err-devel krb5-devel openssl-devel mysql-devel postgresql-devel unixODBC
# For producing .tar.xz archives
sudo yum install -y xz-lzma-compat
# To support Java test suite
sudo yum install -y ant-contrib
echo "JAVA_HOME=/usr/lib/jvm/java-openjdk" | sudo tee --append /etc/environment