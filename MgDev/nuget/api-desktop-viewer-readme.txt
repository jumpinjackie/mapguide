mg-desktop-viewer-net40 README
==============================

This is the NuGet package for the mg-desktop viewer API.

Dependencies
============

- mapguide-api-base
- mg-desktop-net40

Optional Dependencies
=====================

- cs-map-dictionaries

Installation Notes
==================

This package will add the required project references to your application:

 - OSGeo.MapGuide.Viewer
 - OSGeo.MapGuide.Viewer.Desktop

Ensure that your executable project's <Platform Target> is updated to match the architecture-specific package you installed. eg. If you installed the x64 package, you must set the <Platform Target> to be x64. Setting it to x86 or AnyCPU may cause a System.BadImageFormatException to be thrown when loading the mg-desktop dlls

This package does not include the CS-Map coordinate system dictionaries. It is assumed you will install the optional cs-map-dictionaries package or have an existing copy of these dictionaries that will be copied to the Dictionaries subdirectory of your project's output directory.

After package installation, you may need to edit Platform.ini to ensure all the paths defined point to the correct locations

As part of your application's startup, you have to call:

 MgdPlatform.Initialize("<path-to>\\Platform.ini");

before you can use most parts of the MapGuide API.

======= CHANGELOG ==========
