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

#include "stdafx.h"

#include "KmlRenderer.h"
#include "LineStyle.h"
#include "SLDSymbols.h"
#include "UnicodeString.h"
#include "PolygonUtils.h"
#include "SE_SymbolDefProxies.h"

#include "RenderUtil.h"

const double MIN_KML_THICKNESS = 0.00001;
const double ELEV_FACTOR = 0.1;

//default constructor
KmlRenderer::KmlRenderer(KmlContent* kmlContent,
                         RS_Bounds& extents,
                         double scale,
                         double dpi,
                         double metersPerUnit,
                         int drawOrder) :
    m_mainContent(kmlContent),
    m_kmlContent(kmlContent),
    m_styleContent(NULL),
    m_featureCount(0),
    m_mapInfo(NULL),
    m_layerInfo(NULL),
    m_featureClassInfo(NULL),
    m_mapScale(scale),
    m_styleId(0),
    m_extents(extents),
    m_drawOrder(drawOrder),
    m_elevation(0.0),
    m_extrude(false),
    m_elevType(RS_ElevationType_RelativeToGround),
    m_metersPerUnit(metersPerUnit)
{
    m_kmlContent = m_mainContent;
    m_pixelSize = METERS_PER_INCH / dpi;
}


KmlRenderer::~KmlRenderer()
{
    ClearThemes();
    ClearStyles();
}


void KmlRenderer::StartMap(RS_MapUIInfo* mapInfo,
                           RS_Bounds&    /*extents*/,
                           double        /*mapScale*/,
                           double        /*dpi*/,
                           double        /*metersPerUnit*/,
                           CSysTransformer* /*xformToLL*/)
{
    m_featureCount = 0;

    // remember the map info
    m_mapInfo = mapInfo;
}


void KmlRenderer::EndMap()
{
    // clear the map info
    m_mapInfo = NULL;
}


void KmlRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                             RS_FeatureClassInfo* classInfo)
{
    m_featureCount = 0;
    m_layerInfo = layerInfo;
    m_featureClassInfo = classInfo;
    ClearThemes();
    ClearStyles();
}


void KmlRenderer::EndLayer()
{
    //close the very last feature
    if (m_featureCount > 0)
    {
        //end last feature
        m_kmlContent->WriteString("</Placemark>");
    }
    m_kmlContent = m_mainContent;

    //write out the features for each theme
    for (ThemeMap::iterator iter = m_themeMap.begin(); iter != m_themeMap.end(); ++iter)
    {
        m_kmlContent->WriteString("<Folder>");
        m_kmlContent->WriteString("<name><![CDATA[", false);
        m_kmlContent->WriteString((*iter).first, false);
        m_kmlContent->WriteString("]]></name>");
        m_kmlContent->WriteString((*iter).second->GetString(), false);
        m_kmlContent->WriteString("</Folder>");
    }

    //write out the styles
    if (m_styleContent != NULL)
    {
        m_kmlContent->WriteString(m_styleContent->GetString());
    }
}


void KmlRenderer::StartFeature(RS_FeatureReader* /*feature*/,
                               bool /*initialPass*/,
                               const RS_String* tooltip,
                               const RS_String* url,
                               const RS_String* theme,
                               double zOffset,
                               double zExtrusion,
                               RS_ElevationType zOffsetType)
{
    if (m_featureCount > 0)
    {
        //end last feature
        m_kmlContent->WriteString("</Placemark>");
    }
    if (theme == NULL || theme->empty())
    {
        m_kmlContent = m_mainContent;
    }
    else
    {
        ThemeMap::iterator iter = m_themeMap.find(*theme);
        if (iter != m_themeMap.end())
        {
            m_kmlContent = (*iter).second;
        }
        else
        {
            m_kmlContent = new KmlContent();
            m_themeMap[*theme] = m_kmlContent;
        }
    }
    m_kmlContent->WriteString("<Placemark>");
    if ((tooltip != NULL && tooltip->length() > 0) ||
        (url != NULL && url->length() > 0))
    {
        m_kmlContent->WriteString("<description>");
        m_kmlContent->WriteString("<![CDATA[", false);
        if (tooltip != NULL && tooltip->length() > 0)
        {
            //replace line breaks
            RS_String newTT(*tooltip);
            RS_String::size_type lb = newTT.find(L"\\n");
            while (lb != RS_String::npos)
            {
                newTT = newTT.replace(lb, 2, L"<br/>");
                lb = newTT.find(L"\\n", lb);
            }
            m_kmlContent->WriteString(newTT, false);
            m_kmlContent->WriteString("<br/>", false);
        }
        if (url != NULL && url->length() > 0)
        {
            m_kmlContent->WriteString(*url, false);
        }
        m_kmlContent->WriteString("]]>");
        m_kmlContent->WriteString("</description>");
    }
    m_featureCount++;

    // Elevation / Extrusion settings
    m_elevation = zExtrusion + zOffset;
    m_extrude = (zExtrusion > 0);
    m_elevType = zOffsetType;
}


