===============================================================================
    CONSOLE APPLICATION : GeometryConsoleTest Project Overview
===============================================================================

This application is designed to be a module which can be used to test and debug
the Open Source GIS platform without the additional burden of having to have
full blown MAP or MapGuide running on the development system.  The module
serves as a shell in which one or more test sequences can be coded.  Currently,
they are separated by conditional compile preprocessor statements which limit
the executable to a single test.

Typically, this module needs to run from an environment where environment
variables GISPLATFORMROOTDIR and MENTOR_DICTIONARY_PATH are properly set.
Also, you will probably need to set the PATH variable to include paths to the
folders containing over modules referenced by the MgGeometry.dll.  The
ACE_wrappers is an example of such a need.  Pasted below is the command line
script this author uses for testin and debugging:

set GISPLATFORMROOTDIR=C:\Development\Perforce\Map\trunk\Components\MgDev
set MENTOR_DICTIONARY_PATH=%GISPLATFORMROOTDIR%\Oem\CsMap\Dictionaries
subst Q: %GISPLATFORMROOTDIR%
echo Setting the PATH variable to Platform componentes in the %GISPLATFORMROOTDIR" directory.
echo off
set PATH=%PATH%;Q:\Common\bin\debug;Q:\Oem\ACE\ACE_wrappers\lib;Q:\Oem\dbxml-2.3.10\bin\debug;Q:\Oem\geos\VisualStudio\Debug
set PATH=%PATH%;Q:\Common\bin\release;Q:\Oem\dbxml-2.3.10\bin;Q:\Oem\geos\VisualStudio\Release
echo on

/////////////////////////////////////////////////////////////////////////////
