#!/bin/bash

# Make sure setvars.sh is called first before running this script

if [ ! -d ${JAVA_HOME} ];
then
echo "ERROR: Environment variable JAVA_HOME not set. Please set this enviroment variable first"
exit
fi

if [ ! ${MG_BUILD_VARS_SET:-0} -eq 1 ];
then
echo "Please call setvars.sh first (source ./setvars.sh)"
exit 1
fi

if [ ! ${PRESERVE_BUILD_ROOT} -eq 1 ]
then
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
    if [ ${LOCALSVN} = 1 ] 
    then
        echo "Making local SVN copy to ${MGSOURCE}"
        cp -R ${SVNROOT}${SVNRELPATH} ${MGSOURCE}
        echo "Cleaning out .svn directories"
        find . -name .svn -exec rm -rf {} \;
    else
        echo "Performing fresh SVN export to ${MGSOURCE}"
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
./build_apt.sh --prefix ${INSTALLROOT} --with-tomcat
popd

# Need an ubuntu-flavoured build_oem.sh if we're doing ubuntu
if [ ${UBUNTU} -eq 1 ]
then
    cp ${BUILDROOT}/build_oem_ubuntu.sh .
    chmod +x build_oem_ubuntu.sh
    echo "Building Oem (Ubuntu)"
    ./build_oem_ubuntu.sh --prefix ${INSTALLROOT}
else
    echo "Building Oem (Regular)"
    ./build_oem.sh --prefix ${INSTALLROOT}
fi

echo "Building Fusion"
pushd ${MGSOURCE}/Oem/fusion
ant prepare
ant compress
popd

echo "Building MapGuide"
aclocal
libtoolize --force
automake --add-missing --copy
autoconf
./configure --enable-optimized --prefix=${INSTALLROOT}
make $MY_MAKE_OPTS
make install

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
