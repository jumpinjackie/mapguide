#!/bin/sh

set -e
set -x

date | sudo tee /etc/vagrant_box_build_time

mkdir -p ~/.ssh
curl -fsSLo ~/.ssh/authorized_keys https://raw.githubusercontent.com/mitchellh/vagrant/master/keys/vagrant.pub
chmod 700 ~/.ssh/
chmod 600 ~/.ssh/authorized_keys

if [ -f /etc/sudoers ] ; then
    sudo sed -i "s/^.*requiretty/#Defaults requiretty/" /etc/sudoers
    sudo sed -i "s/^\(.*env_keep = \"\)/\1PATH /" /etc/sudoers
    sudo sed -i -e 's/%admin ALL=(ALL) ALL/%admin ALL=NOPASSWD:ALL/g' /etc/sudoers
    sudo sed -i -e 's/%sudo.*ALL=.*ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers
fi