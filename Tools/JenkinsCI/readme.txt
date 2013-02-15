Jenkins CI configuration
========================

Required Tools
==============

 * Microsoft .net Framework 4.0
 * Microsoft Visual C++ Express 2010 with Service Pack 1
 * Windows SDK 7.1
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

C:\apache-ant-1.8.3		[extracted location of apache ant v1.8.3]
C:\builds
	- revnum.pl [copy from http://svn.osgeo.org/fdo/branches/2.5/Installer/scripts/revnum.pl]
C:\fdo-3.8-x86 	[svn wc of http://svn.osgeo.org/fdo/branches/3.8]
C:\fdo-3.8-x64 	[svn wc of http://svn.osgeo.org/fdo/branches/3.8]
C:\mg-2.5-x86
	- Installer [svn wc of http://svn.osgeo.org/mapguide/branches/2.5/Installer]
	- MgDev		[svn wc of http://svn.osgeo.org/mapguide/branches/2.5/MgDev]
C:\mg-2.5-x64
	- Installer [svn wc of http://svn.osgeo.org/mapguide/branches/2.5/Installer]
	- MgDev		[svn wc of http://svn.osgeo.org/mapguide/branches/2.5/MgDev]