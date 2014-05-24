Jenkins CI configuration
========================

Build Artifacts produced by this setup
======================================

== Manually triggered from the Jenkins dashboard ==

 * FDO 3.9 (32-bit/64-bit)
    * Prepared for MapGuide/mg-desktop. No distribution tarballs as we can't build the ArcSDE provider
 * MapGuide Open Source 2.6 (32-bit/64-bit)
    * Windows Installer (32-bit/64-bit)
    * InstantSetup bundles (32-bit/64-bit)
 * mg-desktop 2.6 (32-bit/64-bit)
    * nuget packages (32-bit/64-bit)
    * zip distributions (32-bit/64-bit)

== Automatically triggered by Jenkins (off of svn commit polling) ==

 * MapGuide Maestro trunk

Required Tools
==============

Doesn't have to be *exact versions* where specified, but different versions may require editing Jenkins Job files
and/or MapGuide/FDO setenvironment.bat to point to the correct locations

 * Microsoft .net Framework 4.0
 * Microsoft Visual Studio 2012 Express for Windows Desktop
 * Java SDK 6 (32/64-bit)
 * Java SDK 7 (32/64-bit)
 * Jenkins
 * 7-zip 9.20
 * ActivePerl 5.14.2 (64-bit)
 * ActiveState ActivePython 2.7.2.5 (64-bit)
 * GNU Bison 2.4.1
 * GNU sed 4.2.1
 * Sandcastle Help File Builder 1.9.4.0
 * TortoiseSVN 1.8.x
 * Slik Subversion 1.8.x
 * Windows Installer XML Toolset 3.5
 * Apache Ant 1.8.3
 * MySQL Connector C SDK 6.0.2
 * Oracle Instant Client 11.2 SDK
 * Doxygen 1.8.0 (must be this specific version)
 * GraphViz 2.30
 * NASM (Netwide Assembler) 2.10
 * Microsoft HTML Workshop (32-bit)
 * Sphinx (http://sphinx.pocoo.org)

Filesystem layout
=================

* Currently configured for MGOS 2.6 and FDO 3.9, adjust these version numbers as necessary

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
            3.9.0
                x64     [extract/copy ArcSDE dlls and providers.xml from FDO SDK]
                x86     [extract/copy ArcSDE dlls and providers.xml from FDO SDK]
    sources
        fdo-3.9         [svn checkout of http://svn.osgeo.org/fdo/branches/3.9]
        mg-2.6
            Installer   [svn checkout of http://svn.osgeo.org/mapguide/branches/2.6/Installer]
            MgDev       [svn checkout of http://svn.osgeo.org/mapguide/branches/2.6/MgDev]
    JenkinsCI           [svn checkout of http://svn.osgeo.org/mapguide/trunk/Tools/JenkinsCI]
        home
            slave_win   [the path of JENKINS_HOME]

Required Jenkins Plugins
========================
 
 * MSBuild
 * TextFinder

Jenkins Post-install config
===========================

 * Environment variables (already defined in JenkinsCI checkout. Double check paths)
    * MG_BUILD_ARTIFACT_PATH = C:\builds
    * MG_TOOL_PATH = C:\working\JenkinsCI\tools
    * MG_LIB_ROOT = C:\working\libs
    * MG_BUILD_AREA = C:\working\build_area
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

Updating svn working copies
===========================

SVN working copies under C:\working\sources have to be manually updated. You can run svnupdate.bat and setrevnums.bat under C:\working\JenkinsCI\tools to do this.

The reason these SVN working copies have to be manually updated is to eliminate svn contention for the working copy directory when running FDO and MapGuide build 
jobs in parallel. There were many build failures in the past due to svn operations done from within the build jobs causing lock and sharing violation issues on 
the working copy directories. Making svn-related operations manual and outside of the build jobs eliminates this issue.

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