void KmlRenderer::WriteElevationSettings()
{
    if (m_elevation == 0)
    {
        m_kmlContent->WriteString("<altitudeMode>clampToGround</altitudeMode>");
        m_kmlContent->WriteString("<tessellate>1</tessellate>");
    }
    else
    {
        if (m_extrude)
        {
            m_kmlContent->WriteString("<extrude>1</extrude>");
        }
        switch (m_elevType)
        {
        case RS_ElevationType_RelativeToGround:
            {
                m_kmlContent->WriteString("<altitudeMode>relativeToGround</altitudeMode>");
                break;
            }
        case RS_ElevationType_Absolute:
            {
                m_kmlContent->WriteString("<altitudeMode>absolute</altitudeMode>");
                break;
            }
        }
    }
}


void KmlRenderer::ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill)
{
    //write style
    WriteStyle(fill);

    if (lb != NULL)
    {
        if (lb->cntr_count() == 1)
        {
            m_kmlContent->WriteString("<Polygon>");
            WriteElevationSettings();
            m_kmlContent->WriteString("<outerBoundaryIs>");
            WriteLinearRing(lb);
            m_kmlContent->WriteString("</outerBoundaryIs>");
            m_kmlContent->WriteString("</Polygon>");
        }
        else
        {
            m_kmlContent->WriteString("<MultiGeometry>");
            WriteElevationSettings();
            PolygonUtils::SORTEDRINGS rings;
            PolygonUtils::DetermineInteriorAndExteriorPolygons(lb, rings);
            for (PolygonUtils::SORTEDRINGS::iterator sIter = rings.begin(); sIter != rings.end(); ++sIter)
            {
                RingData* pRingData = sIter->second;

                // only create polygons for outer rings
                if (pRingData->m_type != RingData::Outer)
                    continue;

                if (pRingData->ringPointCount() > 0)
                {
                    // write the outer ring
                    m_kmlContent->WriteString("<Polygon>");
                    WriteElevationSettings();
                    m_kmlContent->WriteString("<outerBoundaryIs>");
                    WriteLinearRing(pRingData->m_lineBuffer, pRingData->m_cntr);
                    m_kmlContent->WriteString("</outerBoundaryIs>");

                    // write the inner rings, if any
                    RingData* pChild = pRingData->m_child;
                    while (pChild != NULL)
                    {
                        if (pChild->ringPointCount() > 0)
                        {
                            m_kmlContent->WriteString("<innerBoundaryIs>");
                            WriteLinearRing(pChild->m_lineBuffer, pChild->m_cntr);
                            m_kmlContent->WriteString("</innerBoundaryIs>");
                        }
                        pChild = pChild->m_child;
                    }
                    m_kmlContent->WriteString("</Polygon>");
                }
            }
            PolygonUtils::Cleanup(rings);
            m_kmlContent->WriteString("</MultiGeometry>");
        }
    }
}


void KmlRenderer::WriteLinearRing(LineBuffer* plb, int contour)
{
    m_kmlContent->WriteString("<LinearRing>");
    WriteContourCoordinates(plb, contour);
    m_kmlContent->WriteString("</LinearRing>");
}


void KmlRenderer::WriteLinearRing(LineBuffer* plb)
{
    m_kmlContent->WriteString("<LinearRing>");
    WriteCoordinates(plb);
    m_kmlContent->WriteString("</LinearRing>");
}


void KmlRenderer::WriteCoordinates(LineBuffer* plb)
{
    char buffer[256];
    m_kmlContent->WriteString("<coordinates>");
    int numPoints(plb->point_count());

    for (int i = 0; i < numPoints; i ++)
    {
        sprintf(buffer, "%f,%f,%f%s", plb->x_coord(i), plb->y_coord(i), m_elevation, (i < numPoints - 1)? " " : "");
        m_kmlContent->WriteString(buffer);
    }
    m_kmlContent->WriteString("</coordinates>");
}


