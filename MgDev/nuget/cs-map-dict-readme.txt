cs-map-dictionaries README
==========================

This is an optional content-only NuGet package containing a subset of the CS-Map Coordinate System Dictionary files.

These files are required if your application uses any MgCoordinateSystem* class in the MapGuide API.

To conserve package size, country-specific grid files have been omitted from this package.

If you are building normal web applications with the MapGuide Web API, this package is not necessary as your application will use the dictionary files specified in webconfig.ini (which normally point to dictionary files that come with your MapGuide Open Source installation)

This package is only required if you are building a mg-desktop application (you installed the mg-desktop-net40 package), or you are building a non-standard application that uses the MgCoordinateSystem* classes in the MapGuide API.

Installation Notes
==================

This package will add a post-build step to copy the dictionary files to a "Dictionaries" subdirectory in the project's output directory.

If you installed the mg-desktop-net40 package, it's Platform.ini expects the dictionary files to be copied to this location and thus does not require any further action on your part if you installed both packages.

For non-standard applications that use the MgCoordinateSystem* classes, you must set the MENTOR_DICTIONARY_PATH environment variable to indicate the directory containing these dictionary files. You must do this before you access the MgCoordinateSystemFactory class.