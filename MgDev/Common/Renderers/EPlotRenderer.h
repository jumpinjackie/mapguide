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

#ifndef EPLOTRENDERER_H
#define EPLOTRENDERER_H

#include "DWFRenderer.h"

namespace DWFToolkit
{
    class DWFEPlotSection;
    class DWFPackageWriter;
};

namespace DWFCore
{
    class DWFUUID;
}

using namespace DWFCore;

///<summary>
/// Specialization of DWFRenderer for EPlot output.
/// An EPlotRenderer needs to be initialized with
/// page size and units settings
///</summary>
class EPlotRenderer : public DWFRenderer
{
public:
    RENDERERS_API EPlotRenderer(const RS_String& filename,
                                double           pageWidth,
                                double           pageHeight,
                                const RS_String& pageUnits);

    RENDERERS_API virtual ~EPlotRenderer();

    ///////////////////////////////////////////////////////////////////
    // DWFRenderer overloads
    //
    RENDERERS_API virtual void StartMap(RS_MapUIInfo*    mapInfo,
                                        RS_Bounds&       extents,
                                        double           mapScale,
                                        double           dpi,
                                        double           metersPerUnit,
                                        CSysTransformer* xformToLL = NULL);

    RENDERERS_API virtual void EndMap();

    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                                          RS_FeatureClassInfo* classInfo);

    RENDERERS_API virtual void EndLayer();

    RENDERERS_API virtual void Done();

    ////////////////////////////////////////////////
    // SE_Renderer overloads
    //
    virtual bool OptimizeGeometry(); 

    ///////////////////////////////////////////////////////////////////
    // EPlot specific stuff
    //
    RENDERERS_API void SetPageWidth(double width);

    RENDERERS_API void SetPageHeight(double height);

    RENDERERS_API void SetPageSizeUnits(std::wstring units);

    RENDERERS_API void SetMapWidth(double width);

    RENDERERS_API void SetMapHeight(double height);

    RENDERERS_API void EnableLayoutPlot();

    inline double & mapWidth()
    {
        return m_dMapWidth;
    }

    inline double & mapHeight()
    {
        return m_dMapHeight;
    }

    inline double & mapOffsetX()
    {
        return m_dMapOffsetX;
    }

    inline double & mapOffsetY()
    {
        return m_dMapOffsetY;
    }

    inline double & mapBoundsHeight()
    {
        return m_dMapBoundsHeight;
    }

    bool IsLayoutPlot();

private:
    // Hidden copy constructor and assignment operator.
    EPlotRenderer(const EPlotRenderer&);
    EPlotRenderer& operator=(const EPlotRenderer&);

    std::wstring m_dwfFilename;

    std::wstring m_pageUnits;

    double m_dPageWidth;
    double m_dPageHeight;

    DWFToolkit::DWFPackageWriter* m_dwfPackageWriter;

    bool m_bFirst;

    DWFUUID* m_uuid;

    double m_dMapWidth;   // the width of the map in page units
    double m_dMapHeight;  // the height of the map in page units
    double m_dMapOffsetX; // the X offset of the map in page units
    double m_dMapOffsetY; // the Y offset of the map in page units
    double m_dMapBoundsHeight; // the height of the map bounds on the page

    bool m_bLayoutPlot;

    stream_list m_lSectionStreams;
    stream_list m_lSectionLabelStreams;
    stream_list m_lSectionLayoutStreams;
    stream_list m_lSectionLayoutLabelStreams;

    void AddW2DResource(DWFCore::DWFBufferOutputStream* w2dStream,
                        DWFToolkit::DWFObjectDefinitionResource* objdefs,
                        const double*   transform,
                        const double*   clip,
                        const wchar_t*  role,
                        const wchar_t*  name);
};

#endif
