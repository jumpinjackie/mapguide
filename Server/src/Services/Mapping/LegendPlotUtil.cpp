//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "LegendPlotUtil.h"
#include "MappingUtil.h"

#include "EPlotRenderer.h"
#include "Stylizer.h"
#include "FeatureTypeStyleVisitor.h"

#include "icons.h"


const double M_TO_MI  = 0.0006214;
const double MI_TO_M  = 1609.34721869443738887477;
const double MI_TO_IN = 63360;
const double IN_TO_MI = 0.0000157828283;
const double FT_TO_IN = 12;
const double IN_TO_FT = 0.0833333333;
const double KM_TO_IN = 39370.0787 ;
const double IN_TO_KM = 0.0000254;
const double M_TO_IN  = 39.3700787 ;
const double M_TO_MM  = 1000;
const double MM_TO_IN = 0.0393700787 ;
const double MM_TO_M  = 0.001;
const double IN_TO_M  = 0.0254;
const double IN_TO_CM = 2.54;
const double IN_TO_MM = 25.4;

const double PNT_TO_IN = 0.0138888888889;  // 1 font point =  1/72 of an inch

const double defaultScalebarWidth = 0.66;  // percent of page width
const double minScalebarWidth = 3.0;  // inch
const double maxScalebarWidth = 6.0;  // inch
const double whiteScaleDivisionHeight = 0.0625;  // inch
const double blackScaleDivisionStartY = 0.015625;  //inch
const double blackScaleDivisionEndY = 0.046875;  // inch
const double scaleBarTickMarkHeight = 0.0573;  // inch
const double scaleHeaderOffsetY = 0.188;  // inch
const double scaleHeaderFontHeight = 0.003; // meters
const double scaleLabelFontHeight = 0.002; // meters
const double scaleLabelOffsetY = 0.1406; // inch
const double scaleFooterOffsetY = 0.2344; // inch
const STRING scaleLabelPrefix = L"Scale 1 : ";

const double northArrowWidth = 1.0;   // inch

// Relative offset amounts for vertices in North Arrow graphics.  Units are in inches.
// See MgServerMappingService::AddNorthArrowElement() method below for description
// of the vertices.
const double northArrowMinVertexX = -0.4062;
const double northArrowMidVertexX = -0.25;
const double northArrowMaxVertexX = -0.0937;
const double northArrowMinVertexY =  0.0;
const double northArrowMidVertexY =  0.1563;
const double northArrowMaxVertexY =  0.3776;
const double northArrowNVertexOffsetMinX = -0.2812;
const double northArrowNVertexOffsetMinY =  0.4063;
const double northArrowNVertexOffsetMaxX = -0.2187;
const double northArrowNVertexOffsetMaxY =  0.5;

const INT32 bitmapPixelWidth = 16;
const INT32 bitmapPixelHeight = 16;
const double bitmapDpi = STANDARD_DISPLAY_DPI;

const double legendSpacing = bitmapPixelHeight / bitmapDpi + 0.005;
const double defaultLegendMargin = 0.1; // inch
const double legendFontHeightMeters = 0.002;
const double legendTextVertAdjust = 0.07;  // inch


MgLegendPlotUtil::MgLegendPlotUtil(MgResourceService* svcResource)
{
    m_svcResource = SAFE_ADDREF(svcResource);

    // get the name of the font to use with the legend
    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetStringValue(MgConfigProperties::MappingServicePropertiesSection,
                          MgConfigProperties::MappingServicePropertyLegendFont,
                          m_legendFontName,
                          MgConfigProperties::DefaultMappingServicePropertyLegendFont);
    assert(m_legendFontName.length() > 0);
}


MgLegendPlotUtil::~MgLegendPlotUtil()
{
    SAFE_RELEASE(m_svcResource);
}


