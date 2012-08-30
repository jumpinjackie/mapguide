#ifndef DESKTOP_PLATFORM_INIT_H
#define DESKTOP_PLATFORM_INIT_H

/// \ingroup Desktop_Module
/// \{

///\brief
/// A static utility class that provides the means for initializing and tearing down
/// the MapGuide Platform. You must initialize the platform before you can use any
/// service classes or the Coordinate System API.
class MG_DESKTOP_API MgdPlatform
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////
    ///\brief
    /// Gets whether the the MapGuide Desktop Platform has been initialized. You
    /// cannot use most MapGuide service classes until the platform has been
    /// initialized. Call MgdPlatform::Initialize(CREFSTRING) to initialize the
    /// platform.
    ///
    ///\return
    /// true if the platform has been initialized.
    static bool IsInitialized();

    ///////////////////////////////////////////////////////////////////////////////
    ///\brief
    /// Initializes the MapGuide Desktop Platform. This must be called at least 
    /// once before using any service classes in the MapGuide API. Subsequent calls
    /// will do nothing.
    ///
    ///\param configFile (String/string)
    /// The path to the MapGuide Desktop Platform configuration file (Platform.ini)
    static void Initialize(CREFSTRING configFile);

    ///////////////////////////////////////////////////////////////////////////////
    ///\brief
    /// Performs cleanup of the MapGuide Desktop Platform. This should be called
    /// as part of your application's cleanup or exit routine. You should not use
    /// any other classes in the MapGuide API after completing this call.
    static void Terminate();

private:
    static bool sm_init;
};
/// \}
#endif