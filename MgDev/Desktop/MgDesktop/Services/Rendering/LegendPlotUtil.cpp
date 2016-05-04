#include "LegendPlotUtil.h"
#include "MappingUtil.h"

#include "EPlotRenderer.h"
#include "Stylizer.h"
#include "FeatureTypeStyleVisitor.h"

#include "icons.h"

// Static helper method to draw a PNG icon.  Calling Renderer::ProcessRaster
// renders the PNG upside down.
static void DrawPNG(Renderer* dr, unsigned char* data, int length, int width, int height, RS_Bounds& extents)
{
    SE_Renderer* drSE = dynamic_cast<SE_Renderer*>(dr);
    if (!drSE)
        return;

    double minx, miny, maxx, maxy;
    drSE->WorldToScreenPoint(extents.minx, extents.miny, minx, miny);
    drSE->WorldToScreenPoint(extents.maxx, extents.maxy, maxx, maxy);

    double cx = 0.5 * (minx + maxx);
    double cy = 0.5 * (miny + maxy);
    double imgDevW = fabs(maxx - minx);
    double imgDevH = fabs(maxy - miny);

    drSE->DrawScreenRaster(data, length, RS_ImageFormat_PNG, width, height, cx, cy, imgDevW, imgDevH, 0.0);
}


MgdLegendPlotUtil::MgdLegendPlotUtil(MgResourceService* svcResource)
{
    m_svcResource = SAFE_ADDREF(svcResource);

    // get the name of the font to use with the legend
    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetStringValue(MgdConfigProperties::MappingServicePropertiesSection,
                          MgdConfigProperties::MappingServicePropertyLegendFont,
                          m_legendFontName,
                          MgdConfigProperties::DefaultMappingServicePropertyLegendFont);
    assert(m_legendFontName.length() > 0);
}


MgdLegendPlotUtil::~MgdLegendPlotUtil()
{
    SAFE_RELEASE(m_svcResource);
}