void MgLegendPlotUtil::AddLegendElement(double dMapScale, Renderer& dr, MgMap* map, MgPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY)
{
    if (NULL == map || NULL == legendSpec)
    {
        throw new MgNullArgumentException(
            L"MgLegendPlotUtil.AddLegendElement", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING pageUnits = legendSpec->GetPageSizeUnits();
    double convertUnits = 1.0;
    if (   _wcsicmp(pageUnits.c_str(), L"mm") == 0
        || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }
    else if (_wcsicmp(pageUnits.c_str(), L"pixels") == 0)
    {
        convertUnits = dr.GetDpi();
    }

    // Here is where we have to get the layers, rules, etc.
    // But first let's generate a box for the legend border...
    LineBuffer lb(4);
    lb.MoveTo(legendOffsetX, legendOffsetY);
    lb.LineTo(legendOffsetX, legendOffsetY + legendSpec->GetPaperHeight());
    lb.LineTo(legendOffsetX + legendSpec->GetPaperWidth(), legendOffsetY + legendSpec->GetPaperHeight());
    lb.LineTo(legendOffsetX + legendSpec->GetPaperWidth(), legendOffsetY);
    lb.Close();

    RS_LineStroke lineStroke;
    dr.ProcessPolyline(&lb, lineStroke);

    //And then do the content.
    BuildLegendContent(map, dMapScale, legendSpec, legendOffsetX, legendOffsetY, dr, convertUnits);
}


void MgLegendPlotUtil::BuildLegendContent(MgMap* map, double scale, MgPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY, Renderer& dr, double convertUnits)
{
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, legendFontHeightMeters, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Left;
    textDef.valign() = RS_VAlignment_Base;

    // Get the layer info
    double x = legendOffsetX + legendSpec->GetMarginLeft();
    double y = legendOffsetY + legendSpec->GetPaperHeight() - legendSpec->GetMarginTop() - legendFontHeightMeters*M_TO_IN*convertUnits;

    //move one vertical distance down to start with since we reference coordinates
    //for icon and label from the bottom up
    y -= legendSpacing * convertUnits;

    // Add legend entries for layers that do not belong to a group
    ProcessLayersForLegend(map, scale, NULL, x, y, textDef, dr, legendSpec, legendOffsetY, convertUnits);

    // do layer groups
    Ptr<MgLayerGroupCollection> mggroups = map->GetLayerGroups();

    // iterate over groups and draw each group's layers
    for (int k = 0; k < mggroups->GetCount(); k++)
    {
        Ptr<MgLayerGroup> mggroup = mggroups->GetItem(k);
        if (mggroup == NULL)
        {
            throw new MgNullReferenceException(L"MgLegendPlotUtil.AddLegendElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgLayerGroup> mgparent = mggroup->GetGroup();

        double indent = 0;
        while (mgparent)
        {
            indent += MgPrintLayout::GroupIndent;
            mgparent = mgparent->GetGroup();
        }

        x = legendOffsetX + (defaultLegendMargin + indent)*convertUnits;

        RS_LabelInfo info(x, y + legendTextVertAdjust*convertUnits, textDef);
        dr.ProcessLabelGroup(&info, 1, mggroup->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);

        y -= legendSpacing*convertUnits;

        if (y < legendSpec->GetMarginBottom())
            break;

        // Process the layers
        ProcessLayersForLegend(map, scale, mggroup, x, y, textDef, dr, legendSpec, legendOffsetY, convertUnits);
    }
}


void MgLegendPlotUtil::ProcessLayersForLegend(MgMap* map, double mapScale, MgLayerGroup* mggroup, double startX, double& startY, RS_TextDef textDef, Renderer& dr, MgPlotSpecification* legendSpec, double legendOffsetY, double convertUnits)
{
    double x;
    double &y = startY;

    //for convenience compute legend bitmap size in plot units (inches, mm, pixels, whatever)
    double dIconWidth = ((double)bitmapPixelWidth / bitmapDpi)*convertUnits;
    double dIconHeight = ((double)bitmapPixelHeight / bitmapDpi)*convertUnits;

    //margin spacing from left edge
    double initialMarginX = defaultLegendMargin * convertUnits;

    //height of every row in the legend
    double verticalDelta = legendSpacing * convertUnits;

    //fudge factor to center text vertically with respect to the legend icon
    double verticalTextAdjust = legendTextVertAdjust * convertUnits;

    //bottom of the legend -- where we stop drawing
    double bottomLimit = legendOffsetY + legendSpec->GetMarginBottom();

    // Process the layers
    Ptr<MgLayerCollection> layers = map->GetLayers();
    for (int i = 0; i < layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> mapLayer = layers->GetItem(i);

        // layer is not currently visible -- don't add to legend
        if (!mapLayer->IsVisible())
            continue;

        Ptr<MgLayerGroup> group = mapLayer->GetGroup();

        bool bRequiredInLegend = false;
        if (group == NULL && mggroup == NULL)
            bRequiredInLegend = true;
        else if (group.p && mggroup && group->GetObjectId() == mggroup->GetObjectId())
            bRequiredInLegend = true;

        if (!bRequiredInLegend)
            continue;

        // get layer definition
        Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
        auto_ptr<MdfModel::LayerDefinition> ldf(MgMappingUtil::GetLayerDefinition(m_svcResource, layerid));

        // Get bitmaps for rules/themes
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

        if (vl)
        {
            //find correct scale range
            MdfModel::VectorScaleRange* sr = Stylizer::FindScaleRange(*vl->GetScaleRanges(), mapScale);
            if (!sr)
                continue; //layer is not visible due to scale range limits

            MdfModel::FeatureTypeStyleCollection* ftscol = sr->GetFeatureTypeStyles();

            //if there is no feature type style at all, don't add to legend
            if (ftscol->GetCount() == 0)
                continue;

            // if there are multiple feature type styles, using the first one
            MdfModel::FeatureTypeStyle* fts = ftscol->GetAt(0);
            MdfModel::RuleCollection* rules = fts->GetRules();
            int nRuleCount = rules->GetCount();

            //add the layer icon, if any
            x = startX;
            RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);

            if (nRuleCount > 1)
            {
                //in case of themed layer, use standard theme icon
                dr.ProcessRaster((unsigned char*)THEMED_LAYER_ICON, sizeof(THEMED_LAYER_ICON), RS_ImageFormat_PNG, bitmapPixelWidth, bitmapPixelHeight, b2);
            }
            else
            {
                //otherwise pick the icon from the only rule
                Ptr<MgByteReader> layerIcon = MgMappingUtil::DrawFTS(m_svcResource, fts, bitmapPixelWidth, bitmapPixelHeight, 0);

                if (layerIcon.p)
                {
                    //draw the layer icon
                    MgByteSink sink(layerIcon);
                    Ptr<MgByte> bytes = sink.ToBuffer();

                    //in case of themed layer, use standard theme icon
                    dr.ProcessRaster(bytes->Bytes(), bytes->GetLength(), RS_ImageFormat_PNG, bitmapPixelWidth, bitmapPixelHeight, b2);
                }
            }

            // Add the layer legend label text.
            x += dIconWidth + initialMarginX;
            RS_LabelInfo info(x, y + verticalTextAdjust, textDef);
            dr.ProcessLabelGroup(&info, 1, mapLayer->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);

            if (nRuleCount > 1)
            {
                //theme icons draw slightly indented
                x = startX + initialMarginX;

                //layer is themed : draw the theme icons under the layer title
                for (int i = 0; i < nRuleCount; i++)
                {
                    MdfModel::Rule* rule = rules->GetAt(i);

                    //move y cursor down one line
                    y -= verticalDelta;

                    if (y < bottomLimit)
                        break;

                    //draw the icon for the current theming rule
                    Ptr<MgByteReader> rdr = MgMappingUtil::DrawFTS(m_svcResource, fts, bitmapPixelWidth, bitmapPixelHeight, i);

                    if (rdr != NULL)
                    {
                        MgByteSink sink(rdr);
                        Ptr<MgByte> bytes = sink.ToBuffer();

                        RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);
                        dr.ProcessRaster(bytes->Bytes(), bytes->GetLength(), RS_ImageFormat_PNG, bitmapPixelWidth, bitmapPixelHeight, b2);
                    }

                    //draw the label after the icon, but also allow
                    //some blank space for better clarity
                    RS_LabelInfo info(x + dIconWidth + initialMarginX, y + verticalTextAdjust, textDef);
                    dr.ProcessLabelGroup(&info, 1, rule->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);
                }
            }
        }
        else if (gl)
        {
            //raster layer

            //find correct scale range
            MdfModel::GridScaleRange* sr = Stylizer::FindScaleRange(*gl->GetScaleRanges(), mapScale);
            if (!sr)
                continue; //layer is not visible due to scale range limits

            //use standard icon
            x = startX;
            RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);
            dr.ProcessRaster((unsigned char*)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON), RS_ImageFormat_PNG, bitmapPixelWidth, bitmapPixelHeight, b2);

            // Add the layer legend label.
            x += dIconWidth;
            RS_LabelInfo info(x, y + verticalTextAdjust, textDef);
            dr.ProcessLabelGroup(&info, 1, mapLayer->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);
        }
        else if (dl)
        {
            //drawing layer
            if (mapScale < dl->GetMinScale() || mapScale >= dl->GetMaxScale())
                continue;

            //use standard icon
            x = startX;
            RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);
            dr.ProcessRaster((unsigned char*)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON), RS_ImageFormat_PNG, bitmapPixelWidth, bitmapPixelHeight, b2);

            // Add the layer legend label.
            x += dIconWidth;
            RS_LabelInfo info(x, y + verticalTextAdjust, textDef);
            dr.ProcessLabelGroup(&info, 1, mapLayer->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);
        }

        //move y cursor down one line
        y -= verticalDelta;

        if (y < bottomLimit)
        {
            break;
        }
    }
}


