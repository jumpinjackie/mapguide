//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef EMAPRENDERER_H
#define EMAPRENDERER_H

#include "DWFRenderer.h"
#include <list>

//dwf declare
namespace DWFCore
{
    class DWFUUID;
}

namespace DWFToolkit
{
    class DWFImageResource;

    namespace DWFEMapExtension
    {
        class DWFEMapLayer;
        class DWFEMapLayerGroup;
    }
}

using namespace DWFCore;
using namespace DWFToolkit;
using namespace DWFToolkit::DWFEMapExtension;

typedef std::map<std::wstring, RS_LayerUIInfo> layerinfo_map;

///<summary>
/// Specialization of DWFRenderer for EMap output.
///</summary>
class EMapRenderer : public DWFRenderer
{
public:
    RENDERERS_API EMapRenderer(const RS_String& filename,
                               const RS_String& agentUri);

    RENDERERS_API virtual ~EMapRenderer();


    ///////////////////////////////////////////////////////////////////
    // DWFRenderer overloads

    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                                          RS_FeatureClassInfo* classInfo);

    RENDERERS_API virtual void EndLayer();

    RENDERERS_API virtual void StartMap(RS_MapUIInfo*    mapInfo,
                                        RS_Bounds&       extents,
                                        double           mapScale,
                                        double           dpi,
                                        double           metersPerUnit,
                                        CSysTransformer* xformToLL = NULL);

    RENDERERS_API virtual void EndMap();

    ///////////////////////////////////////////////////////////////////
    // EMap specific stuff:

    //adds a scale range to current layer
    RENDERERS_API virtual void AddScaleRange(double min,
                                             double max,
                                             std::list<RS_UIGraphic>* uiGraphics);

    //defines a layer group
    RENDERERS_API virtual void AddLayerGroup(RS_LayerUIInfo& layerInfo);

    //adds a thumbnail graphic
//  RENDERERS_API virtual void AddUIGraphic();

    // TODO: For EMapRenderer we also need to overload all geometry
    //       stylization functions to do nothing so that we make sure
    //       no geometry is sent with GetMap requests.  For now we
    //       just take care not to send geometry.

private:
    // Hidden copy constructor and assignment operator.
    EMapRenderer(const EMapRenderer&);
    EMapRenderer& operator=(const EMapRenderer&);

    DWFImageResource* CreateImageResource(unsigned char* data, int len);

    RS_String m_dwfFilename;
    RS_String m_agentUri;

    DWFUUID* m_uuid;

    DWFEMapLayer* m_pLayer;

    layerinfo_map m_hGroups;
};

#endif
