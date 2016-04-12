#!/bin/bash

# Build directory structure underneath directory of this script
# debian/ - common build directory for all packages
#   control - generated all packages control file with subst params
#   changelog - generated changelog file
#   substvars - subst params created by dpkg-shlibdeps
#   mapguideplatformbase/ - packaging directory for common MapGuide components
#     usr/local/mapguideopensource-x.y.z/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidecommon/ - packaging directory for common MapGuide components
#     usr/local/mapguideopensource-x.y.z/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidecoordsys/ - packaging directory for CS-Map coordinate system dictionaries
#     usr/local/mapguideopensource-x.y.z/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidecoordsyslite/ - packaging directory for CS-Map coordinate system dictionaries
#     usr/local/mapguideopensource-x.y.z/ - copied tree for common components
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguideserver/ - packaging directory for MapGuide Server
#     usr/local/mapguideopensource-x.y.z/ - copied tree for Server
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidewebextensions/ - packaging directory for Web Extensions
#     usr/local/mapguideopensource-x.y.z/ - copied tree for Web Extensions
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#
#   mapguidehttpd/ - packaging directory for Apache Bundle
#     usr/local/mapguideopensource-x.y.z/ - copied tree for Apache bundle
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
#

MG_VER_MAJOR=${MG_VER_MAJOR:-3}
MG_VER_MINOR=${MG_VER_MINOR:-0}
MG_VER_REV=${MG_VER_REV:-1}

FDO_VER_MAJOR=${FDO_VER_MAJOR:-4}
FDO_VER_MINOR=${FDO_VER_MINOR:-0}
FDO_VER_REV=${FDO_VER_REV:-0}

BUILDROOT=`pwd`
MGBUILD=${MG_VER_MAJOR}.${MG_VER_MINOR}.${MG_VER_REV}
FDOBUILD=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
MGINST=usr/local/mapguideopensource-${MGBUILD}
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

Package: mapguideopensource-platformbase
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguideplatformbase:Depends}
Description:  Base platform components for OSGeo MapGuide ${MGBUILD}

Package: mapguideopensource-common
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${mapguidecommon:Depends}
Description:  OSGeo MapGuide ${MGBUILD} server-specific common components

Package: mapguideopensource-coordsys
Architecture: ${ARCH}
Section: msic
Priority: optional
Depends: \${mapguidecoordsys:Depends}
Description:  CS-Map Coordinate System Dictionary data files

Package: mapguideopensource-coordsys-lite
Architecture: ${ARCH}
Section: msic
Priority: optional
Depends: \${mapguidecoordsys:Depends}
Description:  CS-Map Coordinate System Dictionary data files. Lite version. Excludes country-specific grid files

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

  * ${BUILDNUM}

 -- MapGuide Internals Mail List <mapguide-internals@lists.osgeo.org>  ${CURRTIME}
END-OF-CHANGELOG


# Pull copyright file from Subversion vault 
wget -N http://svn.osgeo.org/mapguide/trunk/MgDev/License.txt -O tmp/copyright
iconv -f ISO-8859-1 -t UTF-8 tmp/copyright > debian/copyright

MGINST=usr/local/mapguideopensource-${MGBUILD}
PACKAGENAME=mapguideopensource-platformbase
PACKAGEDIR=mapguideplatformbase
ROOT=${BUILDROOT}/debian/mapguideplatformbase
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
DIRLIST="lib"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$ libdwf"
EXCLUDEFILE=platformbase_excludes.txt

source ./dpkgbuild.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}
PACKAGENAME=mapguideopensource-common
PACKAGEDIR=mapguidecommon
ROOT=${BUILDROOT}/debian/mapguidecommon
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
DIRLIST="lib"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$ libdwf"
EXCLUDEFILE=mapguidecommon_excludes.txt

source ./dpkgbuild.sh

EXCLUDEFILE=
PACKAGENAME=mapguideopensource-coordsys
PACKAGEDIR=mapguidecoordsys
ROOT=${BUILDROOT}/debian/mapguidecoordsys
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
DIRLIST="share"
REMOVELIST="\.a\$ \.la\$ \.c\$ \.o\$ \.mak\$ \.nmk\$"
STRIPLIST="\.so\$"

source ./dpkgbuild.sh

source ./csmap_optimize.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}/server
ROOT=${BUILDROOT}/debian/mapguideserver
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
PACKAGENAME=mapguideopensource-server
PACKAGEDIR=mapguideserver
DIRLIST="bin lib Logs Packages Repositories RepositoryAdmin Resources Schema Temp Trash Wfs Wms"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$"

source ./dpkgbuild.sh

MGINST=usr/local/mapguideopensource-${MGBUILD}/webserverextensions
ROOT=${BUILDROOT}/debian/mapguidewebextensions
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
PACKAGENAME=mapguideopensource-webextensions
PACKAGEDIR=mapguidewebextensions
DIRLIST="bin lib www"
REMOVELIST="\.a\$ \.la\$"
STRIPLIST="\.so\$"

source ./dpkgbuild.sh

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