//
//  Compute the legend size and the offset position on the page
//
void MgLegendPlotUtil::ComputeLegendOffsetAndSize(MgPrintLayout* layout, double mapScale, EPlotRenderer& dr, MgMap* map, double &legendOffsetX, double& legendOffsetY, double& legendWidth, double& legendHeight)
{
    double convertUnits = 1.0;
    STRING pageUnits = layout->Units();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    //height of every row in the legend
    double verticalDelta = legendSpacing * convertUnits;

    //legend x offset = map offset - legend width - legend padding
    double startX = dr.mapOffsetX() - (MgPrintLayout::LegendWidth + MgPrintLayout::LegendPadding)*convertUnits;

    Ptr<MgPlotSpecification> spec = layout->GetPlotSize();
    if (startX < spec->GetMarginLeft())
    {
        startX = spec->GetMarginLeft();
    }

    //legend y offset = map height + half the difference between the map height and page height
    double startY = (dr.GetBounds().height() + dr.mapBoundsHeight()) * 0.5;
    if (layout->ShowTitle() &&
        !layout->ShowScalebar() && !layout->ShowNorthArrow() && !layout->ShowUrl() && !layout->ShowDateTime())
    {
        startY -= MgPrintLayout::HeaderHeight*convertUnits * 0.5;
    }

    double y = startY;

    // Reserve space for layer groups in the legend
    Ptr<MgLayerGroupCollection> mggroups = map->GetLayerGroups();
    y -= verticalDelta * mggroups->GetCount();

    // Reserve space for legend labels
    Ptr<MgLayerCollection> layers = map->GetLayers();
    for (int i = 0; i < layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> mapLayer = layers->GetItem(i);
        if (!mapLayer->IsVisible())
            continue;

        //Reserve space for rules in the legend
        // Get layer definition
        Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
        auto_ptr<MdfModel::LayerDefinition> ldf(MgMappingUtil::GetLayerDefinition(m_svcResource, layerid));

        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

        if (vl)
        {
            MdfModel::VectorScaleRangeCollection* scr = vl->GetScaleRanges();

            //find correct scale range
            MdfModel::VectorScaleRange* sr = Stylizer::FindScaleRange(*vl->GetScaleRanges(), mapScale);

            if (!sr)
                continue; //layer is not visible due to scale range limits

            MdfModel::FeatureTypeStyleCollection* ftscol = sr->GetFeatureTypeStyles();

            if (ftscol->GetCount() > 0)
            {
                MdfModel::FeatureTypeStyle* fts = ftscol->GetAt(0);
                MdfModel::RuleCollection* rules = fts->GetRules();

                if (rules->GetCount() == 1)
                    y -= verticalDelta;
                else
                    y -= (rules->GetCount() + 1) * verticalDelta;
             }
        }
        else if (gl)
        {
            //raster layer
            MdfModel::GridScaleRange* sr = Stylizer::FindScaleRange(*gl->GetScaleRanges(), mapScale);

            if (!sr) continue; //layer is not visible due to scale range limits

            y -= verticalDelta;
        }
        else if (dl)
        {
            //drawing layer
            if (mapScale < dl->GetMinScale() || mapScale >= dl->GetMaxScale())
                continue;

            y -= verticalDelta;
        }
    }

    // Include size of top and bottom margins plus 2 * font height. The first font height is included
    // in the offset to the first layer, the second is added to give a little extra tolerance when
    // attempting to fit each layer into the legend image.
    y -= 2 * (defaultLegendMargin*convertUnits +  legendFontHeightMeters*M_TO_IN*convertUnits);

    // Legend cannot be taller than the map
    if (y < startY - dr.mapBoundsHeight())
    {
        y = startY - dr.mapBoundsHeight();
    }

    // Calculate bottom-left vertex for Legend
    legendOffsetX = startX;
    legendOffsetY = y;

    // Calculate the top-right vertex for Legend
    legendWidth = MgPrintLayout::LegendWidth*convertUnits;
    legendHeight = startY - y;
}


