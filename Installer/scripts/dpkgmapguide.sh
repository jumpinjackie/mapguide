#!/bin/bash

# Build directory structure underneath directory of this script
# debian/ - common build directory for all packages
#   control - generated all packages control file with subst params
#   changelog - generated changelog file
#   substvars - subst params created by dpkg-shlibdeps
#   mapguidecommon/ - packaging directory for common MapGuide components
#     usr/local/mapguideopensource-2.2.0/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguideserver/ - packaging directory for MapGuide Server
#     usr/local/mapguideopensource-2.2.0/ - copied tree for Server
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidewebextensions/ - packaging directory for Web Extensions
#     usr/local/mapguideopensource-2.2.0/ - copied tree for Web Extensions
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidehttpd/ - packaging directory for Apache Bundle
#     usr/local/mapguideopensource-2.2.0/ - copied tree for Apache bundle
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
# 
BUILDROOT=`pwd`
MGBUILD=2.2.0
MGINST=usr/local/mapguideopensource-${MGBUILD}
ROOT=${BUILDROOT}/debian/mapguidecommon
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
   echo Fix me later
#  rm -rf tmp/*
#  rm -rf bin/*
#  rm -rf debian/*
fi


# Read and validate command line parameters
#
ARCH="$1"
BUILDNUM="$2"
if test -z ${ARCH}; then 
  echo Usage: dpkgmapguide.sh [i386/amd64] svnChangeNum
  exit -1
fi
if test -z ${BUILDNUM}; then 
  echo Usage: dpkgmapguide.sh [i386/amd64] svnChangeNum
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
  echo Usage: dpkgmapguide.sh [i386/amd64] svnChangeNum
  echo Invalid architecture.  Please specify i386 or amd64.
  exit -1
  ;;
esac


# Create control file
#
cat > debian/control <<END-OF-CONTROL
Source: mapguideopensource-src
Maintainer: MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>
Section: misc
Priority: optional
Homepage: http://mapguide.osgeo.org

Package: mapguideopensource-common
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguidecommon:Depends}
Description:  OSGeo MapGuide ${MGBUILD} common components

Package: mapguideopensource-server
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguideserver:Depends}
Description:  OSGeo MapGuide ${MGBUILD} Server

Package: mapguideopensource-webextensions
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguidewebextensions:Depends}
Description:  OSGeo MapGuide ${MGBUILD} Web Extensions

Package: mapguideopensource-httpd
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguidehttpd:Depends}
Description:  OSGeo MapGuide ${MGBUILD} Apache Web Server
END-OF-CONTROL


# Create changelog file
# TODO: pull revision history from Subversion
#
CURRTIME=`date -R`
cat > debian/changelog <<END-OF-CHANGELOG
mapguideopensource-src (${MGBUILD}-${BUILDNUM}) experimental; urgency=low

  * Pre-release build.

 -- MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>  ${CURRTIME}
END-OF-CHANGELOG


# Pull copyright file from Subversion vault 
# TODO: Create a copyright file for MapGuide, use FDO for now
wget -N http://svn.osgeo.org/fdo/branches/3.5/License_README.txt -O tmp/copyright
iconv -f ISO-8859-1 -t UTF-8 tmp/copyright > debian/copyright

PACKAGENAME=mapguideopensource-common
PACKAGEDIR=mapguidecommon
DIRLIST="lib share"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$ libdwf"

# source ./dpkgbuild.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}/server
ROOT=${BUILDROOT}/debian/mapguideserver
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
PACKAGENAME=mapguideopensource-server
PACKAGEDIR=mapguideserver
DIRLIST="bin lib Logs Packages Repositories RepositoryAdmin Resources Schema Temp Trash Wfs Wms"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$"

# source ./dpkgbuild.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}/webserverextensions
ROOT=${BUILDROOT}/debian/mapguidewebextensions
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
PACKAGENAME=mapguideopensource-webextensions
PACKAGEDIR=mapguidewebextensions
DIRLIST="bin lib www"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$"

#source ./dpkgbuild.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}/webserverextensions
ROOT=${BUILDROOT}/debian/mapguidehttpd
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
PACKAGENAME=mapguideopensource-httpd
PACKAGEDIR=mapguidehttpd
DIRLIST="apache2 php tomcat"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$"

source ./dpkgbuild.sh







