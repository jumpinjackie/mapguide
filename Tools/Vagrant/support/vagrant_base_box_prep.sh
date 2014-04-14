#!/usr/bin/env bash
apt-get -y update
apt-get -y install build-essential zlib1g-dev libssl-dev libreadline6-dev libyaml-dev
cd /tmp
wget ftp://ftp.ruby-lang.org/pub/ruby/1.9/ruby-1.9.3-p125.tar.gz
tar -xvzf ruby-1.9.3-p125.tar.gz
cd ruby-1.9.3-p125/
./configure --prefix=/usr/local
make
make install
gem install chef ruby-shadow --no-ri --no-rdoc
apt-get install -y puppet puppetmaster openssh-server
mkdir ~/.ssh/
cd ~/.ssh
wget http://github.com/mitchellh/vagrant/raw/master/keys/vagrant
wget http://github.com/mitchellh/vagrant/raw/master/keys/vagrant.pub
mv vagrant.pub authorized_keys
apt-get install linux-headers-$(uname -r) build-essential
mkdir /media/cdrom
mount /dev/cdrom /media/cdrom
sh /media/cdrom/VBoxLinuxAdditions.run
apt-get clean
groupadd admin
usermod -G admin vagrant