void MgdLegendPlotUtil::AddLegendElement(double dMapScale, Renderer& dr, MgdMap* map, MgdPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY)
{
    if (NULL == map || NULL == legendSpec)
    {
        throw new MgNullArgumentException(
            L"MgdLegendPlotUtil.AddLegendElement", __LINE__, __WFILE__, NULL, L"", NULL);
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
    lb.MoveTo(legendOffsetX                              , legendOffsetY                               );
    lb.LineTo(legendOffsetX                              , legendOffsetY + legendSpec->GetPaperHeight());
    lb.LineTo(legendOffsetX + legendSpec->GetPaperWidth(), legendOffsetY + legendSpec->GetPaperHeight());
    lb.LineTo(legendOffsetX + legendSpec->GetPaperWidth(), legendOffsetY                               );
    lb.Close();

    RS_LineStroke lineStroke;
    dr.ProcessPolyline(&lb, lineStroke);

    //Pad left at the top-level
    legendOffsetX += (defaultLegendMargin * convertUnits);

    //And then do the content.
    BuildLegendContent(map, dMapScale, legendSpec, legendOffsetX, legendOffsetY, dr, convertUnits);
}


void MgdLegendPlotUtil::BuildLegendContent(MgdMap* map, double scale, MgdPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY, Renderer& dr, double convertUnits)
{
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, legendFontHeightMeters, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Left;
    textDef.valign() = RS_VAlignment_Base;

    //for convenience compute legend bitmap size in plot units (inches, mm, pixels, whatever)
    double dIconWidth = ((double)bitmapPixelWidth / bitmapDpi)*convertUnits;
    double dIconHeight = ((double)bitmapPixelHeight / bitmapDpi)*convertUnits;

    // Get the layer info
    double x = legendOffsetX + legendSpec->GetMarginLeft();
    double y = legendOffsetY + legendSpec->GetPaperHeight() - legendSpec->GetMarginTop() - legendFontHeightMeters*M_TO_IN*convertUnits;

    //move one vertical distance down to start with since we reference coordinates
    //for icon and label from the bottom up
    y -= legendSpacing * convertUnits;

    // Compile the necessary information needed to avoid repeated linear searches for this information
    LayerGroupChildMap groupChildren;
    VisibleLayerCountMap visibleLayers;
    CompileInformation(map, visibleLayers, groupChildren);

    // Add legend entries for layers that do not belong to a group
    ProcessLayersForLegend(map, scale, NULL, x, y, textDef, dr, legendSpec, legendOffsetY, convertUnits, visibleLayers, groupChildren);
}

void MgdLegendPlotUtil::CompileInformation(MgdMap* map, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren)
{
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();

    for (INT32 i = 0; i < layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> layer = layers->GetItem(i);
        if (!layer->IsVisible()) //Not visible
            continue;

        Ptr<MgLayerGroup> parentGroup = layer->GetGroup();
        if (NULL == parentGroup.p) //No parent
            continue;

        STRING parentGroupName = parentGroup->GetName();
        VisibleLayerCountMap::iterator vit = visibleLayers.find(parentGroupName);
        if (vit == visibleLayers.end())
            visibleLayers[parentGroupName] = 0;
        visibleLayers[parentGroupName]++;
    }

    for (INT32 i = 0; i < groups->GetCount(); i++)
    {
        Ptr<MgLayerGroup> group = groups->GetItem(i);
        Ptr<MgLayerGroup> parentGroup = group->GetGroup();
        if (NULL != parentGroup.p)
        {
            STRING groupName = group->GetName();
            STRING parentGroupName = parentGroup->GetName();
            LayerGroupChildMap::iterator cit = groupChildren.find(parentGroupName);
            if (cit == groupChildren.end())
                groupChildren[parentGroupName] = LayerGroupList();
            groupChildren[parentGroupName].push_back(groupName);
        }
    }
}

void MgdLegendPlotUtil::ProcessLayersForLegend(MgdMap* map, double mapScale, MgLayerGroup* mggroup, double startX, double& startY, RS_TextDef textDef, Renderer& dr, MgdPlotSpecification* legendSpec, double legendOffsetY, double convertUnits, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren)
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

    if (NULL != mggroup)
    {
        x = startX - initialMarginX;
        // use group icon
        RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);
        DrawPNG(&dr, (unsigned char*)LAYER_GROUP_ICON, sizeof(LAYER_GROUP_ICON), bitmapPixelWidth, bitmapPixelHeight, b2);

        // Add the group legend label.
        RS_LabelInfo info(x + dIconWidth + (defaultLegendMargin * convertUnits), y + legendTextVertAdjust*convertUnits, textDef);
        dr.ProcessLabelGroup(&info, 1, mggroup->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);

        // Indent for children
        x += initialMarginX;

        //move y cursor down one line
        y -= verticalDelta;

        if (y < bottomLimit)
        {
            return;
        }
    }

    // build the list of layers that need to be processed
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgStringCollection> layerIds = new MgStringCollection();
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

        Ptr<MgResourceIdentifier> layerId = mapLayer->GetLayerDefinition();
        layerIds->Add(layerId->ToString());
    }

    // get resource data
    if (layerIds->GetCount() != 0)
    {
        Ptr<MgStringCollection> layerContents = m_svcResource->GetResourceContents(layerIds, NULL);
        for (int i = 0; i < layerIds->GetCount(); i++)
        {
            for (int j = 0; j < layers->GetCount(); j++)
            {
                Ptr<MgLayerBase> mapLayer = layers->GetItem(j);
                Ptr<MgResourceIdentifier> layerId = mapLayer->GetLayerDefinition();
                if (layerId->ToString() == layerIds->GetItem(i))
                {
                    mapLayer->SetLayerResourceContent(layerContents->GetItem(i));
                    break;
                }
            }
        }
    }

    // process the layers
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

        STRING content = mapLayer->GetLayerResourceContent();
        if (content.empty())
            continue;

        // get layer definition
        auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(content));

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

            //Peek ahead to determine what we're dealing with
            bool bThemed = false;
            int nCompositeStyleCount = 0;
            MdfModel::FeatureTypeStyle* singleFTS = NULL;
            MdfModel::FeatureTypeStyle* singleCTS = NULL;
            int nTotalFTSRules = 0;
            int nTotalCTSRules = 0;
            //Compile type/rule count to determine if this is a themed layer
            for (int j = 0; j < ftscol->GetCount(); j++)
            {
                MdfModel::FeatureTypeStyle* fts = ftscol->GetAt(j);
                if (!fts->IsShowInLegend())
                    continue;

                MdfModel::RuleCollection* rules = fts->GetRules();
                MdfModel::CompositeTypeStyle* cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(fts);
                //Composite Styles take precedence (it's what the stylizer does. So we're doing the same thing here). 
                //If a Composite Style exists, we're using it.
                if (NULL != cts)
                {
                    //For a single type scale range, this will only be set once. Otherwise it could
                    //be set many times over (for each style type we encounter), but this is okay 
                    //because we won't be using this pointer anyway for multi-type layers
                    singleCTS = fts;
                    nTotalCTSRules += rules->GetCount();
                    nCompositeStyleCount++;
                }
                else
                {
                    //For a single type scale range, this will only be set once. Otherwise it could
                    //be set many times over (for each style type we encounter), but this is okay 
                    //because we won't be using this pointer anyway for multi-type layers
                    singleFTS = fts;
                    nTotalFTSRules += rules->GetCount();
                }
            }
            //It's a themed layer if we found more than one rule of that type
            bThemed = (nTotalFTSRules > 1);
            //Found a composite style, re-evaluate against composite rule count
            if (nCompositeStyleCount > 0)
                bThemed = (nTotalCTSRules > 1);

            //add the layer icon, if any
            x = startX;
            RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);

            if (bThemed)
            {
                //in case of themed layer, use standard theme icon
                DrawPNG(&dr, (unsigned char*)THEMED_LAYER_ICON, sizeof(THEMED_LAYER_ICON), bitmapPixelWidth, bitmapPixelHeight, b2);
            }
            else
            {
                assert(NULL != singleFTS || NULL != singleCTS); //Should've been set by peek-ahead from above

                MdfModel::FeatureTypeStyle* theFTS = NULL;
                //Composite TS takes precedence
                if (NULL != singleCTS)
                    theFTS = singleCTS;
                else if (NULL != singleFTS)
                    theFTS = singleFTS;

                //otherwise pick the icon from the only rule
                Ptr<MgByteReader> layerIcon = MgdMappingUtil::DrawFTS(m_svcResource, theFTS, bitmapPixelWidth, bitmapPixelHeight, 0);

                if (layerIcon.p)
                {
                    //draw the layer icon
                    MgByteSink sink(layerIcon);
                    Ptr<MgByte> bytes = sink.ToBuffer();

                    //in case of themed layer, use standard theme icon
                    DrawPNG(&dr, bytes->Bytes(), bytes->GetLength(), bitmapPixelWidth, bitmapPixelHeight, b2);
                }
            }

            // Add the layer legend label text.
            x += dIconWidth + initialMarginX;
            RS_LabelInfo info(x, y + verticalTextAdjust, textDef);
            dr.ProcessLabelGroup(&info, 1, mapLayer->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);

            //layer is themed : draw the theme icons under the layer title
            if (bThemed)
            {
                //theme icons draw slightly indented
                x = startX + initialMarginX;

                for (int j = 0; j < ftscol->GetCount(); j++)
                {
                    MdfModel::FeatureTypeStyle* fts = ftscol->GetAt(j);
                    //Skip items not marked for display
                    if (!fts->IsShowInLegend())
                        continue;

                    MdfModel::CompositeTypeStyle* cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(fts);
                    //If we found one or more composite styles, then they take precedence. So skip all non-composite ones
                    if (nCompositeStyleCount > 0 && NULL == cts)
                        continue;

                    MdfModel::RuleCollection* rules = fts->GetRules();

                    for (int k = 0; k < rules->GetCount(); k++)
                    {
                        MdfModel::Rule* rule = rules->GetAt(k);

                        //move y cursor down one line
                        y -= verticalDelta;

                        if (y < bottomLimit)
                            break;

                        //draw the icon for the current theming rule
                        Ptr<MgByteReader> rdr = MgdMappingUtil::DrawFTS(m_svcResource, fts, bitmapPixelWidth, bitmapPixelHeight, k);

                        if (rdr != NULL)
                        {
                            MgByteSink sink(rdr);
                            Ptr<MgByte> bytes = sink.ToBuffer();

                            RS_Bounds b2(x, y, x + dIconWidth, y + dIconHeight);
                            DrawPNG(&dr, bytes->Bytes(), bytes->GetLength(), bitmapPixelWidth, bitmapPixelHeight, b2);
                        }

                        //draw the label after the icon, but also allow
                        //some blank space for better clarity
                        RS_LabelInfo info(x + dIconWidth + initialMarginX, y + verticalTextAdjust, textDef);
                        dr.ProcessLabelGroup(&info, 1, rule->GetLegendLabel(), RS_OverpostType_All, false, NULL, 0.0);
                    }

                    if (y < bottomLimit)
                        break;
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
            DrawPNG(&dr, (unsigned char*)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON), bitmapPixelWidth, bitmapPixelHeight, b2);

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
            DrawPNG(&dr, (unsigned char*)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON), bitmapPixelWidth, bitmapPixelHeight, b2);

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

    //Process child groups of this legend
    Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
    for (int i = 0; i < groups->GetCount(); i++)
    {
        Ptr<MgLayerGroup> group = groups->GetItem(i);
        if (!group->GetDisplayInLegend())
            continue;

        Ptr<MgLayerGroup> groupParent = group->GetGroup();
        if (groupParent.p == mggroup)
        {
            //If this group has no visible layers, skip it
            if (!HasVisibleLayers(group->GetName(), visibleLayers, groupChildren))
                continue;

            ProcessLayersForLegend(map, mapScale, group, startX + initialMarginX, y, textDef, dr, legendSpec, legendOffsetY, convertUnits, visibleLayers, groupChildren);
            if (y < bottomLimit)
            {
                break;
            }
        }
    }
}



