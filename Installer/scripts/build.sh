#!/bin/bash
APIVERSION=2.2
BUILDNUM=${APIVERSION}.0
BUILDROOT=`pwd`
INSTALLROOT=/usr/local/mapguideopensource-${BUILDNUM}
LOCKFILEDIR=/var/lock/mgserver
MGSOURCE=${BUILDROOT}/mgdev
VERFILE=${MGSOURCE}/Common/ProductVersion.h
SVNROOT="svn://svn.bld.mgproto.net"
#SVNROOT="http://svn.osgeo.org"

rm -rf ${MGSOURCE}
rm -rf ${INSTALLROOT}

MGDEVREV=5624
FUSIONREV=2366
CSMAPREV=1964

REVISION=`svn info ${SVNROOT}/mapguide/branches/2.2/Installer | perl revnum.pl`
echo ${REVISION} > revnum.txt


echo "Building Revision ${BUILDNUM}.${REVISION}" 
svn export --ignore-externals -r ${MGDEVREV}  svn://svn.bld.mgproto.net/mapguide/branches/2.2/MgDev ${MGSOURCE}
svn export -r ${FUSIONREV} svn://svn.bld.mgproto.net/fusion/branches/fusion-2.2 ${MGSOURCE}/Oem/fusion
svn export -r ${CSMAPREV} svn://svn.bld.mgproto.net/metacrs/csmap/branches/12.02/CsMapDev ${MGSOURCE}/Oem/CsMap

cd ${MGSOURCE}

echo "#ifndef PRODUCTVERSION_H_" > ${VERFILE}
echo "#define PRODUCTVERSION_H_" >> ${VERFILE}
echo 'const STRING ProductVersion = L"'${BUILDNUM}'.'${REVISION}'";' >> ${VERFILE}
echo 'const STRING ApiVersion     = L"'${APIVERSION}'";' >> ${VERFILE}
echo '#endif' >> ${VERFILE}

pushd ${MGSOURCE}/Oem/LinuxApt
./build_apt.sh --prefix ${INSTALLROOT} --with-tomcat
popd

./build_oem.sh --prefix=${INSTALLROOT}

pushd ${MGSOURCE}/Oem/fusion
ant prepare
ant compress
popd

aclocal
libtoolize --force
automake --add-missing --copy
autoconf
./configure --enable-optimized --prefix=${INSTALLROOT}
make
make install

# Prepare binaries for packaging by removing unnecessary
# .la and .a files.  Remove executable permission to make Debian packager happy
# Keep symbols around for Debian packaging
pushd ${INSTALLROOT}
LIBDIRS="lib server/lib webserverextensions/lib" 
for libdir in ${LIBDIRS}
do
   # Remove any .la and .a files in lib directories
   rm -f ${libdir}/*.la
   rm -f ${libdir}/*.a

   # Remove execute permission from files in lib directory
   for file in `find ${libdir}/lib*.so* -type f -print`
   do
      chmod a-x ${file}
   done
done
popd

# Tarball the whole works and put it in a bin directory
pushd ${BUILDROOT}
if [ ! -d bin ]; then
   mkdir -p bin
fi
tar -zcf bin/mapguideopensource-${BUILDNUM}.${REVISION}_symbols.tar.gz ${INSTALLROOT} ${LOCKFILEDIR}
popd


# For the final executables, strip the symbols before tarballing
pushd ${INSTALLROOT}
LIBDIRS="lib server/lib webserverextensions/lib" 
for libdir in ${LIBDIRS}
do
   # Strip symbols from final executable
   for file in `find ${libdir}/lib*.so* -type f -print`
   do
      strip --strip-unneeded ${file}
   done
done
popd

# Tarball the whole works and put it in a bin directory
pushd ${BUILDROOT}
if [ ! -d bin ]; then
   mkdir -p bin
fi
tar -zcf bin/mapguideopensource-${BUILDNUM}.${REVISION}.tar.gz ${INSTALLROOT} ${LOCKFILEDIR}
popd