void KmlRenderer::WriteContourCoordinates(LineBuffer* plb, int cntr)
{
    char buffer[256];
    m_kmlContent->WriteString("<coordinates>");
    int numPoints(plb->cntr_size(cntr));
    int pointOffset = plb->contour_start_point(cntr);
    for (int i = 0; i < numPoints; i ++)
    {
        sprintf(buffer, "%f,%f,%f%s", plb->x_coord(pointOffset), plb->y_coord(pointOffset), m_elevation, (i < numPoints - 1)? " " : "");
        m_kmlContent->WriteString(buffer);
        pointOffset++;
    }
    m_kmlContent->WriteString("</coordinates>");
}


void KmlRenderer::ProcessPolyline(LineBuffer* srclb, RS_LineStroke& lsym)
{
    //write style
    WriteStyle(lsym);

    m_kmlContent->WriteString("<MultiGeometry>");
    WriteElevationSettings();
    int numCntrs = srclb->cntr_count();
    for (int i = 0; i < numCntrs; i++)
    {
        m_kmlContent->WriteString("<LineString>");
        WriteContourCoordinates(srclb, i);
        m_kmlContent->WriteString("</LineString>");
    }
    m_kmlContent->WriteString("</MultiGeometry>");
}


void KmlRenderer::ProcessRaster(unsigned char* /*data*/,
                                int /*length*/,
                                RS_ImageFormat /*format*/,
                                int /*width*/,
                                int /*height*/,
                                RS_Bounds& /*extents*/,
                                TransformMesh* /*xformMesh*/)
{
}


void KmlRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* /*bounds*/)
{
    for (int i = 0; i < srclb->point_count(); i++)
    {
        ProcessOneMarker(srclb->x_coord(i), srclb->y_coord(i), mdef, allowOverpost);
    }
}


void KmlRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool /*allowOverpost*/)
{
    //write style
    RS_UIGraphic& uig = m_layerInfo->graphic();
    unsigned char* data = uig.data();
    if(NULL != data)
    {
        wchar_t * legendImage = new wchar_t[4096];
        mbstowcs(legendImage,(const char*)data,4096);
        WriteStyle(0.5,legendImage);
    }

    char buffer[256];
    m_kmlContent->WriteString("<name><![CDATA[", false);
    m_kmlContent->WriteString(mdef.name(), false);
    m_kmlContent->WriteString("]]></name>");
    m_kmlContent->WriteString("<Point>");
    WriteElevationSettings();
    m_kmlContent->WriteString("<coordinates>");
    sprintf(buffer, "%f,%f,%f", x, y, m_elevation);
    m_kmlContent->WriteString(buffer);
    m_kmlContent->WriteString("</coordinates>");
    m_kmlContent->WriteString("</Point>");
}


void KmlRenderer::ProcessLabelGroup(RS_LabelInfo*    /*labels*/,
                                    int              /*nlabels*/,
                                    const RS_String& text,
                                    RS_OverpostType  /*type*/,
                                    bool             /*exclude*/,
                                    LineBuffer*      /*path*/,
                                    double           /*scaleLimit*/)
{
//    char buffer[256];
    m_kmlContent->WriteString("<name><![CDATA[", false);
    m_kmlContent->WriteString(text, false);
    m_kmlContent->WriteString("]]></name>");
/*    m_kmlContent->WriteString("<MultiGeometry>");
    for (int i = 0; i < nlabels; i++)
    {
        m_kmlContent->WriteString("<Point>");
        m_kmlContent->WriteString("<coordinates>");
        sprintf(buffer, "%f,%f,0.00000", labels[i].x(), labels[i].y());
        m_kmlContent->WriteString(buffer);
        m_kmlContent->WriteString("</coordinates>");
        m_kmlContent->WriteString("</Point>");
    }
    m_kmlContent->WriteString("</MultiGeometry>");*/
}


void KmlRenderer::SetSymbolManager(RS_SymbolManager* /*manager*/)
{
}


RS_MapUIInfo* KmlRenderer::GetMapInfo()
{
    return m_mapInfo;
}


RS_LayerUIInfo* KmlRenderer::GetLayerInfo()
{
    return m_layerInfo;
}


RS_FeatureClassInfo* KmlRenderer::GetFeatureClassInfo()
{
    return m_featureClassInfo;
}


double KmlRenderer::GetMapScale()
{
    return m_mapScale;
}