void MgdLegendPlotUtil::ExtentFromMapCenter(MgdMap* map, double metersPerUnit, RS_Bounds& b)
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

void MgdLegendPlotUtil::AddTitleElement(MgdPrintLayout* layout, STRING& mapName, EPlotRenderer& dr)
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
    double y = layout->PageHeight() - MgdPrintLayout::ScalebarHeight*convertUnits;

    STRING title = layout->GetPlotTitle();
    if (title.empty())
        title = mapName;

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, title, RS_OverpostType_All, false, NULL, 0.0);
}


void MgdLegendPlotUtil::AddScalebarElement(MgdPrintLayout* layout, RS_Bounds& mapBounds, double dMapScale, double dMetersPerMapUnit, EPlotRenderer& dr, RS_LineStroke& lineStroke)
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
    double startY = dr.mapOffsetY() - MgdPrintLayout::ScalebarPadding*convertUnits;

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
    if (layout->GetScaleBarUnits() == MgdUnitType::USEnglish)
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
    double scalebarStartX = dr.mapOffsetX() - MgdPrintLayout::LegendPadding*convertUnits;  // legend padding

    Ptr<MgdPlotSpecification> spec = layout->GetPlotSize();

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

void MgdLegendPlotUtil::AddNorthArrowElement(MgdPrintLayout* layout, EPlotRenderer& dr, RS_LineStroke& lineStroke)
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
        Ptr<MgdPlotSpecification> spec = layout->GetPlotSize();
        startX = layout->PageWidth() - spec->GetMarginLeft();
    }

    // position the north arrow just below the the map
    double startY = dr.mapOffsetY() - MgdPrintLayout::ScalebarHeight*convertUnits;

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


