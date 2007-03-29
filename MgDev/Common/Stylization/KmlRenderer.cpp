//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

//using this in contructor
#pragma warning(disable:4355)

const double ELEV_FACTOR = 0.1;

//default constructor
KmlRenderer::KmlRenderer(KmlContent* kmlContent, RS_Bounds& extents,
        double scale, double dpi, double metersPerUnit, int drawOrder) :
    m_mainContent(kmlContent),
    m_kmlContent(kmlContent),
    m_styleContent(NULL),
    m_featureCount(0),
    m_layerInfo(NULL),
    m_featureClassInfo(NULL),
    m_mapScale(scale),
    m_styleId(0),
    m_extents(extents),
    m_drawOrder(drawOrder),
    m_elevation(0),
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


void KmlRenderer::StartMap(RS_MapUIInfo* /*mapInfo*/,
                          RS_Bounds&    /*extents*/,
                          double        /*mapScale*/,
                          double        /*dpi*/,
                          double        /*metersPerUnit*/,
                          CSysTransformer* /*xformToLL*/)
{
    m_featureCount = 0;
}


void KmlRenderer::EndMap()
{
}


void KmlRenderer::StartLayer(RS_LayerUIInfo*     legendInfo,
                            RS_FeatureClassInfo* classInfo)
{
    m_featureCount = 0;
    m_layerInfo = legendInfo;
    m_featureClassInfo = classInfo;
    ClearThemes();
    ClearStyles();
}


void KmlRenderer::EndLayer()
{
    //close the very last feature
    if(m_featureCount > 0)
    {
        //end last feature
        m_kmlContent->WriteString("</Placemark>");
    }
    m_kmlContent = m_mainContent;

    //write out the features for each theme
    for(ThemeMap::iterator iter = m_themeMap.begin(); iter != m_themeMap.end(); iter++)
    {
        m_kmlContent->WriteString("<Folder>");
        m_kmlContent->WriteString("<name><![CDATA[", false);
        m_kmlContent->WriteString((*iter).first, false);
        m_kmlContent->WriteString("]]></name>");
        m_kmlContent->WriteString((*iter).second->GetString(), false);
        m_kmlContent->WriteString("</Folder>");
    }

    //write out the styles
    if(m_styleContent != NULL)
    {
        m_kmlContent->WriteString(m_styleContent->GetString());
    }
}


