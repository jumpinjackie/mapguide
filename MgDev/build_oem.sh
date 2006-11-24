#
#! /bin/bash
#

# MapGuide Open Source build script.
#



#**********************************************************
# Initialization
#**********************************************************

echo "MapGuide Open Source build script for OEM components"
INSTALLDIR=/usr/local/mapguideopensource
CLEAN_FLAG=0
while [ $# -gt 0 ]; do    # Until you run out of parameters . . .
  case "$1" in
    -prefix|--prefix)
              INSTALLDIR="$2"
              shift
              ;;
    -clean|--clean)
	      CLEAN_FLAG=1
	      shift
	      ;;
    -help|--help)
              echo "Usage: $0 (options)"
              echo "Options:"
              echo "  --prefix [installation directory]"
	      echo "  --clean [clean all objects and binaries in Oem]"
	      echo "  --help [Display usage]"
	      exit	
              ;;
  esac
  shift       # Check next set of parameters.
done




echo "OEM Build Initialization ............................"
echo "Oem Libraries will be installed at: ${INSTALLDIR}"

LIB_NAME=""

#**********************************************************
# Generic Functions
#**********************************************************
check_build ()
{
  if [ $? -ne 0 ]; then
    error=$?
    echo "$LIB_NAME: Error build failed ($error)................."
    exit $error
  fi
}
check_clean ()
{
  if [ $? -ne 0 ]; then
    error=$?
    echo "$LIB_NAME: Error clean failed ($error)................."
    exit $error
  fi
}


#**********************************************************
# Build ACE 5.4
# Notes: none
#**********************************************************
init_ace ()
{
	LIB_NAME="ACE 5.4"
}

build_ace ()
{
	pushd ACE/ACE_wrappers
	ACE_PATH="${PWD}"
	pushd ace
	env ACE_ROOT="${ACE_PATH}" make
	check_build
	popd 
	popd
}

clean_ace ()
{
	pushd ACE/ACE_wrappers
	ACE_PATH="${PWD}"
	pushd ace
	env ACE_ROOT="${ACE_PATH}" make clean
	check_clean
	popd 
	popd
}


#**********************************************************
# Build DWFCORE 1.0
# Notes: none
#**********************************************************
init_dwfcore ()
{
	LIB_NAME="DWFCORE 1.0"
	pushd DWFTK7.1/develop/global/build/gnu/dwfcore
	sh ./build_setup.sh
	popd
}

build_dwfcore ()
{
	pushd DWFTK7.1/develop/global/src/dwfcore
	sh ./configure --enable-optimized --prefix="${INSTALLDIR}"
	make
	check_build
	popd
}

clean_dwfcore ()
{
	pushd DWFTK7.1/develop/global/src/dwfcore
	make clean
	check_clean
	popd

}

#**********************************************************
# Build DWFTOOLKIT 7.1
# Notes: none
#**********************************************************
init_dwftk ()
{
	LIB_NAME="DWFTK 7.1"
	pushd DWFTK7.1/develop/global/build/gnu/dwftoolkit
	sh ./build_setup.sh
	popd	
}

build_dwftk ()
{
	pushd DWFTK7.1/develop/global/src/dwf
	sh ./configure --enable-optimized --prefix="${INSTALLDIR}"
	make
	check_build
	popd
}

clean_dwftk ()
{
	pushd DWFTK7.1/develop/global/src/dwf
	make clean
	check_clean
	popd

}
#**********************************************************
# Build DWFEMAP 1.0
# Notes: none
#**********************************************************
init_dwfemap ()
{
	LIB_NAME="DWFEMAP 1.0"
	pushd DWFTK7.1/develop/global/build/gnu/dwfemap
	sh ./build_setup.sh
	popd	
}

build_dwfemap ()
{
	pushd DWFTK7.1/develop/global/src/dwfemap
	sh ./configure --enable-optimized --prefix="${INSTALLDIR}"
	make
	check_build
	popd
}

clean_dwfemap ()
{
	pushd DWFTK7.1/develop/global/src/dwfemap
	make clean
	check_clean
	popd
}
#**********************************************************
# Build GEOS 2.2.0
# Notes: none
#**********************************************************
init_geos ()
{
	LIB_NAME="GEOS 2.2.0"
}

build_geos ()
{
	pushd geos-2.2.0
	sh ./configure --prefix="${INSTALLDIR}"
	make
	# The check build is disabled as the build will fail with automake version < 2.59
	# check_build
	popd
}

clean_geos ()
{
	pushd geos-2.2.0
	make clean
	check_clean
	popd
}

#**********************************************************
# Build SWIGEx 1.0
# Notes: none
#**********************************************************
init_swigex ()
{
	LIB_NAME="SWIGEx 1.0"
}

build_swigex ()
{
	pushd SWIGEx
	mkdir -p Linux/obj/release	
	make
	check_build
	popd
}

clean_swigex ()
{
	pushd SWIGEx
	# Note: the qmake system is required to regenerate the Makefile.
	#   	This is disabled.
	# make clean
	# check_clean
	popd
}

#**********************************************************
# Build BDBXML 2.2.13
# Notes: none
#**********************************************************
init_bdxml ()
{
	LIB_NAME="BDBXML 2.2.13"
}

build_bdxml ()
{
	pushd dbxml-2.2.13
	sh ./buildall.sh
	check_build
	popd 
}

clean_bdxml ()
{
	pushd dbxml-2.2.13
	sh ./buildall.sh --clean
	check_clean
	popd 
}
#**********************************************************
# Build CPPUNIT 1.9.14
# Notes: none
#**********************************************************

init_cppunit ()
{
	LIB_NAME="CPPUNIT 1.9.14"
}

build_cppunit ()
{
  	pushd CppUnit-1.9.14
	sh ./configure --prefix="${INSTALLDIR}"
	make 
	check_build
	popd
}

clean_cppunit ()
{
  	pushd CppUnit-1.9.14
	sh ./configure --prefix="${INSTALLDIR}"
	make clean 
	check_clean
	popd
}

#**********************************************************
# Build LIBFCGI 2.4.0
# Notes: none
#**********************************************************

init_libfcgi ()
{
	LIB_NAME="LIBFCGI 2.4.0"
}

build_libfcgi ()
{
  	pushd fcgi/fcgi-2.4.0
	sh ./configure --prefix="${INSTALLDIR}"
	make 
	check_build
	popd
}

clean_libfcgi ()
{
  	pushd fcgi/fcgi-2.4.0
	sh ./configure --prefix="${INSTALLDIR}"
	make clean 
	check_clean
	popd
}

#**********************************************************
# Build IMake 1.0
# Notes: none
#**********************************************************
init_imake ()
{
	LIB_NAME="IMake 1.0"
}

build_imake ()
{
	pushd ../BuildTools/WebTools/IMake
	mkdir -p Linux/obj/debug
	make
	check_build
	popd
}

clean_imake ()
{
	pushd ../BuildTools/WebTools/IMake
	# Note: does not support clean as qmake is needed to regenerate the Makefile.
	# make clean 
	# check_clean
	popd
}

#**********************************************************
# Script loop 
#**********************************************************
pushd Oem
for lib in ace dwfcore dwftk dwfemap geos swigex bdxml cppunit libfcgi imake;
do
	echo "$lib: Initialization..........................."
	init_"$lib"
	
	if test "$CLEAN_FLAG" = "1"; then
		echo "$lib: Clean ....................................."
		clean_"$lib"
		echo "$lib: Clean Successful .........................."
	else
		echo "$lib: Configure /Build..........................."
		build_"$lib"	
		echo "$lib: Build Successful .........................."
	fi	
done
popd
