//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef MG_PROFILING_SERVICE_H
#define MG_PROFILING_SERVICE_H

/// \defgroup MgProfilingService MgProfilingService
/// \ingroup Profiling_Service_Module
/// \{

/// \brief
/// This class provides services to profile exsiting MapGuide APIs 
/// \since 2.4
class MG_MAPGUIDE_API MgProfilingService : public MgService
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
        MgMap* map,
        MgSelection* selection,
        MgRenderingOptions* options) = 0;

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
        MgMap* map,
        MgSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection) = 0;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProfileService object.
    ///
    MgProfilingService();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_ProfilingService_ProfilingService;
};
/// \}

#endif