void KmlRenderer::StartFeature(RS_FeatureReader* /*feature*/,
                               const RS_String* tooltip,
                               const RS_String* url,
                               const RS_String* theme,
                               double zOffset,
                               double zExtrusion,
                               RS_ElevationType zOffsetType)
{
    if(m_featureCount > 0)
    {
        //end last feature
        m_kmlContent->WriteString("</Placemark>");
    }
    if(theme == NULL || theme->empty())
    {
        m_kmlContent = m_mainContent;
    }
    else
    {
        ThemeMap::iterator iter = m_themeMap.find(*theme);
        if(iter != m_themeMap.end())
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
    if((tooltip != NULL && tooltip->length() > 0) ||
        (url != NULL && url->length() > 0))
    {
        m_kmlContent->WriteString("<description>");
        m_kmlContent->WriteString("<![CDATA[", false);
        if(tooltip != NULL && tooltip->length() > 0)
        {
            //replace line breaks
            RS_String newTT(*tooltip);
            RS_String::size_type lb = newTT.find(L"\\n");
            while(lb != RS_String::npos)
            {
                newTT = newTT.replace(lb, 2, L"<br/>");
                lb = newTT.find(L"\\n", lb);
            }
            m_kmlContent->WriteString(newTT, false);
            m_kmlContent->WriteString("<br/>", false);
        }
        if(url != NULL && url->length() > 0)
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
    if(m_elevation == 0)
    {
        m_kmlContent->WriteString("<altitudeMode>clampToGround</altitudeMode>");
        m_kmlContent->WriteString("<tesselate>1</tesselate>");
    }
    else
    {
        if(m_extrude)
        {
            m_kmlContent->WriteString("<extrude>1</extrude>");
        }
        switch(m_elevType)
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

    if(lb != NULL)
    {
        if(lb->cntr_count() == 1)
        {
            m_kmlContent->WriteString("<Polygon>");
            WriteElevationSettings();
            m_kmlContent->WriteString("<outerBoundaryIs>");
            WriteLinearRing(lb->points(), 0, lb->point_count());
            m_kmlContent->WriteString("</outerBoundaryIs>");
            m_kmlContent->WriteString("</Polygon>");
        }
        else
        {
            m_kmlContent->WriteString("<MultiGeometry>");
            WriteElevationSettings();
            PolygonUtils::SORTEDRINGS rings;
            PolygonUtils::DetermineInteriorAndExteriorPolygons(lb, rings);
            for (PolygonUtils::SORTEDRINGS::iterator sIter = rings.begin(); sIter != rings.end(); sIter++)
            {
                RingData* pRingData = sIter->second;

                // only create polygons for outer rings
                if (pRingData->m_type != RingData::Outer)
                    continue;

                if(pRingData->m_ringPoints != NULL)
                {
                    // write the outer ring
                    m_kmlContent->WriteString("<Polygon>");
                    WriteElevationSettings();
                    m_kmlContent->WriteString("<outerBoundaryIs>");
                    WriteLinearRing(pRingData->m_ringPoints, pRingData->m_ringPointOffset, pRingData->m_ringPointCount);
                    m_kmlContent->WriteString("</outerBoundaryIs>");

                    // write the inner rings, if any
                    RingData* pChild = pRingData->m_child;
                    while(pChild != NULL)
                    {
                        if(pChild->m_ringPoints != NULL)
                        {
                            m_kmlContent->WriteString("<innerBoundaryIs>");
                            WriteLinearRing(pChild->m_ringPoints, pChild->m_ringPointOffset, pChild->m_ringPointCount);
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


void KmlRenderer::WriteLinearRing(double* points, int offset, int numPoints)
{
    m_kmlContent->WriteString("<LinearRing>");
    WriteCoordinates(points, offset, numPoints);
    m_kmlContent->WriteString("</LinearRing>");
}


void KmlRenderer::WriteCoordinates(double* points, int offset, int numPoints)
{
    char buffer[256];
    m_kmlContent->WriteString("<coordinates>");
    int pointOffset;
    for(int i = 0; i < numPoints; i ++)
    {
        pointOffset = offset + (i * 2);
        sprintf(buffer, "%f, %f, %f%s", points[pointOffset], points[pointOffset + 1], m_elevation, (i < numPoints - 1) ? "," : "");
        m_kmlContent->WriteString(buffer);
    }
    m_kmlContent->WriteString("</coordinates>");
}


void KmlRenderer::ProcessPolyline(LineBuffer* srclb, RS_LineStroke& lsym)
{
    //write style
    WriteStyle(lsym);

    m_kmlContent->WriteString("<MultiGeometry>");
    WriteElevationSettings();
    int offset = 0;
    int numCntrs = srclb->cntr_count();
    for(int i = 0; i < numCntrs; i++)
    {
        int cntr_size = srclb->cntrs()[i];

        m_kmlContent->WriteString("<LineString>");
        double* points = srclb->points();
        WriteCoordinates(points, offset, cntr_size);
        m_kmlContent->WriteString("</LineString>");

        offset += cntr_size * 2;
    }
    m_kmlContent->WriteString("</MultiGeometry>");
}


void KmlRenderer::ProcessRaster(unsigned char* /*data*/,
                               int /*length*/,
                               RS_ImageFormat /*format*/,
                               int /*width*/,
                               int /*height*/,
                               RS_Bounds /*extents*/)
{
}


void KmlRenderer::ProcessMarker(LineBuffer* srclb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* /*bounds*/)
{
    for(int i = 0; i < srclb->point_count(); i++)
    {
        ProcessOneMarker(srclb->points()[2*i], srclb->points()[2*i+1], mdef, allowOverpost);
    }
}


void KmlRenderer::ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool /*allowOverpost*/)
{
    char buffer[256];
    m_kmlContent->WriteString("<name><![CDATA[", false);
    m_kmlContent->WriteString(mdef.name(), false);
    m_kmlContent->WriteString("]]></name>");
    m_kmlContent->WriteString("<Point>");
    WriteElevationSettings();
    m_kmlContent->WriteString("<coordinates>");
    sprintf(buffer, "%f, %f, %f", x, y, m_elevation);
    m_kmlContent->WriteString(buffer);
    m_kmlContent->WriteString("</coordinates>");
    m_kmlContent->WriteString("</Point>");
}


void KmlRenderer::ProcessLabel(double /*x*/, double /*y*/, const RS_String& /*text*/, RS_TextDef& /*tdef*/)
{
}


void KmlRenderer::ProcessLabelGroup(RS_LabelInfo*    /*labels*/,
                                   int              /*nlabels*/,
                                   const RS_String& text,
                                   RS_OverpostType  /*type*/,
                                   bool             /*exclude*/,
                                   LineBuffer*      /*path*/)
{
//    char buffer[256];
    m_kmlContent->WriteString("<name><![CDATA[", false);
    m_kmlContent->WriteString(text, false);
    m_kmlContent->WriteString("]]></name>");
/*    m_kmlContent->WriteString("<MultiGeometry>");
    for(int i = 0; i < nlabels; i++)
    {
        m_kmlContent->WriteString("<Point>");
        m_kmlContent->WriteString("<coordinates>");
        sprintf(buffer, "%f, %f, 0.00000", labels[i].x(), labels[i].y());
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
    return NULL;
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


RS_Bounds& KmlRenderer::GetBounds()
{
    return m_extents;
}


double KmlRenderer::GetDpi()
{
    return 96;
}


double KmlRenderer::GetMetersPerUnit()
{
    return m_metersPerUnit;
}


bool KmlRenderer::RequiresClipping()
{
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
    for(ThemeMap::iterator iter = m_themeMap.begin(); iter != m_themeMap.end(); iter++)
    {
        delete (*iter).second;
    }
    m_themeMap.clear();
}


void KmlRenderer::ClearStyles()
{
    if(m_styleContent != NULL)
    {
        delete m_styleContent;
        m_styleContent = NULL;
    }
}


void KmlRenderer::WriteStyle(RS_FillStyle& fill)
{
    if(m_styleContent == NULL)
    {
        m_styleContent = new KmlContent();
    }

    char buffer[256];

    int thisStyleId;
    KmlPolyStyle key(fill.outline().color().abgr(), _MeterToPixels(fill.outline().units(), fill.outline().width()), fill.color().abgr());
    KmlPolyStyleIdMap::iterator iter = m_polyStyleMap.find(key);
    if(iter != m_polyStyleMap.end())
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
        m_styleContent->WriteString("<LineStyle>");
        m_styleContent->WriteString("<color>");
        RS_Color color = fill.outline().color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</color>");
        m_styleContent->WriteString("<width>");
        sprintf(buffer, "%f", key.m_lineWidth);
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</width>");
        m_styleContent->WriteString("</LineStyle>");

        //fill
        m_styleContent->WriteString("<PolyStyle>");
        m_styleContent->WriteString("<color>");
        color = fill.color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</color>");
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
    if(m_styleContent == NULL)
    {
        m_styleContent = new KmlContent();
    }

    char buffer[256];
    int thisStyleId = 0;
    KmlLineStyle key(lsym.color().abgr(), _MeterToPixels(lsym.units(), lsym.width()));
    KmlLineStyleIdMap::iterator iter = m_lineStyleMap.find(key);
    if(iter != m_lineStyleMap.end())
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
        m_styleContent->WriteString("<LineStyle>");
        m_styleContent->WriteString("<color>");
        RS_Color color = lsym.color();
        sprintf(buffer, "%.2X%.2X%.2X%.2X", color.alpha(), color.blue(), color.green(), color.red());
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</color>");
        m_styleContent->WriteString("<width>");
        sprintf(buffer, "%f", key.m_width);
        m_styleContent->WriteString(buffer);
        m_styleContent->WriteString("</width>");
        m_styleContent->WriteString("</LineStyle>");

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

    return number * scale_factor;
}

////////////////////////////////////////////////////////////////////////////////
//
// SE_Renderer implementation
//
////////////////////////////////////////////////////////////////////////////////

void KmlRenderer::DrawScreenPolyline(LineBuffer*      /*geom*/,
                                     const SE_Matrix* /*xform*/,
                                     unsigned int     /*color*/,
                                     double           /*weight*/)
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
                                   double         /*angledeg*/)
{
}

void KmlRenderer::DrawScreenText(const RS_String& /*txt*/,
                                 RS_TextDef&      /*tdef*/,
                                 double           /*insx*/,
                                 double           /*insy*/,
                                 double*          /*path*/,
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

double KmlRenderer::GetPixelsPerMillimeterScreen()
{
    return 96.0 / 25.4; //wrong but not currently used
}

double KmlRenderer::GetPixelsPerMillimeterWorld()
{
    return 96.0 / 25.4 / m_mapScale; //wrong but not currently used
}

RS_FontEngine* KmlRenderer::GetFontEngine()
{
    return NULL;
}

void KmlRenderer::ProcessLabelGroup(SE_LabelInfo*   /*labels*/,
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


void KmlRenderer::ProcessPoint(LineBuffer* geometry, SE_RenderPointStyle* style)
{
    RS_FillStyle fs;
    RS_MarkerDef mdef(1.0, 1.0, 0.5, 0.5, 0.0, RS_Units_Device, L"", L"", fs);

    ProcessMarker(geometry, mdef, !style->addToExclusionRegions, NULL);
}


void KmlRenderer::ProcessLine(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    RS_Color black(0,0,0,255);
    RS_LineStroke ls(black, 0.0, L"Solid", RS_Units_Device);

    //try to get some line style information from the SE symbol
    if (style->symbol.size())
    {
        for (size_t i=0; i<style->symbol.size(); i++)
        {
            SE_RenderPrimitive* rp = style->symbol[i];

            if (rp->type == SE_RenderPolylinePrimitive)
            {
                ls.color() = RS_Color::FromARGB(((SE_RenderPolyline*)rp)->color);
                ls.width() = ((SE_RenderPolyline*)rp)->weight / GetPixelsPerMillimeterScreen() * 0.001; //convert from pixels to meters
            }
        }
    }

    //forward the feature to the regular ProcessPolyline API
    ProcessPolyline(geometry, ls);
}


void KmlRenderer::ProcessArea(LineBuffer* geometry, SE_RenderAreaStyle* style)
{
    //not implemented upstream yet, but let's do something anyway

    RS_FillStyle fs;
    fs.color() = RS_Color(0,0,0,255);

    //try to get some fill style information from the SE symbol
    if (style->symbol.size())
    {
        for (size_t i=0; i<style->symbol.size(); i++)
        {
            SE_RenderPrimitive* rp = style->symbol[i];

            if (rp->type == SE_RenderPolygonPrimitive)
            {
                fs.color() = RS_Color::FromARGB(((SE_RenderPolygon*)rp)->fill);
                fs.outline().color() = RS_Color::FromARGB(((SE_RenderPolygon*)rp)->color);
            }
        }
    }

    ProcessPolygon(geometry, fs);
}
