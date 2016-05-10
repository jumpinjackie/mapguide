#!/bin/bash

# Build directory structure underneath directory of this script
# debian/ - common build directory for all packages
#   control - generated all packages control file with subst params
#   changelog - generated changelog file
#   substvars - subst params created by dpkg-shlibdeps
#   fdocore/ - directory to build FDO Base libs package
#     usr/local/fdo-3.9.0/ - copied tree for core fdo libs
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
# 

FDO_VER_MAJOR=${FDO_VER_MAJOR:-4}
FDO_VER_MINOR=${FDO_VER_MINOR:-0}
FDO_VER_REV=${FDO_VER_REV:-0}

BUILDROOT=`pwd`
FDOBUILD=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
FDO=usr/local/fdo-${FDOBUILD}
ROOT=${BUILDROOT}/debian/fdocore
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${FDO}

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
  echo Usage: dpkgfdocore.sh [i386/amd64] svnChangeNum
  exit -1
fi
if test -z ${BUILDNUM}; then 
  echo Usage: dpkgfdocore.sh [i386/amd64] svnChangeNum
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
  echo Usage: dpkgfdocore.sh [i386/amd64] svnChangeNum
  echo Invalid architecture.  Please specify i386 or amd64.
  exit -1
  ;;
esac


# Create control file
#
cat > debian/control <<END-OF-CONTROL
Source: fdo-src
Maintainer: FDO Internals Mail List <fdo-internals@lists.osgeo.org>
Section: misc
Priority: optional
Homepage: http://fdo.osgeo.org

Package: fdo-core
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdocore:Depends}
Description:  OSGeo FDO ${FDOBUILD} base libraries

Package: fdo-sdf
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdosdf:Depends}
Description:  OSGeo FDO ${FDOBUILD} SDF Provider

Package: fdo-shp
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdoshp:Depends}
Description:  OSGeo FDO ${FDOBUILD} SHP Provider

Package: fdo-wfs
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdowfs:Depends}
Description:  OSGeo FDO ${FDOBUILD} WFS Provider

Package: fdo-wms
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdowms:Depends}
Description:  OSGeo FDO ${FDOBUILD} WMS Provider

Package: fdo-arcsde
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdoarcsde:Depends}
Description:  OSGeo FDO ${FDOBUILD} ArcSDE Provider

Package: fdo-rdbms
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdordbms:Depends}
Description:  OSGeo FDO ${FDOBUILD} Providers for ODBC, MySQL and PostgreSQL

Package: fdo-gdal
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdogdal:Depends}
Description:  OSGeo FDO ${FDOBUILD} GDAL Provider

Package: fdo-ogr
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdoogr:Depends}
Description:  OSGeo FDO ${FDOBUILD} OGR Provider

Package: fdo-kingoracle
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdokingoracle:Depends}
Description:  OSGeo FDO ${FDOBUILD} King Oracle Provider

Package: fdo-sqlite
Architecture: ${ARCH}
Section: misc
Priority: optional
Depends: \${fdosqlite:Depends}
Description:  OSGeo FDO ${FDOBUILD} SQLite Provider
END-OF-CONTROL


# Create changelog file
# TODO: pull revision history from Subversion
#
CURRTIME=`date -R`
cat > debian/changelog <<END-OF-CHANGELOG
fdo-src (${FDOBUILD}-${BUILDNUM}) experimental; urgency=low

  * Pre-release build.

 -- FDO Internals Mail List <fdo-internals@lists.osgeo.org>  ${CURRTIME}
END-OF-CHANGELOG


# Pull copyright file from Subversion vault 
wget http://svn.osgeo.org/fdo/branches/3.8/License_README.txt -O tmp/copyright
iconv -f ISO-8859-1 -t UTF-8 tmp/copyright > debian/copyright

# Blow away old build directory
rm -rf ${ROOT}

# Create and populate documentation directory for fdo-core
mkdir -p ${ROOT}/usr/share/doc/fdo-core
cp debian/copyright ${ROOT}/usr/share/doc/fdo-core
cp debian/changelog ${ROOT}/usr/share/doc/fdo-core/changelog.Debian
gzip -9 ${ROOT}/usr/share/doc/fdo-core/changelog.Debian


# Create directories for binary package build and copy
# install image into build directory
for file in `cat filelist/fdocoredir.lst` 
do
  mkdir -p ${CPROOT}/${file}
done

for file in `cat filelist/fdocore.lst` 
do
  cp -a /${FDO}/${file} ${CPROOT}/${file}
done

# Remove any .la files in the lib directory since
# lintian complains about them
rm -f ${CPROOT}/lib/*.la

# Remove unneeded symbols from files in the lib directory
# and remove their executable status to clean up some lintian errors
for file in `find ${CPROOT}/lib/lib*.so* -type f -print`
do
  strip --strip-unneeded ${file}
  chmod a-x ${file}
done


# Call dpkg-shlibdeps to build out library dependencies
# This will create the substvars file
# LD_LIBRARY_PATH must be set to find private libs
mkdir -p ${ROOT}/DEBIAN
pushd ${BUILDROOT}
# Fdo build creates full path symlinks for third party libs
# Add install directory to path to compensate
#export LD_LIBRARY_PATH=${CPROOT}/lib
export LD_LIBRARY_PATH=${CPROOT}/lib:/${FDO}/lib
dpkg-shlibdeps -pfdocore --ignore-missing-info ${CPROOT}/lib/*.so
dpkg-gensymbols -q -p"fdo-core" -P"debian/fdocore"
export LD_LIBRARY_PATH=

# Now generate a filled in control file for the binary package
# using information from control, changelog, and substvars
dpkg-gencontrol -p"fdo-core" -P"debian/fdocore"

# Build binary package from ROOT
# And move resulting debian package and lintian results to build directory
dpkg-deb -Zlzma --build ${ROOT}
lintian -i debian/fdocore.deb > tmp/fdocore.lintian
mv debian/fdocore.deb bin/fdo-core_${FDOBUILD}-${BUILDNUM}_${ARCH}.deb
popd
