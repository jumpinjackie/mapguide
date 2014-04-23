mg-desktop-net40 README
=======================

This is the NuGet package for the mg-desktop API. This does not include the WinForms-based map viewer control. Install the mg-desktop-viewer-net40 package if you require this component.

Dependencies
============

- mapguide-api-base

Optional Dependencies
=====================

- cs-map-dictionaries

Installation Notes
==================

This package will add the required project references to your application:

 - OSGeo.MapGuide.Desktop

This package does not include the CS-Map coordinate system dictionaries. It is assumed you will install the optional cs-map-dictionaries package or have an existing copy of these dictionaries that will be copied to the Dictionaries subdirectory of your project's output directory.

After package installation, you may need to edit Platform.ini to ensure all the paths defined point to the correct locations

As part of your application's startup, you have to call:

 MgdPlatform.Initialize("<path-to>\\Platform.ini");

before you can use most parts of the MapGuide API.

======= CHANGELOG ==========
