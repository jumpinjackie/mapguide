mapguide-api-base README
=======================

This is the NuGet package for the MapGuide Platform API. This is the base package that is required by implementations of the MapGuide Platform API, which are currently:

 - mapguide-api-web
 - mg-desktop-net40 

This package contains the Foundation, Geometry and PlatformBase components of the MapGuide Platform API.

Dependencies
============

- mapguide-api-base

Installation Notes
==================

This package will add the required project references to your application:

 - OSGeo.MapGuide.Foundation
 - OSGeo.MapGuide.Geometry
 - OSGeo.MapGuide.PlatformBase

The ability to use the classes within these libraries depends on how you initialized the platform. See installation notes for (mapguide-api-web) or (mg-desktop-net40).

If you are using this package on its own (for example, you want to use the MgCoordinateSystem classes), then the only initialization requirement is to define the MENTOR_DICTIONARY_PATH environment variable before you use any MgCoordinateSystem class. This variable should be the path to your CS-Map coordinate system dictionaries

Unlike the assemblies that come with your MapGuide Open Source installation, the assemblies in this package are signed. You cannot use this package if you require un-signed MapGuide assemblies.