void MgLegendPlotUtil::ExtentFromMapCenter(MgMap* map, double metersPerUnit, RS_Bounds& b)
{
    int pixW = map->GetDisplayWidth();
    int pixH = map->GetDisplayHeight();
    int dpi = map->GetDisplayDpi();
    double mapScale = map->GetViewScale();

    // Check to see if we actually have width\height\dpi\scale
    if((0 == pixW) || (0 == pixH) || (0 == dpi) || (0 == mapScale))
    {
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
        b = RS_Bounds(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());
    }
    else
    {
        double unitsPerPixel = METERS_PER_INCH / (double)dpi / metersPerUnit;

        double mapWidth2 = 0.5 * (double)pixW * unitsPerPixel * mapScale;
        double mapHeight2 = 0.5 * (double)pixH * unitsPerPixel * mapScale;

        //compute map extent that corresponds to pixel extent
        Ptr<MgPoint> center = map->GetViewCenter();
        Ptr<MgCoordinate> thecenter = center->GetCoordinate();

        b = RS_Bounds(thecenter->GetX() - mapWidth2,
                    thecenter->GetY() - mapHeight2,
                    thecenter->GetX() + mapWidth2,
                    thecenter->GetY() + mapHeight2);
    }
}


void MgLegendPlotUtil::AddTitleElement(MgPrintLayout* layout, STRING& mapName, EPlotRenderer& dr)
{
    double convertUnits = 1.0;
    STRING pageUnits = layout->Units();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    // TODO: Adjust text size based on string length
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, 0.01, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Center;

    double x = layout->PageWidth() * 0.5;
    double y = layout->PageHeight() - MgPrintLayout::ScalebarHeight*convertUnits;

    STRING title = layout->GetPlotTitle();
    if (title.empty())
        title = mapName;

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, title, RS_OverpostType_All, false, NULL, 0.0);
}


