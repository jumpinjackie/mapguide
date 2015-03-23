#include "FeatureInfoRenderer.h"
#include "KeyEncode.h"
#include "RSMgFeatureReader.h"


MgdFeatureInfoRenderer::MgdFeatureInfoRenderer(MgdSelection* selection, int maxFeatures, double mapScale, double* point, SE_Renderer* impRenderer)
: m_extents(0.0, 0.0, 1.0, 1.0),
  m_numFeatures(0),
  m_url(L""),
  m_tooltip(L""),
  m_props(NULL),
  m_layerId(L""),
  m_nMaxFeatures(maxFeatures),
  m_mapScale(mapScale),
  m_fcName(L""),
  m_mapInfo(NULL),
  m_layerInfo(NULL),
  m_fcInfo(NULL),
  m_pointTest(false),
  m_impRenderer(impRenderer)
{
    m_selection = SAFE_ADDREF(selection);
    m_keyEncode = new KeyEncode();
    if(point != NULL)
    {
        m_point[0] = point[0];
        m_point[1] = point[1];
        m_needPointTest = true;
    }
    else
        m_needPointTest = false;
}


MgdFeatureInfoRenderer::~MgdFeatureInfoRenderer()
{
    SAFE_RELEASE(m_props);
    SAFE_RELEASE(m_selection);

    delete m_keyEncode;
}


void MgdFeatureInfoRenderer::StartFeature(RS_FeatureReader* feature,
                                       bool initialPass,
                                       const RS_String* tooltip,
                                       const RS_String* url,
                                       const RS_String* /*theme*/,
                                       double /*zOffset*/,
                                       double /*zExtrusion*/,
                                       RS_ElevationType /*zOffsetType*/)
{
    if (!initialPass)
        return;

    m_featurePending = false;

    //add feature ID to the selection set
    if (m_selection)
    {
        //generate base 64 id
        const char* base64 = m_keyEncode->EncodeKey(feature);
        size_t len = base64? strlen(base64) : 0;

        //check how big those things can actually get
        assert(len < 65536);

        //only select if it has a key
        if (len > 0)
        {
            wchar_t* touni = (wchar_t*)alloca(sizeof(wchar_t) * (len+1));

            for (size_t k=0; k<len+1; k++)
                touni[k] = (wchar_t)base64[k];

            //add feature to the selection set
            if(m_pointTest)
            {
                m_id = touni;
                m_featurePending = true;
                if (!m_props && m_fcInfo)
                {
                    if (url)
                        m_test_url = *url;
                    if (tooltip)
                        m_test_tooltip = *tooltip;
                    m_feature = feature;
                }
                return;
            }
            else
                m_selection->Add(m_layerId, m_fcName, (const wchar_t*)touni);
        }
    }

    //if it is the first/only feature, process the properties
    if (!m_props && m_fcInfo)
    {
        if (url)
            m_url = *url;

        if (tooltip)
            m_tooltip = *tooltip;

        m_props = new MgPropertyCollection();

        //for each property in the property mapping, add to the
        //return property collection

        //there are two strings in the list, corresponding to each property
        //the first is the actual name of the property and the second is
        //the display name of the property. Use the actual name to retrieve the
        //value and the display name for the name of the DWF property we store
        for (unsigned i=0; i<m_fcInfo->mappings().size()/2; i++)
        {
            Ptr<MgStringProperty> sp = new MgStringProperty(m_fcInfo->mappings()[2*i+1].c_str()
                , feature->GetAsString(m_fcInfo->mappings()[2*i].c_str()));

            m_props->Add(sp);
        }
    }

    m_numFeatures++;
}


void MgdFeatureInfoRenderer::StartMap(RS_MapUIInfo*    mapInfo,
                                   RS_Bounds&       extents,
                                   double           /*mapScale*/,
                                   double           dpi,
                                   double           metersPerUnit,
                                   CSysTransformer* xformToLL)
{
    // remember the map info
    m_mapInfo = mapInfo;
    m_extents = extents;
    m_dpi = dpi;
    m_metersPerUnit = metersPerUnit;
    m_scale = (dpi * metersPerUnit) / (METERS_PER_INCH * m_mapScale);
    WorldToScreenPoint(m_point[0], m_point[1], m_point[2], m_point[3]);
    InitFontEngine(this);
    if(m_impRenderer)
    {
        m_impRenderer->StartMap(mapInfo, extents, m_mapScale, dpi, metersPerUnit, xformToLL);
        // redirect the FontEngine of m_impRenderer to ours, so that the FontEngine can use our transform
        m_impRenderer->GetRSFontEngine()->InitFontEngine(this);
    }
}


void MgdFeatureInfoRenderer::EndMap()
{
    if(m_impRenderer)
    {
        // restore the FontEngine of m_impRenderer
        m_impRenderer->GetRSFontEngine()->InitFontEngine(m_impRenderer);
        m_impRenderer->EndMap();
    }
    // clear the map info
    m_mapInfo = NULL;
}


void MgdFeatureInfoRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                                     RS_FeatureClassInfo* classInfo)
{
    // remember the layer/feature info
    m_layerInfo = layerInfo;
    m_fcInfo = classInfo;

    m_layerId = layerInfo->guid();
    m_fcName = classInfo->name();
}


void MgdFeatureInfoRenderer::EndLayer()
{
    // clear the layer/feature info
    m_layerInfo = NULL;
    m_fcInfo = NULL;
}


void MgdFeatureInfoRenderer::ProcessMarker(LineBuffer*   lb,
                                        RS_MarkerDef& mdef,
                                        bool          allowOverpost,
                                        RS_Bounds*    bounds)
{
    if(m_pointTest && m_featurePending)
    {
        double scale_factor;
        if (mdef.units() == RS_Units_Device)
            scale_factor = m_mapScale / m_metersPerUnit;
        else
            scale_factor = 1.0 / m_metersPerUnit;

        double cx = mdef.width() * scale_factor;
        double cy = mdef.height() * scale_factor;
        double ix = mdef.insx() * cx;
        double iy = mdef.insy() * cy;
        double angleRad = mdef.rotation() * M_PI180;
        double angleCos = cos(angleRad);
        double angleSin = sin(angleRad);

        for (int i = 0; i < lb->point_count(); i++)
        {
            double ox = lb->x_coord(i), oy = lb->y_coord(i);
            double x0 = ox - ix, x1 = x0 + cx;
            double y0 = oy - iy, y1 = y0 + cy;
            double sx = m_point[0] - ox, sy = m_point[1] - oy;
            double rx = sx * angleCos + sy * angleSin;
            double ry = sy * angleCos - sx * angleSin;
            double dx = rx + ox, dy = ry + oy;

            if(dx > x0 && dx < x1 && dy > y0 && dy < y1)
            {
                SetSelected();
                break;
            }
        }
    }
}


void MgdFeatureInfoRenderer::DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke)
{
    if(m_pointTest && m_featurePending)
    {
        RS_Bounds bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
        for(int i = polyline->point_count() - 1; i >= 0; i--)
        {
            RS_F_Point xfpt(polyline->x_coord(i), polyline->y_coord(i));
            if(xform)
                xform->transform(xfpt.x, xfpt.y, xfpt.x, xfpt.y);
            bounds.add_point(xfpt);
        }
        double x = m_point[2], y = m_point[3];
        if(x > bounds.minx && x < bounds.maxx && y > bounds.miny && y < bounds.maxy)
            SetSelected();
    }
}


void MgdFeatureInfoRenderer::DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill)
{
    // treat polygons as polylines
    SE_LineStroke lineStroke(fill, 0.0, SE_LineCap_Round, SE_LineJoin_Round, 5.0);
    DrawScreenPolyline(polygon, xform, lineStroke);
}


void MgdFeatureInfoRenderer::DrawScreenRaster(unsigned char* data, int length,
                                           RS_ImageFormat format, int native_width, int native_height,
                                           double x, double y, double w, double h, double angleDeg)
{
    if(m_pointTest && m_featurePending)
    {
        double angleRad = angleDeg * M_PI180;
        // TODO: should YPointsUp() be checked here?
        double angleCos = cos(angleRad);
        double angleSin = sin(angleRad);
        double sx = m_point[2] - x, sy = m_point[3] - y;
        double rx = sx * angleCos + sy * angleSin;
        double ry = sy * angleCos - sx * angleSin;
        if (fabs(rx) < w * 0.5 && fabs(ry) < h * 0.5)
            SetSelected();
    }
}


void MgdFeatureInfoRenderer::DrawScreenRaster(unsigned char* data, int length,
                                           RS_ImageFormat format, int native_width, int native_height,
                                           double x, double y, double w, double h, double angleDeg,
                                           double alpha)
{
    DrawScreenRaster(data, length, format, native_width, native_height, x, y, w, h, angleDeg);
}


