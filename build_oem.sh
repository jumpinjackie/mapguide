#
#! /bin/bash
#

# MapGuide Open Source build script.
#
#**********************************************************
# Initialization
#**********************************************************

echo "MapGuide Open Source build script for OEM components"
cd Oem

INSTALLDIR=/usr/local/mapguideopensource


#**********************************************************
# Build ACE 5.4
# Notes: none
#**********************************************************

echo "ACE 5.4: Initialization ............................"
cd ACE/ACE_wrappers
ACE_PATH="${PWD}"

echo "ACE 5.4: Configuration / Building .................."
cd ace
env ACE_ROOT="${ACE_PATH}" make

echo "ACE 5.4: Build Successful .........................."
cd ../../..


#**********************************************************
# Build DWFTK 7.1
# Notes: none
#**********************************************************

echo "DWFTK 7.1: Initialization .........................."

# dwfcore
cd DWFTK7.1/develop/global/build/gnu/dwfcore
sh ./build_setup.sh

# dwftoolkit
cd ../dwftoolkit
sh ./build_setup.sh

#dwfemap
cd ../dwfemap
sh ./build_setup.sh

# configure and build dwfcore
echo "DWFTK 7.1: Configure dwfcore ......................."
cd ../../../src/dwfcore
sh ./configure --enable-optimized --prefix="${INSTALLDIR}"

echo "DWFTK 7.1: Make dwfcore ............................"
make

# configure and build dwftoolkit
echo "DWFTK 7.1: Configure dwftoolkit ...................."
cd ../dwf
sh ./configure --enable-optimized --prefix="${INSTALLDIR}"

echo "DWFTK 7.1: Make dwftoolkit ........................."
make

# configure and build emaptoolkit
echo "DWFTK 7.1: Configure emaptoolkit ..................."
cd ../dwfemap
sh ./configure --enable-optimized --prefix="${INSTALLDIR}"

echo "DWFTK 7.1: Make emaptoolkit ........................"
make 

echo "DWFTK 7.1: Build Successful ........................"
cd ../../../../..


#**********************************************************
# Build GEOS 2.2.0
# Notes: none
#**********************************************************

echo "GEOS 2.2.0: Initialization ........................."
cd geos-2.2.0

echo "GEOS 2.2.0: Configuration .........................."
sh ./configure --prefix="${INSTALLDIR}"

echo "GEOS 2.2.0: Make ..................................."
make

echo "GEOS 2.2.0: Build Successful ......................."
cd ..


#**********************************************************
# Build SWIGEx 1.0
# Notes: none
#**********************************************************

echo "SWIGEx 1.0: Initialization ........................."
cd SWIGEx
mkdir -p Linux/obj/release

echo "SWIGEx 1.0: Configuration / Building ..............."
make

echo "SWIGEx 1.0: Build Successful ......................."
cd ..


#**********************************************************
# Build BDBXML 2.2.13
# Notes: none
#**********************************************************

echo "BDBXML 2.2.13: Initialization ......................"
cd dbxml-2.2.13

echo "BDBXML 2.2.13: Configuration / Building ............"
sh ./buildall.sh

echo "BDBXML 2.2.13: Build Successful ...................."
cd ..


#**********************************************************
# Build CPPUNIT 1.9.14
# Notes: none
#**********************************************************

echo "CPPUNIT 1.9.14: Initialization ....................."
cd CppUnit-1.9.14

echo "CPPUNIT 1.9.14: Configuration ......................"
sh ./configure --prefix="${INSTALLDIR}"

echo "CPPUNIT 1.9.14: Make ..............................."
make

echo "CPPUNIT 1.9.14: Build Successful ..................."
cd ..


#**********************************************************
# Build IMake 1.0
# Notes: none
#**********************************************************

echo "IMake 1.0: Initialization .........................."
cd ../BuildTools/WebTools/IMake
mkdir -p Linux/obj/debug

echo "IMake 1.0: Configuration / Building ................"
make

echo "IMake 1.0: Build Successful ........................"


#**********************************************************
# Done
#**********************************************************

echo "Done building OEM components ......................."