void MgLegendPlotUtil::AddScalebarElement(MgPrintLayout* layout, RS_Bounds& mapBounds, double dMapScale, double dMetersPerMapUnit, EPlotRenderer& dr, RS_LineStroke& lineStroke)
{
    double convertUnits = 1.0;
    STRING pageUnits = layout->Units();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    // cache the layout metersPerUnit value
    double metersPerUnit = dr.GetMetersPerUnit();

    // position the scalebar just below the the map
    double startY = dr.mapOffsetY() - MgPrintLayout::ScalebarPadding*convertUnits;

    // Calculate distance units and divisions
    int nUnitsPerDivision = 5;
    int nDivisions = 4;
    double dDivisions = nDivisions;

    // Get the map's drawing scale
    double currentScale = dMapScale;

    // Get the length of the bounding rectangle for the scalebar
    double scalebarWidth = defaultScalebarWidth* layout->PageWidth();
    // ... clip the scalebar against its limits
    if (scalebarWidth < minScalebarWidth*convertUnits)
        scalebarWidth = minScalebarWidth*convertUnits;

    if (scalebarWidth > maxScalebarWidth*convertUnits)
        scalebarWidth = maxScalebarWidth*convertUnits;

    // Calculate the distance represented by the scalebar
    double totalDistance = scalebarWidth * currentScale;
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        totalDistance *= MM_TO_IN;
    }

    // Calculate the distance per division
    double distancePerDivision = totalDistance / dDivisions;

    // Determine the distance units
    STRING unitsText = L"";
    if (layout->GetScaleBarUnits() == MgUnitType::USEnglish)
    {
        if (distancePerDivision >= MI_TO_IN)
        {
            unitsText = L"MILES";  // NOXLATE
            distancePerDivision *= IN_TO_MI;
            totalDistance *= IN_TO_MI;
        }
        else if (distancePerDivision >= FT_TO_IN)
        {
            unitsText = L"FEET";  // NOXLATE
            distancePerDivision *= IN_TO_FT;
            totalDistance *= IN_TO_FT;
        }
        else
        {
            unitsText = L"INCHES";  // NOXLATE
        }
    }
    else
    {
        if ( distancePerDivision >= KM_TO_IN )
        {
            unitsText = L"KILOMETERS";  // NOXLATE
            distancePerDivision *= IN_TO_KM;
            totalDistance       *= IN_TO_KM;
        }
        else if ( distancePerDivision >= M_TO_IN )
        {
            unitsText = L"METERS";  // NOXLATE
            distancePerDivision *= IN_TO_M;
            totalDistance *= IN_TO_M;
        }
        else
        {
            unitsText = L"CENTIMETERS";  // NOXLATE
            distancePerDivision *= IN_TO_CM;
            totalDistance       *= IN_TO_CM;
        }
    }

    // Set the distance per division
    int nPower = int( floor( log10(distancePerDivision) ));
    distancePerDivision /= pow(10.0, nPower);

    if (distancePerDivision < 2)
    {
        distancePerDivision = 1;
    }
    else if (distancePerDivision < 5)
    {
        distancePerDivision = 2;
    }
    else
    {
        distancePerDivision = 5;
    }
    distancePerDivision *= pow(10.0, nPower);

    double inchesPerUnit = (distancePerDivision/nUnitsPerDivision) * (scalebarWidth/totalDistance);
    double inchesPerDivision = inchesPerUnit * nUnitsPerDivision;

    // Align scale bar horizontally with the map
    double scalebarStartX = dr.mapOffsetX() - MgPrintLayout::LegendPadding*convertUnits;  // legend padding

    Ptr<MgPlotSpecification> spec = layout->GetPlotSize();

    if (scalebarStartX < spec->GetMarginLeft())
        scalebarStartX = spec->GetMarginLeft();

    // Adjust horizontal position of scalebar
    if (layout->ShowNorthArrow())
    {
        double arrowExtentLeft = dr.mapOffsetX() + dr.mapWidth() - northArrowWidth*convertUnits;

        if (scalebarStartX + scalebarWidth >= arrowExtentLeft)
            scalebarStartX *= 0.5;
    }

    // ... draw division units (white bars)
    RS_Color fillColor1(255, 255, 255, 255);
    RS_Color bgColor1(255, 255, 255, 255);
    RS_FillStyle fillStyle1(lineStroke, fillColor1, bgColor1, L"Solid");  // NOXLATE

    LineBuffer lb(4);
    double x;
    double y;
    double startX = scalebarStartX;
    double dX = inchesPerUnit;
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        lb.Reset();
        x = startX;
        y = startY;
        lb.MoveTo(x, y);

        x = startX + dX;
        y = startY;
        lb.LineTo(x, y);

        x = startX + dX;
        y = startY + whiteScaleDivisionHeight*convertUnits;
        lb.LineTo(x, y);

        x = startX;
        y = startY + whiteScaleDivisionHeight*convertUnits;
        lb.LineTo(x, y);
        lb.Close();

        dr.ProcessPolygon(&lb, fillStyle1);

        startX += dX;
    }

    // ... draw division units (black bars)
    RS_Color fillColor2(0, 0, 0, 255);
    RS_Color bgColor2(255, 255, 255, 255);
    RS_FillStyle fillStyle2(lineStroke, fillColor2, bgColor2, L"Solid");  // NOXLATE

    startX = scalebarStartX;
    for (i = 0; i < 3; i++)
    {
        lb.Reset();
        x = startX;
        y = startY + blackScaleDivisionStartY*convertUnits ;
        lb.MoveTo(x, y);

        x = startX;
        y = startY + blackScaleDivisionEndY*convertUnits;
        lb.LineTo(x, y);

        x = startX + dX;
        y = startY + blackScaleDivisionEndY*convertUnits;
        lb.LineTo(x, y);

        x = startX + dX;
        y = startY + blackScaleDivisionStartY*convertUnits;
        lb.LineTo(x, y);
        lb.Close();

        dr.ProcessPolygon(&lb, fillStyle2);

        startX += 2*dX;
    }

    // ... draw divisions (white bars)
    RS_Color fillColor3(255, 255, 255, 255);
    RS_Color bgColor3(255, 255, 255, 255);
    RS_FillStyle fillStyle3(lineStroke, fillColor3, bgColor3, L"Solid");  // NOXLATE

    startX = scalebarStartX + 5*dX;
    dX = inchesPerDivision;
    for (i = 0; i < 3; i++)
    {
        lb.Reset();
        x = startX;
        y = startY;
        lb.MoveTo(x, y);

        x = startX + dX;
        y = startY;
        lb.LineTo(x, y);

        x = startX + dX;
        y = startY + whiteScaleDivisionHeight*convertUnits;
        lb.LineTo(x, y);

        x = startX;
        y = startY + whiteScaleDivisionHeight*convertUnits;
        lb.LineTo(x, y);
        lb.Close();

        dr.ProcessPolygon(&lb, fillStyle3);

        startX += dX;
    }

    // ... draw divisions (black bar)
    lb.Reset();
    x = scalebarStartX + 2*inchesPerDivision;
    y = startY + blackScaleDivisionStartY*convertUnits;
    lb.MoveTo(x, y);

    x = scalebarStartX + 3*inchesPerDivision;
    y = startY + blackScaleDivisionStartY*convertUnits;
    lb.LineTo(x, y);

    x = scalebarStartX + 3*inchesPerDivision;
    y = startY + blackScaleDivisionEndY*convertUnits;
    lb.LineTo(x, y);

    x = scalebarStartX + 2*inchesPerDivision;
    y = startY + blackScaleDivisionEndY*convertUnits;
    lb.LineTo(x, y);
    lb.Close();

    dr.ProcessPolygon(&lb, fillStyle2);

    // draw scalebar tick marks
    LineBuffer lb2(2);
    startX = scalebarStartX;
    dX = inchesPerDivision;
    for (i = 0; i < 5; i++)
    {
        lb2.Reset();
        x = startX;
        y = startY;
        lb2.MoveTo(x, y);

        x = startX;
        y = startY - scaleBarTickMarkHeight*convertUnits;
        lb2.LineTo(x, y);

        dr.ProcessPolyline(&lb2, lineStroke);

        startX += dX;
    }

    // scalebar text
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, scaleHeaderFontHeight, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Center;
    textDef.valign() = RS_VAlignment_Base;
    textDef.rotation() = 0.0;

    // ...scalebar header
    string strLabelText;
    INT32 nScale = ROUND(currentScale);
    MgUtil::Int32ToLocaleSpecificString(nScale, strLabelText);
    RS_String scaleLabelText = scaleLabelPrefix + MgUtil::MultiByteToWideChar(strLabelText);  // NOXLATE
    x = scalebarStartX + (nDivisions*inchesPerDivision) * 0.5;
    y = startY + scaleHeaderOffsetY*convertUnits;
    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, scaleLabelText, RS_OverpostType_All, false, NULL, 0.0);

    // ...scalebar labels
    textDef.font().height() = scaleLabelFontHeight;
    for (i = 0; i < 5; i++)
    {
        if (i < 1)
        {
            INT32 nDistance = ROUND(distancePerDivision);
            MgUtil::Int32ToLocaleSpecificString(nDistance, strLabelText);
            scaleLabelText = MgUtil::MultiByteToWideChar(strLabelText);
        }
        else if (i == 1)
        {
            scaleLabelText = L"0";  // NOXLATE
        }
        else // (i > 1)
        {
            INT32 nDistance = ROUND(distancePerDivision*(i-1));
            MgUtil::Int32ToLocaleSpecificString(nDistance, strLabelText);
            scaleLabelText = MgUtil::MultiByteToWideChar(strLabelText);
        }
        x = scalebarStartX + i*inchesPerDivision;
        y = startY - scaleLabelOffsetY*convertUnits;
        info.x() = x;
        info.y() = y;
        dr.ProcessLabelGroup(&info, 1, scaleLabelText, RS_OverpostType_All, false, NULL, 0.0);
    }

    // ...scalebar footer
    scaleLabelText = unitsText;
    x = scalebarStartX + (nDivisions*inchesPerDivision * 0.5);
    y = startY - scaleFooterOffsetY*convertUnits;
    info.x() = x;
    info.y() = y;
    dr.ProcessLabelGroup(&info, 1, scaleLabelText, RS_OverpostType_All, false, NULL, 0.0);
}


