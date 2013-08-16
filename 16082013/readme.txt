Jenkins CI configuration
========================

Build Artifacts produced by this setup
======================================

== Manually triggered from the Jenkins dashboard ==

 * FDO 3.8 (32-bit/64-bit)
    * Prepared for MapGuide/mg-desktop. No distribution tarballs as we can't build the ArcSDE provider
 * MapGuide Open Source 2.5 (32-bit/64-bit)
    * Windows Installer (32-bit/64-bit)
    * InstantSetup bundles (32-bit/64-bit)
 * mg-desktop 2.5 (32-bit/64-bit)
    * nuget packages (32-bit/64-bit)
    * zip distributions (32-bit/64-bit)

== Automatically triggered by Jenkins (off of svn commit polling) ==

 * MapGuide Maestro 5.0
 * MapGuide Maestro 4.0.x

Required Tools
==============

Doesn't have to be *exact versions* where specified, but different versions may require editing Jenkins Job files
and/or MapGuide/FDO setenvironment.bat to point to the correct locations

 * Microsoft .net Framework 4.0
 * Microsoft Visual C++ Express 2010 with Service Pack 1
 * Windows SDK 7.1 (64-bit)
 * Java SDK 6u33 (32/64-bit)
 * Java SDK 7u13 (32/64-bit)
 * Jenkins 1.465
 * 7-zip 9.20
 * ActivePerl 5.14.2 (64-bit)
 * ActiveState ActivePython 2.7.2.5 (64-bit)
 * GNU Bison 2.4.1
 * GNU sed 4.2.1
 * Sandcastle Help File Builder 1.9.4.0
 * TortoiseSVN 1.6.x
 * Slik Subversion 1.6.x
 * Windows Installer XML Toolset 3.5
 * Visual C++ SP1 compiler update for Windows SDK (KB2519277)
 * Apache Ant 1.8.3
 * MySQL Connector C SDK 6.0.2
 * Oracle Instant Client 11.2 SDK
 * Doxygen 1.8.0
 * GraphViz 2.30
 * NASM (Netwide Assembler) 2.10
 * Microsoft HTML Workshop (32-bit)
 * Sphinx (http://sphinx.pocoo.org)

VS Installation Order
=====================

Visual C++ 2010 Express
Windows SDK v7.1 (with Visual C++ compilers option DISABLED and .net tools + reference assemblies ENABLED)
 - Disable C++ compilers because SP1 will remove them (!)
 - Enable .net options because they are required for building C++/CLI code
Visual Studio 2010 SP1
Visual C++ SP1 compiler update for Windows SDK (KB2519277)

Filesystem layout
=================

* Currently configured for MGOS 2.5 and FDO 3.8, adjust these version numbers as necessary
* Will need to revisit for MGOS 2.6 and FDO 3.9 as we'll be moving to VS2012. Fortunately, building with free MS tools is *much* simpler in VS2012.

C:\apache-ant-1.8.3     [extracted location of apache ant v1.8.3]
C:\builds               [all build artifacts produced by Jenkins will be here]
C:\nasm	                [extract nasm binaries here]
C:\working
    libs
        fdo_rdbms_thirdparty
            mysql       [extract/copy MySQL Connector C 32-bit headers/libs here]
            mysql_x64   [extract/copy MySQL Connector C 64-bit headers/libs here]
            oracle      [extract 32-bit oracle 11.2 instant client sdk here]
            oracle_x64  [extract 64-bit oracle 11.2 instant client sdk here]
        fdo_extras      [Optional files to overlay on top of build result]
            3.8.0
                x64     [extract/copy ArcSDE dlls and providers.xml from FDO SDK]
                x86     [extract/copy ArcSDE dlls and providers.xml from FDO SDK]
    sources
        fdo-3.8         [svn checkout of http://svn.osgeo.org/fdo/branches/3.8]
        mg-2.5
            Installer   [svn checkout of http://svn.osgeo.org/mapguide/branches/2.5/Installer]
            MgDev       [svn checkout of http://svn.osgeo.org/mapguide/branches/2.5/MgDev]
    JenkinsCI           [svn checkout of http://svn.osgeo.org/mapguide/trunk/Tools/JenkinsCI]
        home
            slave_win   [the path of JENKINS_HOME]

Required Jenkins Plugins
========================
 
 * MSBuild

Jenkins Post-install config
===========================

 * Environment variables (already defined in JenkinsCI checkout. Double check paths)
    * MG_BUILD_ARTIFACT_PATH = C:\builds
    * MG_TOOL_PATH = C:\working\JenkinsCI\tools
    * MG_LIB_ROOT = C:\working\libs
    * MG_BUILD_AREA = C:\working\build_area
    * WINDOWS_SDK_71_ROOT = C:\Program Files\Microsoft SDKs\Windows\v7.1
    * ANT_HOME = C:\apache-ant-1.8.3

PATH environment variable
=========================

Make sure the following tools are accessible from the PATH env var:

 * NASM
 * GraphViz
 * Doxygen
 * SlikSvn
 * 7-zip
 * Python (installer should do this)
 * Perl (installer should do this)

Test that the following commands are accessible from the command line:

 * bison
 * sed
 * svn
 * perl
 * doxygen
 * graphviz
 * nasm
 * 7z
 * sphinx-build

Jenkins MapGuide/FDO build process
==================================

 * Clears out any existing build area and artifact directories
 * Exports FDO/MapGuide from the working copy location to the build area
 * 64-bit builds: Runs any pre-processing on the build area to support 64-bit compilation with Windows SDK 7.1 C++ compiler
 * Runs the build and copies the build artifacts to the designated directory
 * FDO only: Overlay any extra files (eg. ArcSDE provider) onto the build artifacts directory

TODO
====

 * Regularly scheduled job to auto svn update the working copies and to trigger new MapGuide/FDO builds off of that
 * Equivalent Ubuntu/CentOS build/job configurations
 * Aggregate all the Windows/Linux build environments into a master/slave configuration.