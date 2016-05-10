#!/bin/bash

set -e
set -x

# I suspect that apt-get out of the box is fubar'd on 12.04.5 due to 
# (http://askubuntu.com/questions/41605/trouble-downloading-updates-due-to-a-hash-sum-mismatch-error)
# We already had a workaround for this in one of the down-level scripts, but it seems we need to do this right here, right now
sudo rm -rf /var/lib/apt/lists/*
sudo apt-get update

sudo aptitude -y install dkms
sudo aptitude -y install make

# Uncomment this if you want to install Guest Additions with support for X
#sudo aptitude -y install xserver-xorg

sudo mount -o loop ~/VBoxGuestAdditions.iso /mnt/
sudo /mnt/VBoxLinuxAdditions.run || :
sudo umount /mnt/
rm -f ~/VBoxGuestAdditions.iso

VBOX_VERSION=$(cat ~/.vbox_version)
if [ "$VBOX_VERSION" == '4.3.10' ]; then
  # https://www.virtualbox.org/ticket/12879
  sudo ln -s "/opt/VBoxGuestAdditions-$VBOX_VERSION/lib/VBoxGuestAdditions" /usr/lib/VBoxGuestAdditions
fi
