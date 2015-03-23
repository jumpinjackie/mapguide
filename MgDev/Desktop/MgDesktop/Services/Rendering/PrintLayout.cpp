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

#include "MgDesktop.h"

#include "System/XmlDefs.h"
#include "System/XmlUtil.h"

// Layout element constants
const double MgdPrintLayout::LegendWidth     = 2.0;
const double MgdPrintLayout::LegendPadding   = 0.1;
const double MgdPrintLayout::HeaderHeight    = 1.0;
const double MgdPrintLayout::ScalebarHeight  = 0.75;
const double MgdPrintLayout::ScalebarPadding = 0.5;
const double MgdPrintLayout::FooterHeight    = 0.5;
const double MgdPrintLayout::GroupIndent     = 0.3;

//const double METERS_PER_INCH      = 0.0254; // meters to inches conversion
//const double MILLIMETERS_PER_INCH = 25.4;   // millimeters to inches conversion


MG_IMPL_DYNCREATE(MgdPrintLayout)

// Construct a MgdPrintLayout object
//
MgdPrintLayout::MgdPrintLayout() :
    m_scaleBarUnits(MgdUnitType::Metric),
    m_dPlotScale(0),
    m_dPageWidth(0),
    m_dPageHeight(0),
    m_bShowTitle(false),
    m_bShowLegend(false),
    m_bShowScalebar(false),
    m_bShowNorthArrow(false),
    m_bShowUrl(false),
    m_bShowDateTime(false),
    m_bShowCustomLogos(false),
    m_bShowCustomText(false)
{
    m_plotSpec = new MgdPlotSpecification(8.5f, 11.f, L"inches");  // NOXLATE
    m_plotCenter = new MgCoordinateXY(0.0, 0.0);
    m_bgColor = new MgColor(255, 255, 255, 0);
}


// Destructor for MgdPrintLayout
MgdPrintLayout::~MgdPrintLayout()
{
}


