//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGSERVERRENDERINGSERVICE_H
#define MGSERVERRENDERINGSERVICE_H

#include "ServerRenderingDllExport.h"

class MgFeatureInformation;
class GDRenderer;

class MG_SERVER_RENDERING_API MgServerRenderingService : public MgRenderingService
{
    DECLARE_CLASSNAME(MgServerRenderingService)

public:
    MgServerRenderingService(MgConnectionProperties* connection);
    ~MgServerRenderingService(void);

    virtual MgByteReader* RenderTile(MgMap* map,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow);

    virtual MgByteReader* RenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgEnvelope* extents,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMapLegend(MgMap* map,
                                          INT32 width,
                                          INT32 height,
                                          MgColor* backgroundColor,
                                          CREFSTRING format);

    virtual MgFeatureInformation* QueryFeatures( MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* geometry,
                                        INT32 selectionVariant, // Within, Touching, Topmost
                                        INT32 maxFeatures);

private:
    /// Default constructor that does nothing. Its purpose is to keep
    /// the code from causing a compiler error when using gcc.
    MgServerRenderingService();

    // used for tile generation
    MgByteReader* RenderTile(MgMap* map,
                             MgLayerGroup* baseGroup,
                             INT32 scaleIndex,
                             INT32 width,
                             INT32 height,
                             double scale,
                             double mcsMinX,
                             double mcsMaxX,
                             double mcsMinY,
                             double mcsMaxY,
                             CREFSTRING format);

    // helper used by other methods
    MgByteReader* RenderMapInternal(MgMap* map,
                                    MgSelection* selection,
                                    MgReadOnlyLayerCollection* roLayers,
                                    GDRenderer* dr,
                                    INT32 saveWidth,
                                    INT32 saveHeight,
                                    CREFSTRING format,
                                    double scale,
                                    RS_Bounds& b,
                                    bool expandExtents);

    // member data
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgDrawingService> m_svcDrawing;
    Ptr<MgCoordinateSystemFactory> m_pCSFactory;
};

#endif