// MgLegendPlotUtil::AddNorthArrowElement()
//
// This method creates the graphics for the north arrow components
// Below is a diagram of the vertices and relative coordinates
// that are used to draw the graphics
//
//          vertex2   vertex4
//
//               |\    |
//               | \   |
//               |  \  |
//               |   \ |
//               |    \|
//
//          vertex1    vertex3
//
// vertex1 = (northArrowNVertexOffsetMinX, northArrowNVertexOffsetMinY)
// vertex2 = (northArrowNVertexOffsetMinX, northArrowNVertexOffsetMaxY)
// vertex3 = (northArrowNVertexOffsetMaxX, northArrowNVertexOffsetMinY)
// vertex4 = (northArrowNVertexOffsetMaxX, northArrowNVertexOffsetMaxY)
//
//
//
//               vertex7
//
//                  ^
//                 /|\
//                / |#\
//               /  |##\
//              /   |###\
//             /    |####\
//            /     .#####\
//           /     .  .####\
//          /    .      .###\
//         /  .  vertex6   .#\
//         .                 .
//
//     vertex5               vertex8
//
//
// vertex5 = (northArrowMinVertexX, northArrowMinVertexY)
// vertex6 = (northArrowMidVertexX, northArrowMidVertexY)
// vertex7 = (northArrowMidVertexX, northArrowMaxVertexY)
// vertex8 = (northArrowMaxVertexX, northArrowMinVertexY)
//
void MgLegendPlotUtil::AddNorthArrowElement(MgPrintLayout* layout, EPlotRenderer& dr, RS_LineStroke& lineStroke)
{
    LineBuffer lb(3);
    double x;
    double y;

    double convertUnits = 1.0;
    STRING pageUnits = layout->Units();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    // Align north arrow horizontally with the map
    double startX = dr.mapOffsetX() + dr.mapWidth();
    if (startX > layout->PageWidth())
    {
        Ptr<MgPlotSpecification> spec = layout->GetPlotSize();
        startX = layout->PageWidth() - spec->GetMarginLeft();
    }

    // position the north arrow just below the the map
    double startY = dr.mapOffsetY() - MgPrintLayout::ScalebarHeight*convertUnits;

    // North arrow (left half)
    x = startX + northArrowMinVertexX*convertUnits;
    y = startY + northArrowMinVertexY*convertUnits;
    lb.MoveTo(x, y);  // vertex5

    x = startX + northArrowMidVertexX*convertUnits;
    y = startY + northArrowMidVertexY*convertUnits;
    lb.LineTo(x, y);  // vertex6

    x = startX + northArrowMidVertexX*convertUnits;
    y = startY + northArrowMaxVertexY*convertUnits;
    lb.LineTo(x, y);  // vertex7

    lb.Close();
    RS_FillStyle fillStyle;
    dr.ProcessPolygon(&lb, fillStyle);

    // North arrow (right half)
    lb.Reset();
    x = startX + northArrowMidVertexX*convertUnits;
    y = startY + northArrowMidVertexY*convertUnits;
    lb.MoveTo(x, y);  // vertex6

    x = startX + northArrowMaxVertexX*convertUnits;
    y = startY + northArrowMinVertexY*convertUnits;
    lb.LineTo(x, y);  // vertex8

    x = startX + northArrowMidVertexX*convertUnits;
    y = startY + northArrowMaxVertexY*convertUnits;
    lb.LineTo(x, y);  // vertex7

    lb.Close();
    RS_Color fillColor(0, 0, 0, 255);
    RS_Color bgColor(255, 255, 255, 255);
    RS_FillStyle fillStyle2(lineStroke, fillColor, bgColor, L"Solid");  // NOXLATE
    dr.ProcessPolygon(&lb, fillStyle2);

    // Add the "N" part of the north arrow component
    lb.Reset();
    x = startX + northArrowNVertexOffsetMinX*convertUnits;
    y = startY + northArrowNVertexOffsetMinY*convertUnits;
    lb.MoveTo(x, y);  // vertex1

    x = startX + northArrowNVertexOffsetMinX*convertUnits;
    y = startY + northArrowNVertexOffsetMaxY*convertUnits;
    lb.LineTo(x, y);  // vertex2

    x = startX + northArrowNVertexOffsetMaxX*convertUnits;
    y = startY + northArrowNVertexOffsetMinY*convertUnits;
    lb.LineTo(x, y);  // vertex3

    x = startX + northArrowNVertexOffsetMaxX*convertUnits;
    y = startY + northArrowNVertexOffsetMaxY*convertUnits;
    lb.LineTo(x, y);  // vertex4

    dr.ProcessPolyline(&lb, lineStroke);
}


void MgLegendPlotUtil::AddUrlElement(MgPrintLayout* layout, STRING& mapURL, EPlotRenderer& dr)
{
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, 0.003, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Left;
    textDef.valign() = RS_VAlignment_Base;

    Ptr<MgPlotSpecification> spec = layout->GetPlotSize();
    double x = spec->GetMarginLeft();
    double y = spec->GetMarginBottom();

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, mapURL, RS_OverpostType_All, false, NULL, 0.0);
}


void MgLegendPlotUtil::AddDateTimeElement(MgPrintLayout* layout, EPlotRenderer& dr)
{
    RS_String dateAndTimeText = L"";
    struct tm *newtime;
    time_t aclock;
    time( &aclock );                 /* Get time in seconds */
    newtime = localtime( &aclock );  /* Convert time to struct */
                                    /* tm form */
    wchar_t tempBuf[128];
    wcsftime(tempBuf, 128, L"%A, %B %d, %Y %I:%M %p", newtime);  // NOXLATE
    dateAndTimeText = tempBuf;

    // Create font and right justify
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, 0.003, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Right;
    textDef.valign() = RS_VAlignment_Base;

    Ptr<MgPlotSpecification> spec = layout->GetPlotSize();
    double x = layout->PageWidth() - spec->GetMarginRight();
    double y = spec->GetMarginBottom();

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, dateAndTimeText, RS_OverpostType_All, false, NULL, 0.0);
}


