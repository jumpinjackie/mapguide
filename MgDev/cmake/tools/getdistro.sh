#!/bin/sh

# getdistro.sh
#
# Uses lsb_release to get a suitable distro string for CPack
OS=$(lsb_release -si)
VER=$(lsb_release -sr | cut -d. -f1)
echo -n "${OS}${VER}"