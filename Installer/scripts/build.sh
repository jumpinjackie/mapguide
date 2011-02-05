#!/bin/bash
APIVERSION=2.3
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

REVISION=`svn info ${SVNROOT}/mapguide/trunk/MgDev | perl revnum.pl`
echo ${REVISION} > revnum.txt
echo "Building Revision ${BUILDNUM}.${REVISION}" 
svn export -q -r ${REVISION} ${SVNROOT}/mapguide/trunk/MgDev ${MGSOURCE}

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
# .la and .a files and stripping unneeded symbols from the binaries
pushd ${INSTALLROOT}
LIBDIRS="lib server/lib webserverextensions/lib" 
for libdir in ${LIBDIRS}
do
   # Remove any .la and .a files in lib directories
   rm -f ${libdir}/*.la
   rm -f ${libdir}/*.a

   # Remove unneeded symbols from files in the lib directories
   for file in `find ${libdir}/lib*.so* -type f -print`
   do
      strip --strip-unneeded ${file}
      chmod a-x ${file}
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