void MgdLegendPlotUtil::AddUrlElement(MgdPrintLayout* layout, STRING& mapURL, EPlotRenderer& dr)
{
    RS_TextDef textDef;
    RS_FontDef fontDef(m_legendFontName, 0.003, RS_FontStyle_Regular, RS_Units_Device);
    textDef.font() = fontDef;
    textDef.halign() = RS_HAlignment_Left;
    textDef.valign() = RS_VAlignment_Base;

    Ptr<MgdPlotSpecification> spec = layout->GetPlotSize();
    double x = spec->GetMarginLeft();
    double y = spec->GetMarginBottom();

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, mapURL, RS_OverpostType_All, false, NULL, 0.0);
}


void MgdLegendPlotUtil::AddDateTimeElement(MgdPrintLayout* layout, EPlotRenderer& dr)
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

    Ptr<MgdPlotSpecification> spec = layout->GetPlotSize();
    double x = layout->PageWidth() - spec->GetMarginRight();
    double y = spec->GetMarginBottom();

    RS_LabelInfo info(x, y, textDef);
    dr.ProcessLabelGroup(&info, 1, dateAndTimeText, RS_OverpostType_All, false, NULL, 0.0);
}


void MgdLegendPlotUtil::AddCustomLogoElement(MgdPrintLayout* layout, EPlotRenderer& dr)
{
    Ptr<MgdPlotSpecification> plotSpec = layout->GetPlotSize();
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

            throw new MgInvalidArgumentException(L"MgdLegendPlotUtil.AddCustomLogoElement",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPrintLayoutPageSizeUnits", NULL);
        }
    }
    else
    {
        // default to inches if pageUnits is not specified
        pageUnits = L"inches";  // NOXLATE
    }

    for (MgdCustomLogoInfoVector::iterator iter = layout->CustomLogos().begin();
        iter != layout->CustomLogos().end(); iter++)
    {
        LineBuffer lb(1);
        MgdCustomLogoInfo logoInfo = (MgdCustomLogoInfo)(*iter);

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
            throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement",
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
            throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement",
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
            throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement",
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
            throw new MgdInvalidPrintLayoutSizeUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement",
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


void MgdLegendPlotUtil::AddCustomTextElement(MgdPrintLayout* layout, EPlotRenderer& dr)
{
    Ptr<MgdPlotSpecification> plotSpec = layout->GetPlotSize();
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

            throw new MgInvalidArgumentException(L"MgdLegendPlotUtil.AddCustomTextElement",
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

    for (MgdCustomTextInfoVector::iterator iter = layout->CustomText().begin();
        iter != layout->CustomText().end(); iter++)
    {
        MgdCustomTextInfo textInfo = (MgdCustomTextInfo)(*iter);

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
            throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement", __LINE__, __WFILE__, NULL, L"", NULL);
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
            throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdLegendPlotUtil.AddCustomLogoElement", __LINE__, __WFILE__, NULL, L"", NULL);
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
            throw new MgdInvalidPrintLayoutFontSizeUnitsException(L"MgdLegendPlotUtil.AddCustomTextElement", __LINE__, __WFILE__, NULL, L"", NULL);
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
            throw new MgdInvalidPrintLayoutFontSizeUnitsException(L"MgdLegendPlotUtil.AddCustomTextElement", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        fontName = textInfo.GetFontName();
        fontHeight = textSize;
        RS_FontDef fontDef(fontName, fontHeight, RS_FontStyle_Regular, RS_Units_Device);
        textDef.font() = fontDef;
        RS_LabelInfo info(xCoord, yCoord, textDef);
        dr.ProcessLabelGroup(&info, 1, textInfo.GetValue(), RS_OverpostType_All, false, NULL, 0.0);
    }
}


void MgdLegendPlotUtil::AddLayoutElements(MgdPrintLayout* layout, STRING mapName, STRING mapURL, MgdMap* map, MgLayerCollection* layers, RS_Bounds& mapBounds, double& dMapScale, double& dMetersPerMapUnit, EPlotRenderer& dr)
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

        float legendMargin = (float)(defaultLegendMargin*convertUnits);
        MgdPlotSpecification legendSpec((float)legendWidth, float(legendHeight), pageUnits, legendMargin, legendMargin, legendMargin, legendMargin);

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

bool MgdLegendPlotUtil::HasVisibleLayers(CREFSTRING groupName, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren)
{
    INT32 total = 0;
    VisibleLayerCountMap::iterator vit = visibleLayers.find(groupName);
    if (vit != visibleLayers.end())
    {
        if (vit->second > 0)
            return true;
    }

    LayerGroupChildMap::iterator cit = groupChildren.find(groupName);
    if (cit != groupChildren.end())
    {
        for (LayerGroupList::iterator lit = cit->second.begin(); lit != cit->second.end(); lit++)
        {
            if (HasVisibleLayers(*lit, visibleLayers, groupChildren))
                return true;
        }
    }
    return false;
}

//
//  Compute the legend size and the offset position on the page
//
void MgdLegendPlotUtil::ComputeLegendOffsetAndSize(MgdPrintLayout* layout, double mapScale, EPlotRenderer& dr, MgdMap* map, double &legendOffsetX, double& legendOffsetY, double& legendWidth, double& legendHeight)
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
    double startX = dr.mapOffsetX() - (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;

    Ptr<MgdPlotSpecification> spec = layout->GetPlotSize();
    if (startX < spec->GetMarginLeft())
    {
        startX = spec->GetMarginLeft();
    }

    //legend y offset = map height + half the difference between the map height and page height
    double startY = (dr.GetBounds().height() + dr.mapBoundsHeight()) * 0.5;
    if (layout->ShowTitle() &&
        !layout->ShowScalebar() && !layout->ShowNorthArrow() && !layout->ShowUrl() && !layout->ShowDateTime())
    {
        startY -= MgdPrintLayout::HeaderHeight*convertUnits * 0.5;
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
        auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, layerid));

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
    legendWidth = MgdPrintLayout::LegendWidth*convertUnits;
    legendHeight = startY - y;
}