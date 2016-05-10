#!/bin/bash

# Global vars for this script. Modify as necessary
MG_VER_MAJOR=${MG_VER_MAJOR:-3}
MG_VER_MINOR=${MG_VER_MINOR:-0}
MG_VER_REV=${MG_VER_REV:-1}

FDO_VER_MAJOR=${FDO_VER_MAJOR:-4}
FDO_VER_MINOR=${FDO_VER_MINOR:-0}
FDO_VER_REV=${FDO_VER_REV:-0}

MG_DISTRO=${MG_DISTRO:-centos6}

FDO_INST=/usr/local/fdo-${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
APIVERSION=${MG_VER_MAJOR}.${MG_VER_MINOR}
BUILDNUM=${APIVERSION}.${MG_VER_REV}
BUILDROOT=`pwd`
MGCPUPLATFORM=${MG_ARCH:-i386}
MGDEBUG=0
INSTALLROOT=
if [ -z "${MG_INST_PATH}" ]; then
    INSTALLROOT=/usr/local/mapguideopensource-${BUILDNUM}
else
    INSTALLROOT=${MG_INST_PATH}
fi
LOCKFILEDIR=/var/lock/mgserver
MGSOURCE=${BUILDROOT}/mgdev
VERFILE=${MGSOURCE}/Common/ProductVersion.h
# Use LOCALSVN=1 if you want to build from an existing copy instead of building
# from an export (saves bandwidth and faster)
LOCALSVN=1
SVNROOT=$HOME
if [ "$MG_HOME" != "" ]; then
    SVNROOT=$MG_HOME
fi

#SVNROOT="svn://svn.bld.mgproto.net"
#SVNROOT="http://svn.osgeo.org"
SVNRELPATH=/mapguide/branches/${MG_VER_MAJOR}.${MG_VER_MINOR}/MgDev
if [ "${MG_BRANCH}" = "trunk" ]; then
    SVNRELPATH=/mapguide/trunk/MgDev
fi
MY_MAKE_OPTS="-j 2"
UBUNTU=${UBUNTU:-0}
PRESERVE_BUILD_ROOT=1

echo "******************************************************************"
echo " MapGuide Version is:           ${BUILDNUM}"
echo " MapGuide will be installed to: ${INSTALLROOT}"
echo " SVN Source is:                 ${SVNROOT}${SVNRELPATH}"
echo " Make Options:                  ${MY_MAKE_OPTS}"
echo " Is Ubuntu?:                    ${UBUNTU}"
echo " Debug Build?:                  ${MGDEBUG}"
echo " Preserving the build dir?:     ${PRESERVE_BUILD_ROOT}"
echo " JAVA_HOME:                     ${JAVA_HOME}"
echo " Using FDO headers from:        ${FDO_INST}/include"
echo " Using FDO libs from:           ${FDO_INST}/lib"
echo "******************************************************************"

# Need JAVA_HOME for JavaApi
if [ ! -d ${JAVA_HOME} ];
then
    echo "ERROR: Environment variable JAVA_HOME not set. Please set this enviroment variable first"
    exit 1
fi

mg_check_build()
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

if [ ${UBUNTU} -eq 1 ];
then
    echo "[info]: Performing Ubuntu-specific modifications"
    # GCC 4.8 is causing too much instability, so downgrade one version
    #GCCVER=4.7
    #export GCCVER
    #CC=gcc-$GCCVER
    #export CC
    #CXX=g++-$GCCVER
    #export CXX
    #echo "[info]: Using GCC $GCCVER for Ubuntu"

    # Patch some build scripts and makefiles to accept our chosen compiler

    # CS-Map Dictionary Compiler
    #echo "[info]: Patch CS-Map compiler makefile"
    #sed -i 's/gcc \$/'"$CC"' \$/g' ${MGSOURCE}/Oem/CsMap/Dictionaries/Compiler.mak
    #sed -i 's/gcc -v/'"$CC"' -v/g' ${MGSOURCE}/Oem/CsMap/Dictionaries/Compiler.mak

    # libpng makefile
    #echo "[info]: Patch libpng makefile"
    #sed -i 's/CC=cc/CC='"$CC"'/g' ${MGSOURCE}/Oem/gd/lpng/scripts/makefile.std

    # AGG 2.4 Linux Makefile
    #echo "[info]: Replace agg-2.4 makefile"
    #echo "AGGLIBS= -lagg " > ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo "AGGCXXFLAGS = -O3 -I/usr/X11R6/include -L/usr/X11R6/lib" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo "CXX = $CXX" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo "C = $CC" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo "LIB = ar cr" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo "" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
    #echo ".PHONY : clean" >> ${MGSOURCE}/Oem/agg-2.4/Makefile.in.Linux
else
    echo "[info]: Performing CentOS-specific modifications"
    #sed -i 's/sh .\/buildall.sh/sh .\/buildall.sh --with-xerces-conf=--enable-transcoder-iconv/g' ${MGSOURCE}/build_oem.sh
    #sed -i 's/sh .\/buildall.sh --with-xerces-conf=--enable-transcoder-iconv --clean/sh .\/buildall.sh --clean/g' ${MGSOURCE}/build_oem.sh
fi

start_time=`date +%s`

echo "Building Revision ${BUILDNUM}.${REVISION}" 
cd ${MGSOURCE}

echo "#ifndef PRODUCTVERSION_H_" > ${VERFILE}
echo "#define PRODUCTVERSION_H_" >> ${VERFILE}
echo 'const STRING ProductVersion = L"'${BUILDNUM}'.'${REVISION}'";' >> ${VERFILE}
echo 'const STRING ApiVersion     = L"'${APIVERSION}'";' >> ${VERFILE}
echo '#endif' >> ${VERFILE}

# Note: Remove for trunk as this has been included into build_oem.sh
echo "Building LinuxApt"
pushd ${MGSOURCE}/Oem/LinuxApt
BUILD_COMPONENT="LinuxApt"
./build_apt.sh --prefix ${INSTALLROOT} --with-tomcat
mg_check_build
popd

BUILD_COMPONENT="Oem"
if [ "${MG_ARCH}" = "amd64" ] || [ $(uname -m) = "x86_64" ]; then
    if [ ${MGDEBUG} -eq 1 ]; then
        echo "Building Oem (64-bit, debug)"
        ./build_oem.sh --prefix ${INSTALLROOT} --build 64 --config debug
        mg_check_build
    else
        echo "Building Oem (64-bit, release)"
        ./build_oem.sh --prefix ${INSTALLROOT} --build 64
        mg_check_build
    fi
else
    if [ ${MGDEBUG} -eq 1 ]; then
        echo "Building Oem (32-bit, debug)"
        ./build_oem.sh --prefix ${INSTALLROOT} --config debug
        mg_check_build
    else
        echo "Building Oem (32-bit, release)"
        ./build_oem.sh --prefix ${INSTALLROOT}
        mg_check_build
    fi
fi

# Note: Remove for trunk as this has been included into build_oem.sh
echo "Building Fusion"
pushd ${MGSOURCE}/Oem/fusion
BUILD_COMPONENT="Fusion"
ant prepare
ant compress
mg_check_build
popd

echo "Building MapGuide"
BUILD_COMPONENT="MapGuide"
aclocal
libtoolize --force
automake --add-missing --copy
autoconf
if [ "${MG_ARCH}" = "amd64" ] || [ $(uname -m) = "x86_64" ]; then
    MGCPUPLATFORM=amd64
    if [ ${MGDEBUG} -eq 1 ]; then
        ./configure --disable-optimized --enable-silent-rules --enable-64bit --prefix=${INSTALLROOT} --with-fdo-include=${FDO_INST}/include --with-fdo-lib=${FDO_INST}/lib
    else
        ./configure --enable-optimized --enable-silent-rules --enable-64bit --prefix=${INSTALLROOT} --with-fdo-include=${FDO_INST}/include --with-fdo-lib=${FDO_INST}/lib
    fi
else
    MGCPUPLATFORM=i386
    if [ ${MGDEBUG} -eq 1 ]; then
        ./configure --disable-optimized --enable-silent-rules --prefix=${INSTALLROOT} --with-fdo-include=${FDO_INST}/include --with-fdo-lib=${FDO_INST}/lib
    else
        ./configure --enable-optimized --enable-silent-rules --prefix=${INSTALLROOT} --with-fdo-include=${FDO_INST}/include --with-fdo-lib=${FDO_INST}/lib
    fi
fi
make $MY_MAKE_OPTS
mg_check_build
BUILD_COMPONENT="MapGuide Install"
make install
mg_check_build

end_time=`date +%s`

echo "Preparing binaries for packaging"

# Strip the heavy CS-Map dictionary data files. The geoid stuff is not used and the NSRS ones can be packaged separately
echo "Removing unused CS-Map dictionary files"
pushd ${INSTALLROOT}/share/gis/coordsys
rm -rf WW15MGH.GRD
popd
pushd ${INSTALLROOT}/share/gis/coordsys/Usa
rm -rf Geoid03
rm -rf Geoid96
rm -rf Geoid99
rm -rf NSRS2007
rm -rf NSRS2011
popd

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

# Make tarball if not ubuntu
if [ ${UBUNTU} -eq 0 ];
then
    echo "Creating MapGuide Open Source binary tarball"
    # Tarball the whole works and put it in a bin directory
    pushd ${BUILDROOT}
    if [ ! -d bin ]; then
        mkdir -p bin
    fi

    tar -Jcf bin/mapguideopensource-${MG_DISTRO}-${BUILDNUM}.${REVISION}.${MGCPUPLATFORM}.tar.xz ${INSTALLROOT} ${LOCKFILEDIR}
fi
echo "Build complete!"
echo MapGuide main build execution: `expr $end_time - $start_time` s
