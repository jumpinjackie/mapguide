========================================================================
    CONSOLE APPLICATION : GeometryConsoleTest Project Overview
========================================================================

This application is designed to be a module which can be used to test and debug the Open Source GIS
platform without the additional burden of having to have full blown MAP or MapGuide running on the
development system.  Currently, the module only has code to test the new Grid/Graticule portion of the
platform.  It is expected that a complete series of regression test will be added.  Hopefully, this same
module can be made usable in the Linux environemnt as well.  See below for some useage notes.
/////////////////////////////////////////////////////////////////////////////

The project file expects that the GIISPLATFORMROOT environmental variable is set to the root
of the MapGuide development directroy tree.  ON my system this is:

C:\Perforce\caldiesel\MgDev

Obviously, this will be different on your system.  You may want to add this environmental
variable to the standard environment list maintained by Windows, or use the command window
and "devenv" trick.  See below about runtime considerations before you make your decision.

/////////////////////////////////////////////////////////////////////////////

At runtime, Windows (and Linux I presume) will need to know where to look for all the
DLL's associated with the platform.  MgFoundation.dll, MgGeometry.dll, etc.  One way to
achieve this is to have your PATH variable modified to include the appropriate directories.

Thus, I suggest that you use a command file (setPath.bat for example) which accomplishes all
of the following:

1> Add the directories in which all of the appropiate DLL's reside to the PATH variable.
2> Set the MENTOR_DICTIONARY_PATH variable to point to the location of the dictionaries (*.CSD files).
3> Set the value of the GISPLATFORMROOTDIR variable.
4> Activate the Visual Studio variable setting for command line operation.

I then start the development environment from the command line.  This seems to work rather
weel, and the whole platform can be tested/debugged without having to wait for MAP or MapGuide
to put themselves together.

/////////////////////////////////////////////////////////////////////////////