void MgdFeatureInfoRenderer::DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy,
                                         RS_F_Point* path, int npts, double param_position)
{
    if (m_pointTest && m_featurePending)
    {
        if (path)
        {
            // TODO: can we reach here?
        }
        else
        {
            // block text
            // Check that we have a valid text metrics
            if ( tm.font != NULL )
            {
                RS_Bounds b(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
                for (size_t i = 0; i < tm.line_pos.size(); i++)
                {
                    b.add_point(tm.line_pos[i].ext[0]);
                    b.add_point(tm.line_pos[i].ext[2]);
                }
                double angleRad = tdef.rotation() * M_PI180;
                if (!YPointsUp())
                    angleRad = -angleRad;
                double angleCos = cos(angleRad);
                double angleSin = sin(angleRad);
                double sx = m_point[2] - insx, sy = m_point[3] - insy;
                double rx = sx * angleCos + sy * angleSin;
                double ry = sy * angleCos - sx * angleSin;
                if (rx > b.minx && rx < b.maxx && ry > b.miny && ry < b.maxy)
                    SetSelected();
            }
        }
    }
}


void MgdFeatureInfoRenderer::SetSelected()
{
    m_selection->Add(m_layerId, m_fcName, m_id);
    m_numFeatures++;
    m_featurePending = false;
    if (!m_props && m_fcInfo)
    {
        m_url = m_test_url;
        m_tooltip = m_test_tooltip;

        m_props = new MgPropertyCollection();
        for (unsigned i=0; i<m_fcInfo->mappings().size()/2; i++)
        {
            Ptr<MgStringProperty> sp = new MgStringProperty(m_fcInfo->mappings()[2*i+1].c_str(),
                m_feature->GetAsString(m_fcInfo->mappings()[2*i].c_str()));
            m_props->Add(sp);
        }
    }
}


bool MgdFeatureInfoRenderer::YPointsUp()
{
    return m_impRenderer? m_impRenderer->YPointsUp() : true;
}


void MgdFeatureInfoRenderer::GetWorldToScreenTransform(SE_Matrix& xform)
{
    xform.x0 = m_scale;
    xform.x1 = 0.0;
    xform.x2 = 0.0;
    xform.y0 = 0.0;
    xform.y1 = YPointsUp()? m_scale : -m_scale;
    xform.y2 = 0.0;
}


void MgdFeatureInfoRenderer::WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy)
{
    ox = inx * m_scale;
    oy = (YPointsUp()? iny : -iny) * m_scale;
}


void MgdFeatureInfoRenderer::ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy)
{
    ox = inx / m_scale;
    oy = (YPointsUp()? iny : -iny) / m_scale;
}


// returns number of pixels per millimeter device
double MgdFeatureInfoRenderer::GetScreenUnitsPerMillimeterDevice()
{
    return m_dpi / MILLIMETERS_PER_INCH;
}


// returns number of pixels per millimeter world
double MgdFeatureInfoRenderer::GetScreenUnitsPerMillimeterWorld()
{
    return m_dpi / MILLIMETERS_PER_INCH / m_mapScale;
}


// screen units are pixels
double MgdFeatureInfoRenderer::GetScreenUnitsPerPixel()
{
    return 1.0;
}


RS_FontEngine* MgdFeatureInfoRenderer::GetRSFontEngine()
{
    return this;
}


RS_MapUIInfo* MgdFeatureInfoRenderer::GetMapInfo()
{
    return m_mapInfo;
}


RS_LayerUIInfo* MgdFeatureInfoRenderer::GetLayerInfo()
{
    return m_layerInfo;
}


RS_FeatureClassInfo* MgdFeatureInfoRenderer::GetFeatureClassInfo()
{
    return m_fcInfo;
}


double MgdFeatureInfoRenderer::GetMapScale()
{
    return m_mapScale;
}


double MgdFeatureInfoRenderer::GetDrawingScale()
{
    // compute drawing scale
    // drawing scale is map scale converted to [mapping units] / [pixels]
    double metersPerPixel = METERS_PER_INCH / m_dpi;
    return m_mapScale * metersPerPixel / GetMetersPerUnit();
}


double MgdFeatureInfoRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


double MgdFeatureInfoRenderer::GetDpi()
{
    return m_dpi;
}


RS_Bounds& MgdFeatureInfoRenderer::GetBounds()
{
    return m_extents;
}


bool MgdFeatureInfoRenderer::RequiresClipping()
{
    return false;
}


bool MgdFeatureInfoRenderer::RequiresLabelClipping()
{
    return false;
}


bool MgdFeatureInfoRenderer::SupportsZ()
{
    // Z values in feature geometry are ignored
    return false;
}


bool MgdFeatureInfoRenderer::RequiresCompositeLineStyleSeparation()
{
    return false;
}


void MgdFeatureInfoRenderer::MeasureString(const RS_String& s,
                                        double           height,
                                        const RS_Font*   font,
                                        double           angleRad,
                                        RS_F_Point*      res,       //assumes length equals 4 points
                                        float*           offsets)   //assumes length equals length of string
{
    if (m_impRenderer)
        m_impRenderer->GetRSFontEngine()->MeasureString(s, height, font, angleRad, res, offsets);
}


void MgdFeatureInfoRenderer::DrawString(const RS_String& s,
                                     double           x,
                                     double           y,
                                     double           width,
                                     double           height,
                                     const RS_Font*   font,
                                     RS_Color&        color,
                                     double           angleRad)
{
}


const RS_Font* MgdFeatureInfoRenderer::FindFont(RS_FontDef& def)
{
    return m_impRenderer? m_impRenderer->GetRSFontEngine()->FindFont(def) : NULL;
}
