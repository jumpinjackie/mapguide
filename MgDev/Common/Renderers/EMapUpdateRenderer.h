//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef EMAPUPDATERENDERER_H
#define EMAPUPDATERENDERER_H

#include "DWFRenderer.h"
#include <list>

//dwf declare
namespace DWFCore
{
    class DWFUUID;
}

namespace DWFToolkit
{
    class DWFObjectDefinitionResource;
    class DWFImageResource;

    namespace DWFEMapExtension
    {
        class DWFEMapLayer;
    }
}

using namespace DWFCore;
using namespace DWFToolkit;
using namespace DWFToolkit::DWFEMapExtension;

typedef std::map<std::wstring, RS_LayerUIInfo> layerinfo_map;
typedef std::vector<RS_LayerUIInfo> layerinfo_list;

///<summary>
/// Specialization of DWFRenderer for EMap output.
///</summary>
class EMapUpdateRenderer : public DWFRenderer
{
public:
    RENDERERS_API EMapUpdateRenderer(const RS_String& filename,
                                     unsigned int     sequenceId,
                                     bool             allowVSLines,
                                     bool             allowVSAreas);

    RENDERERS_API virtual ~EMapUpdateRenderer();

    RENDERERS_API virtual void StartMap(RS_MapUIInfo*    mapInfo,
                                        RS_Bounds&       extents,
                                        double           mapScale,
                                        double           dpi,
                                        double           metersPerUnit,
                                        CSysTransformer* xformToLL);

    RENDERERS_API virtual void EndMap();

    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                                          RS_FeatureClassInfo* classInfo);

    RENDERERS_API virtual void EndLayer();

    RENDERERS_API void AddLayerGroupInfo(RS_LayerUIInfo& layerInfo);
    RENDERERS_API void AddLayerInfo(RS_LayerUIInfo& layerInfo);

    RENDERERS_API virtual void CmdAddLayer(const RS_String& guid);
    RENDERERS_API virtual void CmdRemoveLayer(const RS_String& guid);
    RENDERERS_API virtual void CmdUpdateLayer(const RS_String& guid);
    RENDERERS_API virtual void CmdAddLayerGroup(const RS_String& guid);
    RENDERERS_API virtual void CmdRemoveLayerGroup(const RS_String& guid);
    RENDERERS_API virtual void CmdUpdateLayerGroup(const RS_String& guid);
    RENDERERS_API virtual void CmdUpdateUIGraphicForLayer(const RS_String& guid);
    RENDERERS_API virtual void CmdUpdateUIGraphicForLayerGroup(const RS_String& guid);

    //TODO
    RENDERERS_API virtual void AddScaleRange(RS_String& layerGuid,
                                             double min,
                                             double max,
                                             std::list<RS_UIGraphic>* uiGraphics );

private:
    // Hidden copy constructor and assignment operator.
    EMapUpdateRenderer(const EMapUpdateRenderer&);
    EMapUpdateRenderer& operator=(const EMapUpdateRenderer&);

    void AddW2DResource(DWFCore::DWFBufferOutputStream* w2dStream,
                        DWFToolkit::DWFObjectDefinitionResource* objdefs,
                        const wchar_t*  role,
                        const wchar_t*  parentId,
                        const double*   transform,
                        const double*   clip,
                        double          zorder);

    DWFImageResource* CreateImageResource(unsigned char* data, int len);

    bool           m_bFirst;
    DWFUUID*       m_uuid;
    std::wstring   m_dwfFilename;
    unsigned int   m_sequenceId;

    layerinfo_map   m_hGroups;

    //Note: we have both a list and a map for the layer info structures
    //in order to ensure that less assumptions are made as to how
    //the API of this class is called. In the normal Start-EndLayer loop
    //all we need is a list of the layer infos so that we can iterate over them
    //at the end when writing out the DWF.
    //On the other hand the map is used only for the EMap transaction commands.
    //This way the commands are separate from generation of useful things like
    //graphics.
    layerinfo_map   m_hLayerInfoMap;
    layerinfo_list  m_lLayerInfos;
    std::map<RS_String, DWFEMapLayer*> m_hAddedLayers;
};

#endif
