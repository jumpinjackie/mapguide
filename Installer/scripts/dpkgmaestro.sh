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
#
# Updated 31 Jan 2012 for use with Maestro trunk (r6468)
#
BUILDROOT=`pwd`
MGBUILD=4.1.0
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
# NOTE: Mono (2.8?) has split these packages into a bajillion little pieces
# just to make things hard for us. Anyway, I think we've got all the dependencies
# covered
cat > debian/control <<END-OF-CONTROL
Source: mapguide-maestro-src
Maintainer: MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>
Section: misc
Priority: optional
Homepage: http://mapguide.osgeo.org

Package: mapguide-maestro
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: mono-runtime (>= 2.8), libmono-system-windows-forms4.0-cil (>= 2.8), libmono-system-core4.0-cil (>= 2.8), libmono-system-numerics4.0-cil (>= 2.8), libmono-system-web4.0-cil (>= 2.8)
Description: MapGuide Maestro ${MGBUILD} is an open source map authoring tool for MapGuide Open Source and Autodesk MapGuide Enterprise / Infrastructure Map Server
END-OF-CONTROL


# Create changelog file
# TODO: pull revision history from Subversion
#
CURRTIME=`date -R`
cat > debian/changelog <<END-OF-CHANGELOG
mapguide-maestro-src (${MGBUILD}-${BUILDNUM}) experimental; urgency=low

  * Beta build.

 -- MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>  ${CURRTIME}
END-OF-CHANGELOG


# TODO: Pull copyright file from Subversion vault 
#
cat > tmp/copyright <<END-OF-COPYRIGHT
Copyright (C) 2009-2012, Jackie Ng
http://trac.osgeo.org/mapguide/wiki/maestro, jumpinjackie@gmail.com

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

PACKAGENAME=mapguide-maestro
PACKAGEDIR=mapguidemaestro
DIRLIST="."
REMOVELIST=""
STRIPLIST=""

source ./dpkgbuild.sh