void MgLegendPlotUtil::AddCustomLogoElement(MgPrintLayout* layout, EPlotRenderer& dr)
{
    Ptr<MgPlotSpecification> plotSpec = layout->GetPlotSize();
    STRING pageUnits = plotSpec->GetPageSizeUnits();
    if (!pageUnits.empty())
    {
        if ( _wcsicmp(pageUnits.c_str(), L"inches") == 0 || _wcsicmp(pageUnits.c_str(), L"in") == 0 )  // NOXLATE
        {
            pageUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(pageUnits.c_str(), L"meters") == 0 || _wcsicmp(pageUnits.c_str(), L"m") == 0)  // NOXLATE
        {
            pageUnits = L"meters";  // NOXLATE
        }
        else if ( _wcsicmp(pageUnits.c_str(), L"millimeters") == 0 || _wcsicmp(pageUnits.c_str(), L"mm") == 0)  // NOXLATE
        {
            pageUnits = L"millimeters";  // NOXLATE
        }
        else
        {
            // invalid page units specified
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(pageUnits);

            throw new MgInvalidArgumentException(L"MgLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPrintLayoutPageSizeUnits", NULL);
        }
    }
    else
    {
        // default to inches if pageUnits is not specified
        pageUnits = L"inches";  // NOXLATE
    }

    for (MgCustomLogoInfoVector::iterator iter = layout->CustomLogos().begin();
        iter != layout->CustomLogos().end(); iter++)
    {
        LineBuffer lb(1);
        MgCustomLogoInfo logoInfo = (MgCustomLogoInfo)(*iter);

        double xCoord = 0.0;
        double yCoord = 0.0;
        double positionX = logoInfo.GetX();
        double positionY = logoInfo.GetY();
        STRING positionUnits = logoInfo.GetPositionUnits();

        if ( _wcsicmp(positionUnits.c_str(), L"percent") == 0 || positionUnits.empty() )  // NOXLATE
        {
            positionUnits = L"percent";  // NOXLATE
        }
        else if ( _wcsicmp(positionUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            positionUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(positionUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            positionUnits = L"meters";  // NOXLATE
        }
        else
        {
            // invalid positionUnits specified
            throw new MgInvalidPrintLayoutPositionUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (_wcsicmp(positionUnits.c_str(), L"percent") == 0)  // NOXLATE
        {
            double pageWidth = plotSpec->GetPaperWidth();
            double pageHeight = plotSpec->GetPaperHeight();
            xCoord = positionX / 100.0 * pageWidth;
            yCoord = positionY / 100.0 * pageHeight;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"inches") == 0 && _wcsicmp(pageUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            xCoord = positionX * IN_TO_M;
            yCoord = positionY * IN_TO_M;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"inches") == 0 && _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
        {
            xCoord = positionX * IN_TO_MM;
            yCoord = positionY * IN_TO_MM;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"meters") == 0 && _wcsicmp(pageUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            xCoord = positionX * M_TO_IN;
            yCoord = positionY * M_TO_IN;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"meters") == 0 && _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
        {
            xCoord = positionX * M_TO_MM;
            yCoord = positionY * M_TO_MM;
        }
        else if ( _wcsicmp(positionUnits.c_str(), pageUnits.c_str()) == 0)
        {
            xCoord = positionX;
            yCoord = positionY;
        }
        else
        {
            // invalid positionUnits specified - should never get here
            throw new MgInvalidPrintLayoutPositionUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        lb.MoveTo(xCoord, yCoord);

        double logoWidth = 0.0;
        double logoHeight = 0.0;
        STRING sizeUnits = logoInfo.GetSizeUnits();
        if (sizeUnits.empty() )
        {
            sizeUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(sizeUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            sizeUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(sizeUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            sizeUnits = L"meters";  // NOXLATE
        }
        else
        {
            // invalid positionUnits specified
            throw new MgInvalidPrintLayoutPositionUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (sizeUnits == L"inches")  // NOXLATE
        {
            logoWidth = logoInfo.GetWidth() * IN_TO_M;
            logoHeight = logoInfo.GetHeight() * IN_TO_M;
        }
        else if (sizeUnits == L"meters")  // NOXLATE
        {
            logoWidth = logoInfo.GetWidth();
            logoHeight = logoInfo.GetHeight();
        }
        else
        {
            // invalid units specified - should never get here
            throw new MgInvalidPrintLayoutSizeUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        RS_MarkerDef markerDef;
        markerDef.type() = RS_MarkerType_W2D; 
        markerDef.units() = RS_Units_Device;
        markerDef.width() = logoWidth;
        markerDef.height() = logoHeight;
        markerDef.rotation() = logoInfo.GetRotation();
        markerDef.library() = logoInfo.GetResourceId();
        markerDef.name() = logoInfo.GetName();

        dr.ProcessMarker(&lb, markerDef, true);
    }
}


void MgLegendPlotUtil::AddCustomTextElement(MgPrintLayout* layout, EPlotRenderer& dr)
{
    Ptr<MgPlotSpecification> plotSpec = layout->GetPlotSize();
    STRING pageUnits = plotSpec->GetPageSizeUnits();
    if (!pageUnits.empty())
    {
        if ( _wcsicmp(pageUnits.c_str(), L"inches") == 0 || _wcsicmp(pageUnits.c_str(), L"in") == 0 )  // NOXLATE
        {
            pageUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(pageUnits.c_str(), L"meters") == 0 || _wcsicmp(pageUnits.c_str(), L"m") == 0)  // NOXLATE
        {
            pageUnits = L"meters";  // NOXLATE
        }
        else if ( _wcsicmp(pageUnits.c_str(), L"millimeters") == 0 || _wcsicmp(pageUnits.c_str(), L"mm") == 0)  // NOXLATE
        {
            pageUnits = L"millimeters";  // NOXLATE
        }
        else
        {
            // invalid page units specified
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(pageUnits);

            throw new MgInvalidArgumentException(L"MgLegendPlotUtil.AddCustomTextElement",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPrintLayoutPageSizeUnits", NULL);
        }
    }
    else
    {
        // default to inches if pageUnits is not specified
        pageUnits = L"inches";  // NOXLATE
    }

    RS_TextDef textDef;
    RS_String fontName;
    double fontHeight;

    textDef.halign() = RS_HAlignment_Left;
    textDef.valign() = RS_VAlignment_Base;

    for (MgCustomTextInfoVector::iterator iter = layout->CustomText().begin();
        iter != layout->CustomText().end(); iter++)
    {
        MgCustomTextInfo textInfo = (MgCustomTextInfo)(*iter);

        double xCoord = 0.0;
        double yCoord = 0.0;
        double positionX = textInfo.GetX();
        double positionY = textInfo.GetY();
        STRING positionUnits = textInfo.GetPositionUnits();
        if ( _wcsicmp(positionUnits.c_str(), L"percent") == 0 || positionUnits.empty() )  // NOXLATE
        {
            positionUnits = L"percent";  // NOXLATE
        }
        else if ( _wcsicmp(positionUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            positionUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(positionUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            positionUnits = L"meters";  // NOXLATE
        }
        else
        {
            // invalid positionUnits specified
            throw new MgInvalidPrintLayoutPositionUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (positionUnits == L"percent")  // NOXLATE
        {
            double pageWidth = plotSpec->GetPaperWidth();
            double pageHeight = plotSpec->GetPaperHeight();
            xCoord = positionX / 100.0 * pageWidth;
            yCoord = positionY / 100.0 * pageHeight;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"inches") == 0 && _wcsicmp(pageUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            xCoord = positionX * IN_TO_M;
            yCoord = positionY * IN_TO_M;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"inches") == 0 && _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
        {
            xCoord = positionX * IN_TO_MM;
            yCoord = positionY * IN_TO_MM;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"meters") == 0 && _wcsicmp(pageUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            xCoord = positionX * M_TO_IN;
            yCoord = positionY * M_TO_IN;
        }
        else if (_wcsicmp(positionUnits.c_str(), L"meters") == 0 && _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
        {
            xCoord = positionX * M_TO_MM;
            yCoord = positionY * M_TO_MM;
        }
        else if ( _wcsicmp(positionUnits.c_str(), pageUnits.c_str()) == 0)
        {
            xCoord = positionX;
            yCoord = positionY;
        }
        else
        {
            // invalid positionUnits specified - should never get here
            throw new MgInvalidPrintLayoutPositionUnitsException(L"MgLegendPlotUtil.AddCustomLogoElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        double textSize = 0.0;
        STRING sizeUnits = textInfo.GetSizeUnits();
        if (sizeUnits.empty())
        {
            sizeUnits = L"points";  // NOXLATE
        }
        else if ( _wcsicmp(sizeUnits.c_str(), L"points") == 0)  // NOXLATE
        {
            sizeUnits = L"points";  // NOXLATE
        }
        else if ( _wcsicmp(sizeUnits.c_str(), L"inches") == 0)  // NOXLATE
        {
            sizeUnits = L"inches";  // NOXLATE
        }
        else if ( _wcsicmp(sizeUnits.c_str(), L"meters") == 0)  // NOXLATE
        {
            sizeUnits = L"meters";  // NOXLATE
        }
        else
        {
            // invalid positionUnits specified
            throw new MgInvalidPrintLayoutFontSizeUnitsException(L"MgLegendPlotUtil.AddCustomTextElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (sizeUnits == L"inches")  // NOXLATE
        {
            textSize =  textInfo.GetFontHeight() * IN_TO_M;
        }
        else if (sizeUnits == L"meters" || sizeUnits.empty())  // NOXLATE
        {
            textSize = textInfo.GetFontHeight();
        }
        else if (sizeUnits == L"points")  // NOXLATE
        {
            textSize = textInfo.GetFontHeight() * PNT_TO_IN * IN_TO_M;
        }
        else
        {
            // invalid units specified
            throw new MgInvalidPrintLayoutFontSizeUnitsException(L"MgLegendPlotUtil.AddCustomTextElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        fontName = textInfo.GetFontName();
        fontHeight = textSize;
        RS_FontDef fontDef(fontName, fontHeight, RS_FontStyle_Regular, RS_Units_Device);
        textDef.font() = fontDef;
        RS_LabelInfo info(xCoord, yCoord, textDef);
        dr.ProcessLabelGroup(&info, 1, textInfo.GetValue(), RS_OverpostType_All, false, NULL, 0.0);
    }
}


void MgLegendPlotUtil::AddLayoutElements(MgPrintLayout* layout, STRING mapName, STRING mapURL, MgMap* map, MgLayerCollection* layers, RS_Bounds& mapBounds, double& dMapScale, double& dMetersPerMapUnit, EPlotRenderer& dr)
{
    RS_LineStroke lineStroke;

    RS_Bounds pageBounds(0, 0, layout->PageWidth(), layout->PageHeight());

    dr.StartLayout(pageBounds);

    double convertUnits = 1.0;
    STRING pageUnits = layout->Units();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    // Title
    if (layout->ShowTitle())
        AddTitleElement(layout, mapName, dr);

    double legendOffsetX = 0.0;
    double legendOffsetY = 0.0;

    // Legend
    if (layout->ShowLegend())
    {
        double legendWidth = 0.0;
        double legendHeight = 0.0;

        ComputeLegendOffsetAndSize(layout, dMapScale, dr, map, legendOffsetX, legendOffsetY, legendWidth, legendHeight);

        MgDwfVersion dwfVersion;
        float legendMargin = (float)(defaultLegendMargin*convertUnits);
        MgPlotSpecification legendSpec((float)legendWidth, float(legendHeight), pageUnits, legendMargin, legendMargin, legendMargin, legendMargin);

        // add the legend directly to the ePlot
        AddLegendElement(dMapScale, dr, map, &legendSpec, legendOffsetX, legendOffsetY);
    }

    // Scalebar
    if (layout->ShowScalebar())
        AddScalebarElement(layout, mapBounds, dMapScale, dMetersPerMapUnit, dr, lineStroke);

    // North arrow
    if (layout->ShowNorthArrow())
        AddNorthArrowElement(layout, dr, lineStroke);

    // URL
    if (layout->ShowUrl())
        AddUrlElement(layout, mapURL, dr);

    // Date/time
    if (layout->ShowDateTime())
        AddDateTimeElement(layout, dr);

    // Custom Logo
    if (layout->ShowCustomLogos())
        AddCustomLogoElement(layout, dr);

    // Custom Text
    if (layout->ShowCustomText())
        AddCustomTextElement(layout, dr);

    // Finish adding the layout elements to the page
    dr.EndLayout();
}
