mapguide-api-web README
=======================

This is the NuGet package for the MapGuide Web Extensions API. This contains the MapGuideCommon and Web parts of the MapGuide API.

Dependencies
============

- mapguide-api-base

Installation Notes
==================

This package will add the required project references to your application:

 - OSGeo.MapGuide.MapGuideCommmon
 - OSGeo.MapGuide.Web

After package installation, copy over the webconfig.ini from your MapGuide Open Source installation and make sure this file is part of your project output.

As part of your application's startup, you have to call:

 MapGuideApi.MgInitializeWebTier("<path-to>\\webconfig.ini");

Before you can use most parts of the MapGuide API. For Web Applications (ASP.net), you can generally call this in the Application_Start() method of your Global.asax

If you installed the x64 package, in order to be able to debug your MapGuide Application, you must have your application use <Local IIS> instead of the built-in web server. This normally requires you run Visual Studio as administrator. The reason for this is simply that the built-in web server is a 32-bit process which cannot load your MapGuide 64-bit dlls.

Unlike the assemblies that come with your MapGuide Open Source installation, the assemblies in this package are signed. You cannot use this package if you require un-signed MapGuide assemblies.