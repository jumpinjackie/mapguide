#!/bin/bash

# Build directory structure underneath directory of this script
# debian/ - common build directory for all packages
#   control - generated all packages control file with subst params
#   changelog - generated changelog file
#   substvars - subst params created by dpkg-shlibdeps
#   fdo[provider]/ - directory to build FDO Provider
#     usr/local/fdo-3.9.0/ - copied tree for core fdo libs
#     DEBIAN/
#       control - control file generated from dpkg-gencontrol
#       symbols - symbols file generated from dpkg-gensymbols
# 

FDO_VER_MAJOR=${FDO_VER_MAJOR:-4}
FDO_VER_MINOR=${FDO_VER_MINOR:-0}
FDO_VER_REV=${FDO_VER_REV:-0}

# Read and validate command line parameters
#
FDO_ARCH="$1"
BUILDNUM="$2"
PROVIDER="$3"
if test -z ${FDO_ARCH}; then 
  echo Usage: dpkgfdoprovider.sh [i386/amd64] svnChangeNum provider
  exit -1
fi
if test -z ${BUILDNUM}; then 
  echo Usage: dpkgfdoprovider.sh [i386/amd64] svnChangeNum provider
  exit -1
fi

case $FDO_ARCH in 
i386)
  echo Setting architecture to i386.
  ;;
amd64)
  echo Setting architecture to amd64.
  ;;
*)
  echo Usage: dpkgfdoprovider.sh [i386/amd64] svnChangeNum provider
  echo Invalid architecture.  Please specify i386 or amd64.
  exit -1
  ;;
esac

case $PROVIDER in
sdf) ;;
shp) ;;
wms) ;;
wfs) ;;
arcsde) ;;
rdbms) ;;
gdal) ;;
ogr) ;;
kingoracle) ;;
sqlite) ;;
*)
  echo Usage: dpkgfdoprovider.sh [i386/amd64] svnChangeNum provider
  echo Invalid provider.  Please specify one of the following providers:
  echo sdf,shp,wms,wfs,arcsde,rdbms,gdal,ogr,postgis,kingoracle,sqlite
  exit -1
  ;;
esac


BUILDROOT=`pwd`
FDOBUILD=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
FDO=usr/local/fdo-${FDOBUILD}
ROOT=${BUILDROOT}/debian/fdo${PROVIDER}
CPROOT=${ROOT}/${FDO}

# Create output directory structure and eat error messag
#
if [ ! -d tmp ]; then
  mkdir tmp >& /dev/null
  mkdir bin >& /dev/null
  mkdir debian >& /dev/null
fi


# Blow away old build directory
rm -rf ${ROOT}


# Create and populate documentation directory for provider
mkdir -p ${ROOT}/usr/share/doc/fdo-${PROVIDER}
cp debian/copyright ${ROOT}/usr/share/doc/fdo-${PROVIDER}
cp debian/changelog ${ROOT}/usr/share/doc/fdo-${PROVIDER}/changelog.Debian
gzip -9 ${ROOT}/usr/share/doc/fdo-${PROVIDER}/changelog.Debian


# Create directories for binary package build and copy install image
# into build directory.  Also include fdocore directories for completeness
for file in `cat filelist/fdocoredir.lst` 
do
  mkdir -p ${CPROOT}/${file}
done

for file in `cat filelist/${PROVIDER}dir.lst` 
do
  mkdir -p ${CPROOT}/${file}
done

for file in `cat filelist/${PROVIDER}.lst` 
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
dpkg-shlibdeps --ignore-missing-info -pfdo${PROVIDER} ${CPROOT}/lib/*.so
dpkg-gensymbols -q -p"fdo-${PROVIDER}" -P"debian/fdo${PROVIDER}"
export LD_LIBRARY_PATH=

# Now generate a filled in control file for the binary package
# using information from control, changelog, and substvars
dpkg-gencontrol -p"fdo-${PROVIDER}" -P"debian/fdo${PROVIDER}"

# Build binary package from ROOT
# And move resulting debian package and lintian results to build directory
dpkg-deb -Zlzma --build ${ROOT}
lintian -i debian/fdo${PROVIDER}.deb > tmp/fdo${PROVIDER}.lintian
mv debian/fdo${PROVIDER}.deb bin/fdo-${PROVIDER}_${FDOBUILD}-${BUILDNUM}_${FDO_ARCH}.deb
popd
