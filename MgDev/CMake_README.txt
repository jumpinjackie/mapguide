CMake build for MapGuide
========================

This document contains instructions on how to use CMake to build MapGuide on Linux. 

This is an experimental alternative build system to autotools for building MapGuide on Linux.

Requirements
============

You have FDO already built and installed at /usr/local/fdo-x.y.z (where x.y.z is the current version of FDO)
    - You should build FDO with CMake too

These instructions assume an Ubuntu 14.04 LTS environment.

Install the following packages:

build-essential libpng-dev libjpeg62-dev libxslt-dev libfreetype6-dev libpcre3-dev openjdk-7-jdk 
ant libldap-dev libssl-dev libcurl4-openssl-dev libexpat-dev libmysqlclient-dev unixODBC-dev 
python-dev libpq-dev libcppunit-dev libxalan-c-dev libxerces-c-dev libboost-date-time-dev 
libboost-thread-dev libboost-system-dev libboost-chrono-dev libboost-program-options-dev libgdal-dev 
libjsoncpp-dev libgd-dev libace-dev libjsoncpp-dev cmake 

If you wish to build with ninja instead of make, install the following:

ninja-build

TODO: Specify dep list for CentOS

Usage
=====

1. Create a directory where internal thirdparty components will be built in

    mkdir -p ~/mapguide_oem_build

2. Build the internal thirdparty components specifying the directory where thirdparty components will be built in

    sudo ./cmake_bootstrap.sh --oem_working_dir ~/mapguide_oem_build

   This will copy the DBXML, httpd, PHP and the Tomcat connector sources to your specified directory and build it from there.
   Once completed, an env_vars.sh script will be written to this directory that the following step will use.

   This will generally require elevation (with sudo) as the default install prefix is /usr/local/mapguideopensource-x.y.z where
   x.y.z is the current version of MapGuide.

3. Start the main CMake build

    ./cmake_build.sh --oem_working_dir ~/mapguide_oem_build --cmake_build_dir ~/mapguide_build_area

   This will set up the CMake build using the generated env_vars.sh script and set up a CMake build tree in the directory specified
   by --cmake_build_dir

4. Install the MapGuide binaries. This will generally require elevation (with sudo) as well.

    cd ~/mapguide_build_area
    sudo make install

5. Start the MapGuide Server (replace x.y.z with the current version of MapGuide)

    cd /usr/local/mapguideopensource-x.y.z/server/bin
    ./mgserver.sh

6. Start the Apache Web Server (replace x.y.z with the current version of MapGuide)

    cd /usr/local/mapguideopensource-x.y.z/webserverextensions/apache2/bin
    ./apachectl start

Manual CMake Build
==================

cmake_build.sh is a wrapper around calling CMake with a standard set of options. If you want to invoke CMake manually, do the following
after running cmake_bootstrap.sh.

    mkdir -p ~/mapguide_build_area
    cd ~/mapguide_build_area
    source ~/mapguide_oem_build/env_vars.sh
    cmake <YOUR SOURCE DIRECTORY> -DMG_OEM_WORK_DIR=~/mapguide_oem_build -DPHP_VER=${PHP_VER} -DHTTPD_VER=${HTTPD_VER} -DCMAKE_BUILD_TYPE=$BUILD_CONFIG <other options...>

The CMake build supports the following options. Prefix with "-D" in your cmake command to enable such options:

 - CMAKE_BUILD_TYPE=(Debug|Release)
    Build the source in Debug or Release configuration

 - WITH_JAVA=(TRUE|FALSE)
    Enable Java support. This will build the Java API wrappers and enable Tomcat install/integration.
    If not set, these parts are skipped.
 
 - WITH_DESKTOP=(TRUE|FALSE)
    Enable building mg-desktop.

 - SANITIZE_ADDRESS=(ON|OFF)
    Build the source with AddressSanitizer (ASAN) support. 
    This allows one to catch a certain class of errors during execution that previously would require an external tool like valgrind.
    You should also be building your source in Debug configuration if you enable this option.

Help/Support
============

File a ticket on https://trac.osgeo.org/mapguide or send an email to the mapguide-users mailing list.

TODO
====

 - Integrate with system-provided httpd/php if available and in supported version range.
 - Support full internal Oem build (currently only covers components we know not to be provided by Ubuntu)
 - Windows support and vcpkg integration