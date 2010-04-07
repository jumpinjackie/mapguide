#!/bin/bash

# Build directory structure underneath directory of this script
# debian/ - common build directory for all packages
#   control - generated all packages control file with subst params
#   changelog - generated changelog file
#   substvars - subst params created by dpkg-shlibdeps
#   mapguidemaestro/ - packaging directory for common MapGuide components
#     usr/local/mapguidemaestro-2.0.0/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
BUILDROOT=`pwd`
MGBUILD=2.0.0
MGINST=usr/local/mapguidemaestro-${MGBUILD}
ROOT=${BUILDROOT}/debian/mapguidemaestro
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}

# Create output directory structure and ignore errors
#
if [ ! -d tmp ]; then
  mkdir tmp >& /dev/null
  mkdir bin >& /dev/null
  mkdir debian >& /dev/null
else
# clean out existing build tree
  rm -rf tmp/*
  rm -rf bin/*
  rm -rf debian/*
fi


# Read and validate command line parameters
#
ARCH="$1"
BUILDNUM="$2"
if test -z ${ARCH}; then 
  echo Usage: dpkgmaestro.sh [i386/amd64] svnChangeNum
  exit -1
fi
if test -z ${BUILDNUM}; then 
  echo Usage: dpkgmaestro.sh [i386/amd64] svnChangeNum
  exit -1
fi

case $ARCH in 
i386)
  echo Setting architecture to i386.
  ;;
amd64)
  echo Setting architecture to amd64.
  ;;
*)
  echo Usage: dpkgmaestro.sh [i386/amd64] svnChangeNum
  echo Invalid architecture.  Please specify i386 or amd64.
  exit -1
  ;;
esac


# Create control file
#
cat > debian/control <<END-OF-CONTROL
Source: mapguideopensource-maestro-src
Maintainer: MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>
Section: misc
Priority: optional
Homepage: http://mapguide.osgeo.org

Package: mapguideopensource-maestro
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: mono-runtime (>= 2.4), libmono-winforms2.0-cil (>= 2.4)
Description:  OSGeo MapGuide Maestro ${MGBUILD}
END-OF-CONTROL


# Create changelog file
# TODO: pull revision history from Subversion
#
CURRTIME=`date -R`
cat > debian/changelog <<END-OF-CHANGELOG
mapguideopensource-maestro-src (${MGBUILD}-${BUILDNUM}) experimental; urgency=low

  * Beta build.

 -- MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>  ${CURRTIME}
END-OF-CHANGELOG


# TODO: Pull copyright file from Subversion vault 
#
cat > tmp/copyright <<END-OF-COPYRIGHT
Copyright (C) 2009 - 2010, Kenneth Skovhede
http://www.hexad.dk, opensource@hexad.dk

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
END-OF-COPYRIGHT
iconv -f ISO-8859-1 -t UTF-8 tmp/copyright > debian/copyright

PACKAGENAME=mapguideopensource-maestro
PACKAGEDIR=mapguidemaestro
DIRLIST="."
REMOVELIST=""
STRIPLIST=""

source ./dpkgbuild.sh

