#!/bin/bash

# Sets the common set of vars used throughout these build scripts

export APIVERSION=2.4
export BUILDNUM=${APIVERSION}.0
export BUILDROOT=`pwd`
export INSTALLROOT=/usr/local/mapguideopensource-${BUILDNUM}
export LOCKFILEDIR=/var/lock/mgserver
export MGSOURCE=${BUILDROOT}/mgdev
export VERFILE=${MGSOURCE}/Common/ProductVersion.h
# Use LOCALSVN=1 if you want to build from an existing copy instead of building
# from an export
export LOCALSVN=1
export SVNROOT=/home/jackie
export SVNRELPATH=/mg-2.4/MgDev
#export SVNROOT="svn://svn.bld.mgproto.net"
#export SVNROOT="http://svn.osgeo.org"
#export SVNRELPATH=/mapguide/trunk/MgDev
export MY_MAKE_OPTS="-j 4"
export UBUNTU=1