// Initialize a new MgdPrintLayout object given a resource service
// and print layout defintion.
//
void MgdPrintLayout::Create(MgResourceService* resourceService, MgResourceIdentifier* layoutDefinition)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    if (0 == resourceService || 0 == layoutDefinition)
    {
        throw new MgNullArgumentException(
            L"MgdPrintLayout.Create", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the PrintLayout contents
    byteReader = resourceService->GetResourceContent(layoutDefinition);
    Ptr<MgByteSink> byteSink = new MgByteSink(byteReader.p);
    string resourceContent;
    byteSink->ToStringUtf8(resourceContent);

    // Parse the XML
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(resourceContent.c_str());

    // Get the PrintLayout properties
    GetLayoutPropertiesFromXml(&xmlUtil);

    MG_CATCH_AND_THROW(L"MgdPrintLayout.Create")
}


// Get the plot title
//
STRING MgdPrintLayout::GetPlotTitle()
{
    return m_plotTitle;
}


// Set the plot title.  This overrides the default plot title
// with a user specified title.
//
void MgdPrintLayout::SetPlotTitle(CREFSTRING plotTitle)
{
    m_plotTitle = plotTitle;
}


//  Get the scale to use for generating the plot with layout.
//
double MgdPrintLayout::GetPlotScale()
{
    return m_dPlotScale;
}


// Sets the map scale for the plot.  This will override the
// current map scale with the specified scale.
//
void MgdPrintLayout::SetPlotScale(double dMapScale)
{
    m_dPlotScale = dMapScale;
}


// Get the map center for generating the plot.
//
MgCoordinate* MgdPrintLayout::GetPlotCenter()
{
    return SAFE_ADDREF((MgCoordinate*)m_plotCenter);
}


// Sets the map center for generating the plot.  This will override the
// current map center with the specified map center.
void MgdPrintLayout::SetPlotCenter(MgCoordinate* center)
{
    m_plotCenter = SAFE_ADDREF(center);
}


// Get the plot size.
//
MgdPlotSpecification* MgdPrintLayout::GetPlotSize()
{
    return SAFE_ADDREF(m_plotSpec.p);
}


// Set the plot size.  This will override the page size specified
// in the PrintLayout resource.
//
void MgdPrintLayout::SetPlotSize(MgdPlotSpecification* plotSpec)
{
    m_plotSpec = SAFE_ADDREF(plotSpec);
}


// Gets the system of measurement for the scale bar.
//
STRING MgdPrintLayout::GetScaleBarUnits()
{
    return m_scaleBarUnits;
}


// Sets the system of measurement for the scale bar.
//
void MgdPrintLayout::SetScaleBarUnits(CREFSTRING units)
{
    m_scaleBarUnits = units;
}


// Gets the background color for the layout.
//
MgColor* MgdPrintLayout::GetBackgroundColor()
{
    return SAFE_ADDREF((MgColor*)m_bgColor);
}


// Helper method to get the layout properties from the PrintLayout xml resource.
void MgdPrintLayout::GetLayoutPropertiesFromXml(MgXmlUtil* pXmlUtil)
{
    CHECKNULL(pXmlUtil, L"MgdPrintLayout.GetLayoutPropertiesFromXml()");

    MG_TRY()

    // Retrieve values from the xml document
    DOMElement* root = pXmlUtil->GetRootNode();
    CHECKNULL(root, L"MgdPrintLayout.GetLayoutProperitesFromXml()");

//  DOMElement* pageProperties = pXmlUtil->GetElementNode(root, "PageProperties");

    DOMElement* backgroundColor = pXmlUtil->GetElementNode(root, "BackgroundColor");

    wstring szRed;
    pXmlUtil->GetElementValue(backgroundColor, "Red", szRed);

    wstring szGreen;
    pXmlUtil->GetElementValue(backgroundColor, "Green", szGreen);

    wstring szBlue;
    pXmlUtil->GetElementValue(backgroundColor, "Blue", szBlue);

    DOMElement* layoutProperties = pXmlUtil->GetElementNode(root, "LayoutProperties");

    wstring szShowTitle;
    pXmlUtil->GetElementValue(layoutProperties, "ShowTitle", szShowTitle, false);

    wstring szShowLegend;
    pXmlUtil->GetElementValue(layoutProperties, "ShowLegend", szShowLegend, false);

    wstring szShowScalebar;
    pXmlUtil->GetElementValue(layoutProperties, "ShowScaleBar", szShowScalebar, false);

    wstring szShowNorthArrow;
    pXmlUtil->GetElementValue(layoutProperties, "ShowNorthArrow", szShowNorthArrow, false);

    wstring szShowUrl;
    pXmlUtil->GetElementValue(layoutProperties, "ShowURL", szShowUrl, false);

    wstring szShowDateTime;
    pXmlUtil->GetElementValue(layoutProperties, "ShowDateTime", szShowDateTime, false);

    wstring szShowCustomLogos;
    pXmlUtil->GetElementValue(layoutProperties, "ShowCustomLogos", szShowCustomLogos, false);

    wstring szShowCustomText;
    pXmlUtil->GetElementValue(layoutProperties, "ShowCustomText", szShowCustomText, false);

    DOMNode* customLogosNode = pXmlUtil->GetElementNode(root, "CustomLogos", false);
    if (NULL != customLogosNode)
    {
        DOMNodeList* customLogosNodeList = pXmlUtil->GetNodeList(customLogosNode, "Logo");
        if (NULL != customLogosNodeList)
        {
            for (XMLSize_t i = 0; i < customLogosNodeList->getLength(); ++i)
            {
                MgdCustomLogoInfo logoInfo;
                wstring positionX;
                wstring positionY;
                wstring positionUnits;
                wstring resId;
                wstring name;
                wstring sizeWidth;
                wstring sizeHeight;
                wstring sizeUnits;
                wstring rotation;

                DOMNode* logoNode = customLogosNodeList->item(i);
                DOMNode* logoPositionNode = pXmlUtil->GetElementNode(logoNode, "Position", false);
                if (NULL != logoPositionNode)
                {
                    pXmlUtil->GetElementValue(logoPositionNode, "Left", positionX, false);
                    pXmlUtil->GetElementValue(logoPositionNode, "Bottom", positionY, false);
                    pXmlUtil->GetElementValue(logoPositionNode, "Units", positionUnits, false);
                    if (!positionUnits.empty()
                        && positionUnits != L"meters" && positionUnits != L"inches" && positionUnits != L"percent")
                    {
                        // invalid print layout position units
                        throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdPrintLayout.GetLayoutPropertiesFromXml", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
                pXmlUtil->GetElementValue(logoNode, "ResourceId", resId, false);
                pXmlUtil->GetElementValue(logoNode, "Name", name, false);
                DOMNode* logoSizeNode = pXmlUtil->GetElementNode(logoNode, "Size", false);
                if (NULL != logoSizeNode)
                {
                    pXmlUtil->GetElementValue(logoSizeNode, "Width", sizeWidth, false);
                    pXmlUtil->GetElementValue(logoSizeNode, "Height", sizeHeight, false);
                    pXmlUtil->GetElementValue(logoSizeNode, "Units", sizeUnits, false);
                    if (!sizeUnits.empty()
                        && sizeUnits != L"inches" && sizeUnits != L"meters")
                    {
                        // invalid print layout size units
                        throw new MgdInvalidPrintLayoutSizeUnitsException(L"MgdPrintLayout.GetLayoutPropertiesFromXml", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
                pXmlUtil->GetElementValue(logoNode, "Rotation", rotation, false);
                logoInfo.SetX( MgUtil::StringToDouble(positionX) );
                logoInfo.SetY( MgUtil::StringToDouble(positionY) );
                logoInfo.SetPositionUnits( positionUnits );
                logoInfo.SetResourceId( resId );
                logoInfo.SetName( name );
                logoInfo.SetWidth( MgUtil::StringToDouble(sizeWidth) );
                logoInfo.SetHeight( MgUtil::StringToDouble(sizeHeight) );
                logoInfo.SetSizeUnits( sizeUnits );
                logoInfo.SetRotation( MgUtil::StringToDouble(rotation) );

                CustomLogos().push_back(logoInfo);
            }
        }
    }

    DOMNode* customTextNode = pXmlUtil->GetElementNode(root, "CustomText", false);
    if (NULL != customTextNode)
    {
        DOMNodeList* customTextNodeList = pXmlUtil->GetNodeList(customTextNode, "Text");
        if (NULL != customTextNodeList)
        {
            for (XMLSize_t i = 0; i < customTextNodeList->getLength(); ++i)
            {
                MgdCustomTextInfo textInfo;
                wstring positionX;
                wstring positionY;
                wstring positionUnits;
                wstring value;
                wstring fontName;
                wstring fontHeight;
                wstring fontSizeUnits;

                DOMNode* textNode = customTextNodeList->item(i);
                DOMNode* textPositionNode = pXmlUtil->GetElementNode(textNode, "Position", false);
                if (NULL != textPositionNode)
                {
                    pXmlUtil->GetElementValue(textPositionNode, "Left", positionX, false);
                    pXmlUtil->GetElementValue(textPositionNode, "Bottom", positionY, false);
                    pXmlUtil->GetElementValue(textPositionNode, "Units", positionUnits, false);
                    if (!positionUnits.empty()
                        && positionUnits != L"percent" && positionUnits != L"meters" && positionUnits != L"inches")
                    {
                        // invalid print layout position units
                        throw new MgdInvalidPrintLayoutPositionUnitsException(L"MgdPrintLayout.GetLayoutPropertiesFromXml", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
                DOMNode* fontNode = pXmlUtil->GetElementNode(textNode, "Font", false);
                if (NULL != fontNode)
                {
                    pXmlUtil->GetElementValue(fontNode, "Name", fontName, false);
                    pXmlUtil->GetElementValue(fontNode, "Height", fontHeight, false);
                    pXmlUtil->GetElementValue(fontNode, "Units", fontSizeUnits, false);
                    if (!fontSizeUnits.empty()
                        && fontSizeUnits != L"points" && fontSizeUnits != L"meters" && fontSizeUnits != L"inches")
                    {
                        // invalid print layout font size units
                        throw new MgdInvalidPrintLayoutFontSizeUnitsException(L"MgdPrintLayout.GetLayoutPropertiesFromXml", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
                pXmlUtil->GetElementValue(textNode, "Value", value, false);
                textInfo.SetX( MgUtil::StringToDouble( positionX ) );
                textInfo.SetY( MgUtil::StringToDouble( positionY ) );
                textInfo.SetPositionUnits( positionUnits );
                textInfo.SetValue( value );
                textInfo.SetFontName( fontName );
                textInfo.SetFontHeight( MgUtil::StringToDouble( fontHeight ) );
                textInfo.SetSizeUnits( fontSizeUnits );

                CustomText().push_back(textInfo);
            }
        }
    }

    // Set MgdPrintLayout values
    INT32 nRed = MgUtil::StringToInt32( szRed );
    INT32 nGreen = MgUtil::StringToInt32( szGreen );
    INT32 nBlue = MgUtil::StringToInt32( szBlue );
    m_bgColor = new MgColor(nRed, nGreen, nBlue, 255);

    ShowTitle() = MgUtil::StringToBoolean( szShowTitle );

    ShowLegend() = MgUtil::StringToBoolean( szShowLegend );

    ShowScalebar() = MgUtil::StringToBoolean( szShowScalebar );

    ShowNorthArrow() = MgUtil::StringToBoolean( szShowNorthArrow );

    ShowUrl() = MgUtil::StringToBoolean( szShowUrl );

    ShowDateTime() = MgUtil::StringToBoolean( szShowDateTime );

    ShowCustomLogos() = MgUtil::StringToBoolean( szShowCustomLogos );

    ShowCustomText() = MgUtil::StringToBoolean( szShowCustomText );

    MG_CATCH_AND_THROW(L"MgdPrintLayout.GetLayoutPropertiesFromXml")
}


// Serialize data to a stream
//
void MgdPrintLayout::Serialize(MgStream* stream)
{
    stream->WriteString(m_plotTitle);
    stream->WriteObject(m_plotSpec);
    stream->WriteString(m_scaleBarUnits);
    stream->WriteDouble(m_dPlotScale);

    stream->WriteObject(m_bgColor);

    stream->WriteBoolean(m_bShowTitle);
    stream->WriteBoolean(m_bShowLegend);
    stream->WriteBoolean(m_bShowScalebar);
    stream->WriteBoolean(m_bShowNorthArrow);
    stream->WriteBoolean(m_bShowUrl);
    stream->WriteBoolean(m_bShowDateTime);
    stream->WriteBoolean(m_bShowCustomLogos);
    stream->WriteBoolean(m_bShowCustomText);

    INT32 customLogosCount = (INT32)m_logos.size();
    stream->WriteInt32(customLogosCount);
    if (customLogosCount > 0)
    {
        for (MgdCustomLogoInfoVector::iterator iter = m_logos.begin();
            iter != m_logos.end(); ++iter)
        {
            MgdCustomLogoInfo logoInfo = (MgdCustomLogoInfo)(*iter);

            stream->WriteDouble(logoInfo.GetX());
            stream->WriteDouble(logoInfo.GetY());
            stream->WriteString(logoInfo.GetPositionUnits());
            stream->WriteDouble(logoInfo.GetWidth());
            stream->WriteDouble(logoInfo.GetHeight());
            stream->WriteString(logoInfo.GetSizeUnits());
            stream->WriteDouble(logoInfo.GetRotation());
            stream->WriteString(logoInfo.GetResourceId());
            stream->WriteString(logoInfo.GetName());
        }
    }

    INT32 customTextCount = (INT32)m_text.size();
    stream->WriteInt32(customTextCount);
    if (customTextCount > 0)
    {
        for (MgdCustomTextInfoVector::iterator iter = m_text.begin();
            iter != m_text.end(); ++iter)
        {
            MgdCustomTextInfo textInfo = (MgdCustomTextInfo)(*iter);

            stream->WriteDouble(textInfo.GetX());
            stream->WriteDouble(textInfo.GetY());
            stream->WriteString(textInfo.GetPositionUnits());
            stream->WriteString(textInfo.GetFontName());
            stream->WriteDouble(textInfo.GetFontHeight());
            stream->WriteString(textInfo.GetSizeUnits());
            stream->WriteString(textInfo.GetValue());
        }
    }
}


// Deserialize data from a stream
//
void MgdPrintLayout::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    streamReader->GetString(m_plotTitle);

    m_plotSpec = (MgdPlotSpecification*)streamReader->GetObject();

    streamReader->GetString(m_scaleBarUnits);

    streamReader->GetDouble(m_dPlotScale);

    m_bgColor = (MgColor*)streamReader->GetObject();

    streamReader->GetBoolean(m_bShowTitle);
    streamReader->GetBoolean(m_bShowLegend);
    streamReader->GetBoolean(m_bShowScalebar);
    streamReader->GetBoolean(m_bShowNorthArrow);
    streamReader->GetBoolean(m_bShowUrl);
    streamReader->GetBoolean(m_bShowDateTime);
    streamReader->GetBoolean(m_bShowCustomLogos);
    streamReader->GetBoolean(m_bShowCustomText);

    INT32 customLogosCount;
    streamReader->GetInt32(customLogosCount);
    for (INT32 i = 0; i < customLogosCount; i++)
    {
        double xCoord;
        streamReader->GetDouble(xCoord);
        double yCoord;
        streamReader->GetDouble(yCoord);
        STRING positionUnits;
        streamReader->GetString(positionUnits);
        double width;
        streamReader->GetDouble(width);
        double height;
        streamReader->GetDouble(height);
        STRING sizeUnits;
        streamReader->GetString(sizeUnits);
        double rotation;
        streamReader->GetDouble(rotation);
        STRING resourceId;
        streamReader->GetString(resourceId);
        STRING name;
        streamReader->GetString(name);

        MgdCustomLogoInfo customLogo;
        customLogo.SetX(xCoord);
        customLogo.SetY(yCoord);
        customLogo.SetPositionUnits(positionUnits);
        customLogo.SetWidth(width);
        customLogo.SetHeight(height);
        customLogo.SetSizeUnits(sizeUnits);
        customLogo.SetRotation(rotation);
        customLogo.SetResourceId(resourceId);
        customLogo.SetName(name);

        m_logos.push_back(customLogo);
    }

    INT32 customTextCount;
    streamReader->GetInt32(customTextCount);
    for (INT32 i = 0; i < customTextCount; i++)
    {
        double xCoord;
        streamReader->GetDouble(xCoord);
        double yCoord;
        streamReader->GetDouble(yCoord);
        STRING positionUnits;
        streamReader->GetString(positionUnits);
        STRING fontName;
        streamReader->GetString(fontName);
        double fontHeight;
        streamReader->GetDouble(fontHeight);
        STRING sizeUnits;
        streamReader->GetString(sizeUnits);
        STRING value;
        streamReader->GetString(value);

        MgdCustomTextInfo customText;
        customText.SetX(xCoord);
        customText.SetY(yCoord);
        customText.SetPositionUnits(positionUnits);
        customText.SetFontName(fontName);
        customText.SetFontHeight(fontHeight);
        customText.SetSizeUnits(sizeUnits);
        customText.SetValue(value);

        m_text.push_back(customText);
    }
}


// Compute the offset and size for the available map area on the page
//
void MgdPrintLayout::ComputeMapOffsetAndSize(double mapScale, MgEnvelope* mapBounds, double metersPerUnit,
                                            double& mapOffsetX, double& mapOffsetY, double& mapWidth, double& mapHeight, bool expandToFit)
{
    double convertUnits = 1.0;
    STRING pageUnits = m_plotSpec->GetPageSizeUnits();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )
    {
        convertUnits = MILLIMETERS_PER_INCH;
    }

    mapOffsetX = m_plotSpec->GetMarginLeft();
    mapOffsetY = m_plotSpec->GetMarginBottom();

    if (expandToFit)
    {
        // The map is scaled to fit the available page area (aspect is not necessarily preserved)
        mapWidth = m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight();
        mapHeight = m_dPageHeight - mapOffsetY - m_plotSpec->GetMarginTop();

        if (m_bShowLegend)
        {
            mapWidth -= (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
            mapOffsetX += (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
        }
        if (m_bShowTitle)
        {
            mapHeight -= MgdPrintLayout::HeaderHeight*convertUnits;
        }
        if (m_bShowScalebar || m_bShowNorthArrow)
        {
            mapHeight -= (MgdPrintLayout::ScalebarHeight + MgdPrintLayout::ScalebarPadding)*convertUnits;
        }
        if (m_bShowUrl || m_bShowDateTime)
        {
            mapHeight -= MgdPrintLayout::FooterHeight*convertUnits;
        }
    }

    else
    {
        // The visible extent of the map is rendered with the current aspect ratio.
        // Clipping of the extents is done as necessary.
        if (mapScale <= 0)
        {
            // The map is scaled to fit the available page area (aspect is not necessarily preserved)
            mapWidth = m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight();
            mapHeight = m_dPageHeight - mapOffsetY - m_plotSpec->GetMarginTop();

            if (m_bShowLegend)
            {
                mapWidth -= (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
                mapOffsetX += (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
            }
            if (m_bShowTitle)
            {
                mapHeight -= MgdPrintLayout::HeaderHeight;
            }
            if (m_bShowScalebar || m_bShowNorthArrow)
            {
                mapHeight -= MgdPrintLayout::ScalebarHeight;
            }
            if (m_bShowUrl || m_bShowDateTime)
            {
                mapHeight -= MgdPrintLayout::FooterHeight;
            }
        }
        else
        {
            // A specific scale is used for the map

            double width = mapBounds->GetWidth();
            double height = mapBounds->GetHeight();
            double mapar = width / height;

            mapHeight = height * metersPerUnit / mapScale;
            mapHeight /= METERS_PER_INCH;

            mapWidth = mapar * mapHeight;

            // mapHeight and mapWidth must be in the same units as the units specified for the page
            mapHeight *= convertUnits;
            mapWidth *= convertUnits;

            // Determine the offset in X direction such that the map is centered horizontally
            if (m_bShowLegend)
            {
                if (mapWidth > (m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight() - (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits))
                {
                    mapWidth = m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight() - (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
                    mapOffsetX += (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits;
                }
                else
                {
                    mapOffsetX += (m_dPageWidth - m_plotSpec->GetMarginLeft() - m_plotSpec->GetMarginRight()
                        - mapWidth + (MgdPrintLayout::LegendWidth + MgdPrintLayout::LegendPadding)*convertUnits) * 0.5;
                }
            }
            else
            {
                if (mapWidth > (m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight() ))
                {
                    mapWidth = m_dPageWidth - mapOffsetX - m_plotSpec->GetMarginRight();
                }
                else
                {
                    //simply center the map on the page
                    mapOffsetX = (m_dPageWidth - mapWidth) * 0.5;
                }
            }

            double dMapHeightAdjustment = -(m_plotSpec->GetMarginTop() + m_plotSpec->GetMarginBottom());
            if (m_bShowTitle)
            {
                dMapHeightAdjustment -= MgdPrintLayout::HeaderHeight*convertUnits;
            }
            if (m_bShowScalebar || m_bShowNorthArrow)
            {
                dMapHeightAdjustment -= MgdPrintLayout::ScalebarHeight*convertUnits;
            }
            if (m_bShowUrl || m_bShowDateTime)
            {
                dMapHeightAdjustment -= MgdPrintLayout::FooterHeight*convertUnits;
            }

            if (mapHeight > m_dPageHeight + dMapHeightAdjustment)
            {
                mapHeight = m_dPageHeight + dMapHeightAdjustment;
            }
        }
    }

    //finally center map vertically using updated map height
    if (m_bShowTitle)
    {
        mapOffsetY = (m_dPageHeight - MgdPrintLayout::HeaderHeight*convertUnits - mapHeight) * 0.5;
    }
    if (m_bShowScalebar || m_bShowNorthArrow || m_bShowUrl || m_bShowDateTime)
    {
        mapOffsetY = (m_dPageHeight - mapHeight) * 0.5;
    }
}


// Determine the map extents which will make maximum use of available page space.
// The extents will be based on the map center and scale, and the size of available map area.
//
MgEnvelope* MgdPrintLayout::DetermineLayoutMapExtents(MgdMap* map, double metersPerUnit, double mapWidth, double mapHeight)
{
    // Compute the aspect ratio of the available map area
    double pageAR = mapWidth / mapHeight;

    // Compute new bounds (extents) that will best fit the available area on the page
    double mapBoundsHeight = (mapHeight * m_dPlotScale * METERS_PER_INCH) / metersPerUnit;
    double mapBoundsWidth = mapBoundsHeight * pageAR;

    // Determine the actual layout bounds using the map center and the mapBounds height and width
    //...lower left extent
    double minx = m_plotCenter->GetX() - 0.5*mapBoundsWidth;
    double miny = m_plotCenter->GetY() - 0.5*mapBoundsHeight;
    //...upper right extent
    double maxx = m_plotCenter->GetX() + 0.5*mapBoundsWidth;
    double maxy = m_plotCenter->GetY() + 0.5*mapBoundsHeight;

    Ptr<MgCoordinateXY> newll = new MgCoordinateXY(minx, miny);
    Ptr<MgCoordinateXY> newur = new MgCoordinateXY(maxx, maxy);

    return new MgEnvelope(newll, newur);
}
