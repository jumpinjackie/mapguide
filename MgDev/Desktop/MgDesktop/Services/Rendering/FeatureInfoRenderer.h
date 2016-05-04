#ifndef DESKTOP_FEATURE_INFO_RENDERER_H
#define DESKTOP_FEATURE_INFO_RENDERER_H

#include "SE_Renderer.h"
#include "RS_FontEngine.h"

class MgPropertyCollection;
class LineBuffer;
class KeyEncode;
class TransformMesh;

///\cond INTERNAL
//A simple Renderer implementation for use by the QueryFeatures
//RenderingService API. Accumulates feature IDs and also attributes
//for one feature (case where a tooltip or attribute info needs to
//be displayed in the AJAX viewer)
class MG_DESKTOP_API MgdFeatureInfoRenderer : public SE_Renderer, public RS_FontEngine
{
public:
    MgdFeatureInfoRenderer(MgdSelection* selection,
                        int maxFeatures,
                        double mapScale,
                        double* point = NULL,
                        SE_Renderer* impRenderer = NULL);
    virtual ~MgdFeatureInfoRenderer();

    ///////////////////////////////////
    // Renderer implementation

    virtual void StartMap(RS_MapUIInfo*    mapInfo,
                          RS_Bounds&       extents,
                          double           mapScale,
                          double           dpi,
                          double           metersPerUnit,
                          CSysTransformer* xformToLL);

    virtual void EndMap();

    virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                            RS_FeatureClassInfo* classInfo);

    virtual void EndLayer();

    virtual void ProcessPolygon(LineBuffer*   lb,
                                RS_FillStyle& fill)
    {}

    virtual void ProcessPolyline(LineBuffer*    lb,
                                 RS_LineStroke& lsym)
    {}

    virtual void ProcessRaster(unsigned char* data,
                               int            length,
                               RS_ImageFormat format,
                               int            width,
                               int            height,
                               RS_Bounds&     extents,
                               TransformMesh* xformMesh = NULL)
    {}

    virtual void ProcessMarker(LineBuffer*   lb,
                               RS_MarkerDef& mdef,
                               bool          allowOverpost,
                               RS_Bounds*    bounds = NULL);

    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit)
    {}

    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String& section,
                               const RS_String& passwd,
                               const RS_String& filter)
    {}

    virtual void SetSymbolManager(RS_SymbolManager* manager)
    {}

    virtual void StartFeature(RS_FeatureReader* feature,
                              bool              initialPass,
                              const RS_String*  tooltip = NULL,
                              const RS_String*  url = NULL,
                              const RS_String*  theme = NULL,
                              double            zOffset = 0.0,
                              double            zExtrusion = 0.0,
                              RS_ElevationType  zOffsetType = RS_ElevationType_RelativeToGround);

    virtual RS_MapUIInfo* GetMapInfo();

    virtual RS_LayerUIInfo* GetLayerInfo();

    virtual RS_FeatureClassInfo* GetFeatureClassInfo();

    virtual double GetMapScale();

    virtual double GetDrawingScale();

    virtual double GetMetersPerUnit();

    virtual double GetDpi();

    virtual RS_Bounds& GetBounds();

    virtual bool RequiresClipping();

    virtual bool RequiresLabelClipping();

    virtual bool SupportsZ();

    virtual bool RequiresCompositeLineStyleSeparation();

    ///////////////////////////////////
    // SE_Renderer implementation

    virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
    {}

    virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
    {}

    virtual void DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& xform, double angleRad)
    {}

    virtual void DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke);
    virtual void DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill);
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg);
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg,
                                  double alpha);
    virtual void DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy,
                                RS_F_Point* path, int npts, double param_position);

    virtual bool YPointsUp();
    virtual void GetWorldToScreenTransform(SE_Matrix& xform);
    virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy);
    virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy);

    virtual double GetScreenUnitsPerMillimeterDevice();
    virtual double GetScreenUnitsPerMillimeterWorld();
    virtual double GetScreenUnitsPerPixel();

    virtual RS_FontEngine* GetRSFontEngine();

    virtual void ProcessSELabelGroup(SE_LabelInfo*   labels,
                                     int             nlabels,
                                     RS_OverpostType type,
                                     bool            exclude,
                                     LineBuffer*     path = NULL)
    {}

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts)
    {}

    ////////////////////////////////////////////////
    // RS_FontEngine implementation

    virtual void MeasureString(const RS_String& s,
                               double           height,
                               const RS_Font*   font,
                               double           angleRad,
                               RS_F_Point*      res,
                               float*           offsets);

    virtual void DrawString(const RS_String& s,
                            double           x,
                            double           y,
                            double           width,
                            double           height,
                            const RS_Font*   font,
                            RS_Color&        color,
                            double           angleRad);

    virtual const RS_Font* FindFont(RS_FontDef& def);

    ///////////////////////////////////////////////////////////////////////
    // MgdFeatureInfoRenderer functions

    int GetNumFeaturesProcessed()
    {
        return m_numFeatures;
    }

    int GetNumMaxFeatures()
    {
        return m_nMaxFeatures;
    }

    RS_String GetUrl()
    {
        return m_url;
    }

    RS_String GetTooltip()
    {
        return m_tooltip;
    }

    MgPropertyCollection* GetProperties()
    {
        return SAFE_ADDREF(m_props);
    }

    bool NeedPointTest()
    {
        return m_needPointTest;
    }

    void PointTest(bool bEnable)
    {
        m_pointTest = bEnable;
    }

    void SetSelected();

protected:
    //common to FeaturePropRenderer and MgdFeatureInfoRenderer
    RS_String m_layerId;
    RS_String m_fcName;

    int m_numFeatures;
    int m_nMaxFeatures;

    KeyEncode* m_keyEncode;

    // map/layer/feature info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_fcInfo;

    MgdSelection* m_selection;

    RS_Bounds m_extents;
    double m_mapScale;
    double m_metersPerUnit;
    double m_dpi;
    double m_scale;

private:
    //specific to MgdFeatureInfoRenderer
    RS_String m_url;
    RS_String m_tooltip;
    MgPropertyCollection* m_props;

    SE_Renderer* m_impRenderer;
    double m_point[4];
    bool m_needPointTest;
    bool m_pointTest;
    bool m_featurePending;
    RS_String m_id;
    RS_String m_test_url;
    RS_String m_test_tooltip;
    RS_FeatureReader* m_feature;
};
///\endcond
#endif