double KmlRenderer::GetDrawingScale()
{
    //compute drawing scale
    //drawing scale is map scale converted to [mapping units] / [pixels]
    return m_mapScale * m_pixelSize / m_metersPerUnit;
}


double KmlRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


double KmlRenderer::GetDpi()
{
    return STANDARD_DISPLAY_DPI;
}


RS_Bounds& KmlRenderer::GetBounds()
{
    return m_extents;
}


bool KmlRenderer::RequiresClipping()
{
    return false;
}


bool KmlRenderer::RequiresLabelClipping()
{
    return false;
}


bool KmlRenderer::SupportsZ()
{
    // although KmlRenderer supports elevation, it ignores Z values
    // in feature geometry
    return false;
}


//Inserts the contents of a given DWF input stream
//into the current output W2D. The given coord sys
//transformation is applied and geometry will be clipped
//to the RS_Bounds context of the DWFRenderer
void KmlRenderer::AddDWFContent(RS_InputStream*   /*in*/,
                               CSysTransformer*  /*xformer*/,
                               const RS_String&  /*section*/,
                               const RS_String&  /*passwd*/,
                               const RS_String&  /*w2dfilter*/)
{
}


void KmlRenderer::ClearThemes()
{
    for (ThemeMap::iterator iter = m_themeMap.begin(); iter != m_themeMap.end(); ++iter)
    {
        delete (*iter).second;
    }
    m_themeMap.clear();
}


void KmlRenderer::ClearStyles()
{
    delete m_styleContent;
    m_styleContent = NULL;
}


void KmlRenderer::WriteStyle(RS_FillStyle& fill)
{
    if (m_styleContent == NULL)
        m_styleContent = new KmlContent();

    char buffer[256];
    int thisStyleId = 0;
    KmlPolyStyle key(fill.outline().color().abgr(), _MeterToPixels(fill.outline().units(), fill.outline().width()), fill.color().abgr());
    KmlPolyStyleIdMap::iterator iter = m_polyStyleMap.find(key);
    if (iter != m_polyStyleMap.end())
    {
        thisStyleId = (*iter).second;
    }
    else
    {
        thisStyleId = m_polyStyleMap[key] = m_styleId;

        //write the style definitions into the style storage
        sprintf(buffer, "<Style id=\"%d\">", m_styleId++);
        m_styleContent->WriteString(buffer);

        //outline
        m_styleContent->WriteString("<LineStyle>",false);
        m_styleContent->WriteString("<color>",false);
        RS_Color color = fill.outline().color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer,false);
        m_styleContent->WriteString("</color>",false);
        m_styleContent->WriteString("<width>",false);
        sprintf(buffer, "%f", key.m_lineWidth);
        m_styleContent->WriteString(buffer,false);
        m_styleContent->WriteString("</width>",false);
        m_styleContent->WriteString("</LineStyle>");

        //fill
        m_styleContent->WriteString("<PolyStyle>",false);
        m_styleContent->WriteString("<color>",false);
        color = fill.color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer,false);
        m_styleContent->WriteString("</color>",false);
        m_styleContent->WriteString("</PolyStyle>");

        //icon
//        m_styleContent->WriteString("<IconStyle><Icon>");
//        m_styleContent->WriteString("<href>");
//        m_styleContent->WriteString("http://callt83-le:8008/mapguide/mapagent/mapagent.fcgi?OPERATION=GetLegendImage&amp;LAYERDEFINITION=Library://Samples/Sheboygan/Layers/Parcels.LayerDefinition&amp;WIDTH=32&amp;HEIGHT=32&amp;SCALE=4950&amp;FORMAT=PNG&amp;THEMECATEGORY=3&amp;TYPE=-1&amp;VERSION=1");
//        m_styleContent->WriteString("http://callt83-le:8008/mapguide/mapagent/icon.png");
//        m_styleContent->WriteString("</href>");
//        m_styleContent->WriteString("</Icon></IconStyle>");

        m_styleContent->WriteString("</Style>");
    }

    //write a style reference into the main document
    sprintf(buffer, "<styleUrl>#%d</styleUrl>", thisStyleId);
    m_kmlContent->WriteString(buffer);
}


