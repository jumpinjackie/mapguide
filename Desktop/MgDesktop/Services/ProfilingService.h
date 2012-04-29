#ifndef DESKTOP_PROFILING_SERVICE_H
#define DESKTOP_PROFILING_SERVICE_H

#include "MgDesktop.h"

class MgRenderingOptions;
class MgRenderingService;

/// \defgroup MgProfilingService MgProfilingService
/// \ingroup Profiling_Service_Module
/// \{

/// \brief
/// This class provides services to profile exsiting MapGuide APIs 
class MG_DESKTOP_API MgProfilingService : public MgService
{
PUBLISHED_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Profile the process of rendering all dynamic layers in the specified 
    /// MgMap to a dynamic overlay image with a transparent background.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param options
    /// Input
    /// rendering options
    ///
    /// \return
    /// A byte reader containing the profile result
    ///
    virtual MgByteReader* ProfileRenderDynamicOverlay(
        MgdMap* map,
        MgdSelection* selection,
        MgRenderingOptions* options);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Profile the process of rendering a MgMap to the requested image 
    /// format with specified center point and scale.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param center
    /// Input
    /// map center point. Specifies the center point for the map
    /// \param scale
    /// Input
    /// map scale. Specifies the scale for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    ///
    /// \return
    /// A byte reader containing the profile result
    ///
    virtual MgByteReader* ProfileRenderMap(
        MgdMap* map,
        MgdSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProfileService object.
    ///
    MgProfilingService();

EXTERNAL_API:
	virtual ~MgProfilingService();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
	virtual INT32 GetClassId() { return m_cls_id; }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
	virtual void Dispose() { delete this; }

private:
	MgRenderingService* m_svcRendering;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_ProfilingService_ProfilingService;
};
/// \}

#endif