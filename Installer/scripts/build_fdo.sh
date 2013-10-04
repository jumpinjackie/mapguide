#!/bin/bash

# build_fdo.sh
#
# Builds FDO and creates a tarball from the binary output
#
# Modify setenvironment.sh to point to the correct Oracle, MySQL and PostgreSQL paths
# before building

FDO_BUILD_COMPONENT=

# FDO version. Make sure this matches your FDO build source
FDO_VER_MAJOR=3
FDO_VER_MINOR=8
FDO_VER_REV=1

# FDO install directory
FDO_VER_FULL=${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}
FDO_INST=/usr/local/fdo-${FDO_VER_FULL}

check_build()
{
    error=$?
    if [ $error -ne 0 ]; then
        echo "[error]: ${FDO_BUILD_COMPONENT} - Error build failed ($error)"
        exit $error
    fi
}

check_fdo_lib()
{
	libname=$1-${FDO_VER_FULL}.so
	libpath=${FDO_INST}/lib/${libname}
	if [ ! -e ${libpath} ]; then
		echo "[error]: Error building ${libname}"
		exit 1
	fi
}

BUILDROOT=`pwd`

LOCALSVN=1
PRESERVE_BUILD_ROOT=1
CMAKE=0

FDO_SRC=/home/mgbuild/fdo/trunk
#FDO_SRC=http://svn.osgeo.org/fdo/trunk
FDO_BUILD_AREA=${BUILDROOT}/fdo_build_area

modify_sdk_paths()
{
	echo "[info]: Updating setenvironment.sh"
	# Note: Change your paths here if they're different
	sed -i 's/export FDOMYSQL=$FDOTHIRDPARTY\/mysql\/rhlinux/export FDOMYSQL=\/usr/g' ${FDO_BUILD_AREA}/setenvironment.sh
	sed -i 's/export FDOPOSTGRESQL=$FDOTHIRDPARTY\/pgsql/export FDOPOSTGRESQL=\/usr/g' ${FDO_BUILD_AREA}/setenvironment.sh
	echo "export FDOORACLE=/home/mgbuild/fdo_rdbms_thirdparty/oracle/x86/instantclient_11_2/sdk" >> ${FDO_BUILD_AREA}/setenvironment.sh
}

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "[error]: You must run this script with superuser privileges"
	exit 1
fi

echo "***********************************************************"
echo " FDO Source: ${FDO_SRC}"
echo " FDO Build Area: ${FDO_BUILD_AREA}"
echo " FDO Install dir: ${FDO_INST}"
echo " CMake build: ${CMAKE}"
echo " Export from local SVN checkout: ${LOCALSVN}"
echo " Re-use previous build area: ${PRESERVE_BUILD_ROOT}"
echo "***********************************************************"
start_time=`date +%s`
REVISION=`svn info ${FDO_SRC} | perl revnum.pl`

if [ ${CMAKE} -eq 1 ];
then
	echo "[error]: CMake build of FDO not supported yet"
	exit 1
else
	echo "[info]: Using automake build"
	if [ -d ${FDO_BUILD_AREA} ];
	then
		if [ ${PRESERVE_BUILD_ROOT} -eq 1 ];
		then
			echo "[info]: FDO build area ${FDO_BUILD_AREA} exists. Going straight to build"
		else
			echo "[info]: Removing old FDO build area at ${FDO_BUILD_AREA}"
			rm -rf ${FDO_BUILD_AREA}
			if [ ${LOCALSVN} -eq 1 ] 
			then
				svn export ${FDO_SRC} ${FDO_BUILD_AREA}
				modify_sdk_paths
			else
				echo "[info]: Performing fresh SVN export of ${FDO_SRC} (r${REVISION}) to ${FDO_BUILD_AREA}"
				svn export -q -r ${REVISION} ${FDO_SRC} ${FDO_BUILD_AREA}
				modify_sdk_paths
			fi			
		fi
	else
		echo "[info]: FDO build area ${FDO_BUILD_AREA} does not exist. Doing svn export"
		echo "[info]: Exporting svn revision ${REVISION}"
		if [ ${LOCALSVN} -eq 1 ] 
		then
			svn export ${FDO_SRC} ${FDO_BUILD_AREA}
			modify_sdk_paths
		else
			echo "[info]: Performing fresh SVN export of ${FDO_SRC} (r${REVISION}) to ${FDO_BUILD_AREA}"
			svn export -q -r ${REVISION} ${FDO_SRC} ${FDO_BUILD_AREA}
			modify_sdk_paths
		fi
	fi
fi

echo "[info]: Building FDO (${FDO_VER_MAJOR}.${FDO_VER_MINOR}.${FDO_VER_REV}) rev (${REVISION})"
cd ${FDO_BUILD_AREA}
source ./setenvironment.sh
if [ ! -d ${FDO_INST} ];
then
	echo "[error]: FDO install directory ${FDO_INST} doesn't exist. setenvironment.sh should've created this directory. Ensure the FDO version number in this script is correct"
	exit 1;
fi
FDO_BUILD_COMPONENT="FDO Thirdparty"
./build_thirdparty.sh
check_build

if [ ${CMAKE} -eq 1 ];
then
	FDO_BUILD_COMPONENT="FDO (cmake)"
	echo "[error]: CMake build of FDO not supported yet"
	exit 1;
else
	#NOTE: We never build ArcSDE provider because we haven't paid the ESRI tax for their ArcSDE SDK
	for comp in fdocore fdo utilities shp sqlite gdal ogr wfs wms rdbms kingoracle sdf
	do
		FDO_BUILD_COMPONENT="$comp (automake)"
		./build_linux.sh --w $comp --p ${FDO_INST}
		check_build
	done
fi
check_fdo_lib libFDO
check_fdo_lib libExpressionEngine
check_fdo_lib libSDFProvider
check_fdo_lib libSHPProvider
check_fdo_lib libSHPOverrides
check_fdo_lib libWFSProvider
check_fdo_lib libWMSProvider
check_fdo_lib libWMSOverrides
# ArcSDE provider currently disabled due to missing libraries
#./buildfdoprovider.sh arcsde
#check_fdo_lib libArcSDEProvider
check_fdo_lib libFdoMySQL
check_fdo_lib libFdoPostgreSQL
check_fdo_lib libFdoODBC
check_fdo_lib libSchemaMgr_OV
check_fdo_lib libGRFPProvider
check_fdo_lib libGRFPOverrides
check_fdo_lib libOGRProvider
check_fdo_lib libKingOracleProvider
check_fdo_lib libKingOracleOverrides
check_fdo_lib libSQLiteProvider

end_time=`date +%s`

FDO_BUILD_COMPONENT="Remove .la files from ${FDO_INST}"
# Remove .la files from lib directory
rm -f ${FDO_INST}/lib/*.la
check_build

FDO_BUILD_COMPONENT="Strip so symbols and remove execute flag"
# Remove unneeded symbols from files in the lib directory
# and make them non-executable
for file in `find ${FDO_INST}/lib/lib*.so* -type f -print`
do
	strip --strip-unneeded ${file}
	chmod a-x ${file}
done
check_build

cd $BUILDROOT
FDO_BUILD_COMPONENT="Make tarball"
# Create a binary tar ball for FDO
tar -zcf fdosdk-centos5-${FDO_VER_FULL}_${REVISION}.tar.gz ${FDO_INST}
check_build

echo "[info]: FDO build complete!"
echo Main build execution time: `expr $end_time - $start_time` s