void KmlRenderer::WriteStyle(RS_LineStroke& lsym)
{
    if (m_styleContent == NULL)
    {
        m_styleContent = new KmlContent();
    }

    char buffer[256];
    int thisStyleId = 0;
    KmlLineStyle key(lsym.color().abgr(), _MeterToPixels(lsym.units(), lsym.width()));
    KmlLineStyleIdMap::iterator iter = m_lineStyleMap.find(key);
    if (iter != m_lineStyleMap.end())
    {
        thisStyleId = (*iter).second;
    }
    else
    {
        thisStyleId = m_lineStyleMap[key] = m_styleId;

        //write the style definitions into the style storage
        sprintf(buffer, "<Style id=\"%d\">", m_styleId++);
        m_styleContent->WriteString(buffer);

        //line style
        m_styleContent->WriteString("<LineStyle>",false);
        m_styleContent->WriteString("<color>",false);
        RS_Color color = lsym.color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer,false);
        m_styleContent->WriteString("</color>",false);
        m_styleContent->WriteString("<width>",false);
        sprintf(buffer, "%f", key.m_width);
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</width>",false);
        m_styleContent->WriteString("</LineStyle>");

        m_styleContent->WriteString("</Style>");
    }

    //write a style reference into the main document
    sprintf(buffer, "<styleUrl>#%d</styleUrl>", thisStyleId);
    m_kmlContent->WriteString(buffer);
}

void KmlRenderer::WriteStyle(double scale, const std::wstring& href)
{
    if (m_styleContent == NULL)
    {
        m_styleContent = new KmlContent();
    }

    char buffer[256];
    int thisStyleId = 0;
    KmlIconStyle key(1, href);
    KmlIconStyleIdMap::iterator iter = m_iconStyleMap.find(key);
    if (iter != m_iconStyleMap.end())
    {
        thisStyleId = (*iter).second;
    }
    else
    {
        thisStyleId = m_iconStyleMap[key] = m_styleId;

        //write the style definitions into the style storage
        sprintf(buffer, "<Style id=\"%d\">", m_styleId++);
        m_styleContent->WriteString(buffer);

        //line style
        m_styleContent->WriteString("<IconStyle>",false);
        m_styleContent->WriteString("<scale>",false);
        sprintf(buffer, "%f", scale);
        m_styleContent->WriteString(buffer,false);
        m_styleContent->WriteString("</scale>",false);
        m_styleContent->WriteString("<Icon>",false);
        m_styleContent->WriteString("<href>",false);
        m_styleContent->WriteString(href,false);
        m_styleContent->WriteString("</href>",false);
        m_styleContent->WriteString("</Icon>",false);
        m_styleContent->WriteString("</IconStyle>");

        m_styleContent->WriteString("</Style>");
    }

    //write a style reference into the main document
    sprintf(buffer, "<styleUrl>#%d</styleUrl>", thisStyleId);
    m_kmlContent->WriteString(buffer);
}

double KmlRenderer::_MeterToPixels(RS_Units unit, double number)
{
    double scale_factor;

    if (unit == RS_Units_Device) // in meters, fixed size
    {
        scale_factor = 1.0 / m_pixelSize;
    }
    else
    {
        scale_factor = 1.0 / m_mapScale / m_pixelSize;
    }

    double result = number * scale_factor;
    //We cannot apply a thickness of 0 as this will cause Google Earth to draw nothing
    //so if the thickness computes to 0, replace it with a very small value that's greater
    //than 0
    return result == 0.0 ? MIN_KML_THICKNESS : result;
}


////////////////////////////////////////////////////////////////////////////////
//
// SE_Renderer implementation
//
////////////////////////////////////////////////////////////////////////////////


void KmlRenderer::DrawScreenPolyline(LineBuffer*          /*geom*/,
                                     const SE_Matrix*     /*xform*/,
                                     const SE_LineStroke& /*lineStroke*/)
{
}


void KmlRenderer::DrawScreenPolygon(LineBuffer*      /*geom*/,
                                    const SE_Matrix* /*xform*/,
                                    unsigned int     /*fill*/)
{
}


void KmlRenderer::DrawScreenRaster(unsigned char* /*data*/,
                                   int            /*length*/,
                                   RS_ImageFormat /*format*/,
                                   int            /*native_width*/,
                                   int            /*native_height*/,
                                   double         /*x*/,
                                   double         /*y*/,
                                   double         /*w*/,
                                   double         /*h*/,
                                   double         /*angleDeg*/)
{
}


void KmlRenderer::DrawScreenRaster(unsigned char* data, int length,
                                   RS_ImageFormat format, int native_width, int native_height,
                                   double x, double y, double w, double h, double angleDeg,
                                   double alpha)
{
    RenderUtil::DrawScreenRaster(this, data, length, format, native_width, native_height,
                                 x, y, w, h, angleDeg, alpha);
}


