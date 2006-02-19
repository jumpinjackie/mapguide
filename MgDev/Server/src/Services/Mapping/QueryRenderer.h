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

#ifndef QUERYRENDERER_H
#define QUERYRENDERER_H

#include "Renderer.h"

class MgFeatureReader;
class MgFeatureService;
class LineBuffer;

class MG_SERVER_MAPPING_API QueryRenderer : public Renderer
{

public:
    QueryRenderer(int width, int height, MgFeatureService* svcFeature);
    virtual ~QueryRenderer();

    void InitializeFeatureContext(MgFeatureReader* reader, MgResourceIdentifier* dstFeatures, RS_Units dstUnits);

    ///////////////////////////////////
    // Renderer implementation

    virtual void StartMap      (RS_MapUIInfo* mapInfo,
                                RS_Bounds&    extents,
                                double        mapScale,
                                double        dpi,
                                double        metersPerUnit,
                                CSysTransformer* xformToLL
                               );

    virtual void EndMap           ();

    virtual void StartLayer     (RS_LayerUIInfo*      legendInfo,
                                 RS_FeatureClassInfo* classInfo
                                );

    virtual void EndLayer       ();

    virtual void StartFeature (RS_FeatureReader* feature,
                               const RS_String*  tooltip = NULL,
                               const RS_String*  url = NULL);

    virtual void ProcessPolygon (LineBuffer* lb,
                                 RS_FillStyle& fill);

    virtual void ProcessPolyline(LineBuffer* lb,
                                 RS_LineStroke& lsym);

    virtual void ProcessRaster  (unsigned char* data,
                                 int length,
                                 RS_ImageFormat format,
                                 int width, int height,
                                 RS_Bounds         extents);

    virtual void ProcessMarker( LineBuffer* lb,
                                RS_MarkerDef&     mdef,
                                bool              allowOverpost);


    virtual void ProcessLabel(double x, double y,
                              const RS_String& text, RS_TextDef& tdef);

    virtual void ProcessLabelGroup( RS_LabelInfo*    labels,
                                                    int              nlabels,
                                                    const RS_String& text,
                                                    RS_OverpostType  type,
                                                    bool             exclude,
                                                    LineBuffer*      path);

    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String&    section,
                               const RS_String&    passwd
                               );

    virtual void SetSymbolManager(RS_SymbolManager* manager);

    virtual double GetMapScale();

    virtual RS_Bounds& GetBounds();

    virtual double GetDpi();

    virtual double GetMetersPerUnit();

    virtual bool RequiresClipping();

    virtual bool UseLocalOverposting();

    int GetNumFeaturesProcessed();

private:

    MgByteReader* GetGeometry(LineBuffer* srclb);

    void TransferFeature(RS_FeatureReader* src, MgByteReader* geom);

    inline double _TX(double x);
    inline double _TY(double y);
    void _TransformPointsInPlace(LineBuffer* lb);

    int m_width;
    int m_height;

    RS_Bounds m_extents;
    double m_metersPerUnit;
    double m_dpi;
    double m_mapScale;
    double m_scale;
    double m_offsetX;
    double m_offsetY;
    double m_drawingScale;
    RS_Units m_units;

    MgResourceIdentifier* m_dstResId;
    MgClassDefinition* m_fclass;
    MgFeatureService* m_svcFeature;

    RS_String m_coordSys;

    int m_numFeatures;

    RS_FeatureReader* m_feature;
};

#endif
