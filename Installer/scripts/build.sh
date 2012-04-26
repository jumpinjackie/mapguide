#!/bin/bash

# Global vars for this script. Modify as necessary
APIVERSION=2.4
BUILDNUM=${APIVERSION}.0
BUILDROOT=`pwd`
INSTALLROOT=/usr/local/mapguideopensource-${BUILDNUM}
LOCKFILEDIR=/var/lock/mgserver
MGSOURCE=${BUILDROOT}/mgdev
VERFILE=${MGSOURCE}/Common/ProductVersion.h
# Use LOCALSVN=1 if you want to build from an existing copy instead of building
# from an export (saves bandwidth and faster)
LOCALSVN=1
SVNROOT=/home/user
#SVNRELPATH=/mg-2.4/MgDev
#SVNROOT="svn://svn.bld.mgproto.net"
#SVNROOT="http://svn.osgeo.org"
SVNRELPATH=/mapguide/trunk/MgDev
MY_MAKE_OPTS="-j 4"
UBUNTU=1
PRESERVE_BUILD_ROOT=1

echo "******************************************************************"
echo " MapGuide will be installed to: ${INSTALLROOT}"
echo " SVN Source is: ${SVNROOT}${SVNRELPATH}"
echo " Make Options: ${MY_MAKE_OPTS}"
echo " Is Ubuntu?: ${UBUNTU}"
echo " Preserving the build dir?: ${PRESERVE_BUILD_ROOT}"
echo " JAVA_HOME: ${JAVA_HOME}"
echo "******************************************************************"

# Need JAVA_HOME for JavaApi
if [ ! -d ${JAVA_HOME} ];
then
echo "ERROR: Environment variable JAVA_HOME not set. Please set this enviroment variable first"
exit
fi

check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "$BUILD_COMPONENT: Error build failed ($error)................."
        exit $error
    fi
}

if [ ! ${PRESERVE_BUILD_ROOT} -eq 1 ]
then
    echo "Removing existing build dir at ${MGSOURCE}"
    rm -rf ${MGSOURCE}
else
    echo "Preserving existing build dir"
fi
rm -rf ${INSTALLROOT}

REVISION=`svn info ${SVNROOT}${SVNRELPATH} | perl revnum.pl`
echo ${REVISION} > revnum.txt
if [ ! ${PRESERVE_BUILD_ROOT} -eq 1 -o ! -d ${MGSOURCE} ];
then
    if [ ! -d ${MGSOURCE} ];
    then
    	echo "Build root ${MGSOURCE} does not exist. Doing svn export"
    fi
    echo "Exporting svn revision ${REVISION}"
    if [ ${LOCALSVN} -eq 1 ] 
    then
        echo "Making local SVN copy of ${SVNROOT}${SVNRELPATH} to ${MGSOURCE}"
        cp -R ${SVNROOT}${SVNRELPATH} ${MGSOURCE}
        echo "Cleaning out .svn directories"
	pushd ${MGSOURCE}
        find . -name .svn -exec rm -rf {} \;
	popd
    else
        echo "Performing fresh SVN export of ${SVNROOT}${SVNRELPATH} (r${REVISION}) to ${MGSOURCE}"
        svn export -q -r ${REVISION} ${SVNROOT}${SVNRELPATH} ${MGSOURCE}
    fi
fi
echo "Building Revision ${BUILDNUM}.${REVISION}" 
cd ${MGSOURCE}

echo "#ifndef PRODUCTVERSION_H_" > ${VERFILE}
echo "#define PRODUCTVERSION_H_" >> ${VERFILE}
echo 'const STRING ProductVersion = L"'${BUILDNUM}'.'${REVISION}'";' >> ${VERFILE}
echo 'const STRING ApiVersion     = L"'${APIVERSION}'";' >> ${VERFILE}
echo '#endif' >> ${VERFILE}

echo "Building LinuxApt"
pushd ${MGSOURCE}/Oem/LinuxApt
BUILD_COMPONENT="LinuxApt"
./build_apt.sh --prefix ${INSTALLROOT} --with-tomcat
check_build
popd

# Need an ubuntu-flavoured build_oem.sh if we're doing ubuntu
if [ ${UBUNTU} -eq 1 ]
then
    cp ${BUILDROOT}/build_oem_ubuntu.sh .
    chmod +x build_oem_ubuntu.sh
    echo "Building Oem (Ubuntu)"
    BUILD_COMPONENT="Oem (Ubuntu)"
    ./build_oem_ubuntu.sh --prefix ${INSTALLROOT}
    check_build
else
    echo "Building Oem (Regular)"
    BUILD_COMPONENT="Oem (Regular)"
    ./build_oem.sh --prefix ${INSTALLROOT}
    check_build
fi

echo "Building Fusion"
pushd ${MGSOURCE}/Oem/fusion
BUILD_COMPONENT="Fusion"
ant prepare
ant compress
check_build
popd

echo "Building MapGuide"
BUILD_COMPONENT="MapGuide"
aclocal
libtoolize --force
automake --add-missing --copy
autoconf
./configure --enable-optimized --prefix=${INSTALLROOT}
make $MY_MAKE_OPTS
check_build
BUILD_COMPONENT="MapGuide Install"
make install
check_build

echo "Preparing binaries for packaging"
# Prepare binaries for packaging by removing unnecessary
# .la and .a files and stripping unneeded symbols from the binaries
pushd ${INSTALLROOT}
LIBDIRS="lib server/lib webserverextensions/lib" 
echo "Stripping symbols from binaries"
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

echo "Creating MapGuide Open Source binary tarball"
# Tarball the whole works and put it in a bin directory
pushd ${BUILDROOT}
if [ ! -d bin ]; then
    mkdir -p bin
fi

tar -zcf bin/mapguideopensource-${BUILDNUM}.${REVISION}.tar.gz ${INSTALLROOT} ${LOCKFILEDIR}

echo "Build complete!"