void KmlRenderer::DrawScreenText(const RS_TextMetrics&  /*tm*/,
                                 RS_TextDef&      /*tdef*/,
                                 double           /*insx*/,
                                 double           /*insy*/,
                                 RS_F_Point*      /*path*/,
                                 int              /*npts*/,
                                 double           /*param_position*/)
{
}


bool KmlRenderer::YPointsUp()
{
    return true;
}


void KmlRenderer::GetWorldToScreenTransform(SE_Matrix& /*xform*/)
{
}


void KmlRenderer::WorldToScreenPoint(double& /*inx*/, double& /*iny*/, double& /*ox*/, double& /*oy*/)
{
}


void KmlRenderer::ScreenToWorldPoint(double& /*inx*/, double& /*iny*/, double& /*ox*/, double& /*oy*/)
{
}


// returns number of pixels per millimeter device
double KmlRenderer::GetScreenUnitsPerMillimeterDevice()
{
    return STANDARD_DISPLAY_DPI / MILLIMETERS_PER_INCH; // wrong but not currently used
}


// returns number of pixels per millimeter world
double KmlRenderer::GetScreenUnitsPerMillimeterWorld()
{
    return STANDARD_DISPLAY_DPI / MILLIMETERS_PER_INCH / m_mapScale; // wrong but not currently used
}


// screen units are pixels
double KmlRenderer::GetScreenUnitsPerPixel()
{
    return 1.0;
}


RS_FontEngine* KmlRenderer::GetRSFontEngine()
{
    return NULL;
}


void KmlRenderer::ProcessSELabelGroup(SE_LabelInfo*   /*labels*/,
                                      int             /*nlabels*/,
                                      RS_OverpostType /*type*/,
                                      bool            /*exclude*/,
                                      LineBuffer*     /*path*/)
{
}


void KmlRenderer::AddExclusionRegion(RS_F_Point* /*fpts*/, int /*npts*/)
{
}


/////////////////////////////////////////////////////////////////////////////
// TODO: If we want true composite styles generated by the SE_Renderer
// these 3 functions need to be removed and instead the above DrawScreen*
// functions should be implemented. The risk of fully styling all features
// in the kml renderer is the insanely huge resulting KML files. So the solution
// right now is to just pipe all features that come from the SE_Renderer
// directly into the old Renderer APIs using arbitrary styles based on
// information obtained from cursory examination of the SE composite style.


void KmlRenderer::ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style, RS_Bounds* bounds)
{
    RS_FillStyle fs;
    RS_MarkerDef mdef(RS_MarkerType_Marker, 1.0, 1.0, 0.5, 0.5, 0.0, RS_Units_Device, SLDType_Square, L"", L"", fs);

    ProcessMarker(ctx->geometry, mdef, !style->addToExclusionRegion, bounds);
}


void KmlRenderer::ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
{
    RS_Color black(0, 0, 0, 255);
    RS_LineStroke ls(black, 0.0, L"Solid", RS_Units_Device);

    // try to get some line style information from the SE symbol
    if (style->symbol.size())
    {
        for (size_t i=0; i<style->symbol.size(); i++)
        {
            SE_RenderPrimitive* rp = style->symbol[i];

            if (rp->type == SE_RenderPrimitive_Polyline)
            {
                ls.color() = RS_Color::FromARGB(((SE_RenderPolyline*)rp)->lineStroke.color);
                ls.width() = ((SE_RenderPolyline*)rp)->lineStroke.weight / GetScreenUnitsPerMillimeterDevice() * 0.001; //convert from screen units to meters
            }
        }
    }

    // forward the feature to the regular ProcessPolyline API
    ProcessPolyline(ctx->geometry, ls);
}


void KmlRenderer::ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
{
    //not implemented upstream yet, but let's do something anyway

    RS_FillStyle fs;
    fs.color() = RS_Color(0, 0, 0, 255);

    //try to get some fill style information from the SE symbol
    if (style->symbol.size())
    {
        for (size_t i=0; i<style->symbol.size(); i++)
        {
            SE_RenderPrimitive* rp = style->symbol[i];

            if (rp->type == SE_RenderPrimitive_Polygon)
            {
                fs.color() = RS_Color::FromARGB(((SE_RenderPolygon*)rp)->fill);
                fs.outline().color() = RS_Color::FromARGB(((SE_RenderPolygon*)rp)->lineStroke.color);
            }
        }
    }

    ProcessPolygon(ctx->geometry, fs);
}
