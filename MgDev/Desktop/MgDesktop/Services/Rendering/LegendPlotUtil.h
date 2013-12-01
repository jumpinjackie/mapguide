#ifndef DESKTOP_LEGEND_PLOT_UTIL_H
#define DESKTOP_LEGEND_PLOT_UTIL_H

class EPlotRenderer;

typedef std::vector<STRING> LayerGroupList;
typedef std::map<STRING, LayerGroupList> LayerGroupChildMap;
typedef std::map<STRING, int> VisibleLayerCountMap;

class MG_DESKTOP_API MgdLegendPlotUtil
{
public:

    MgdLegendPlotUtil(MgResourceService* svcResource);
    virtual ~MgdLegendPlotUtil();

    void AddTitleElement(MgdPrintLayout* layout, STRING& mapName, EPlotRenderer& dr);
    void AddScalebarElement(MgdPrintLayout* layout, RS_Bounds& mapBounds, double dMapScale, double dMetersPerMapUnit, EPlotRenderer& dr, RS_LineStroke& lineStroke);
    void AddNorthArrowElement(MgdPrintLayout* layout, EPlotRenderer& dr, RS_LineStroke& lineStroke);
    void AddUrlElement(MgdPrintLayout* layout, STRING& mapURL, EPlotRenderer& dr);
    void AddDateTimeElement(MgdPrintLayout* layout, EPlotRenderer& dr);
    void AddCustomLogoElement(MgdPrintLayout* layout, EPlotRenderer& dr);
    void AddCustomTextElement(MgdPrintLayout* layout, EPlotRenderer& dr);

    void AddLayoutElements(MgdPrintLayout* layout, STRING mapName, STRING mapURL, MgdMap* map, MgLayerCollection* layers, RS_Bounds& mapBounds, double& dMapScale, double& dMetersPerMapUnit, EPlotRenderer& dr);
    void ComputeLegendOffsetAndSize(MgdPrintLayout* layout, double mapScale, EPlotRenderer& dr, MgdMap* map, double &legendOffsetX, double& legendOffsetY, double& legendWidth, double& legendHeight);
    
    //These have been cleaned up to work with a generic renderer, rather than just EPlot.
    //Eventually the scale bar and north arrow ones need to be cleaned up also in order
    //to use them in the AJAX viewer
    void AddLegendElement(double dMapScale, Renderer& dr, MgdMap* map, MgdPlotSpecification* plotSpec, double legendOffsetX, double legendOffsetY);
    void ProcessLayersForLegend(MgdMap* map, double mapScale, MgLayerGroup* mggroup, double startX, double& startY, RS_TextDef textDef, Renderer& dr, MgdPlotSpecification* plotSpec, double legendOffsetY, double convertUnits, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren);
    void BuildLegendContent(MgdMap* map, double scale, MgdPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY, Renderer& dr, double convertUnits);

    void ExtentFromMapCenter(MgdMap* map, double metersPerUnit, RS_Bounds& b);
    void CompileInformation(MgdMap* map, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren);

private:
    static bool HasVisibleLayers(CREFSTRING groupName, VisibleLayerCountMap& visibleLayers, LayerGroupChildMap& groupChildren);

    MgResourceService* m_svcResource;
    RS_String m_legendFontName;
};

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
const double legendFontHeightMeters = 0.003;
const double legendTextVertAdjust = 0.06;  // inch

const double PrintLegendWidth     = 2.0;
const double PrintLegendPadding   = 0.1;
const double PrintHeaderHeight    = 1.0;
const double PrintScalebarHeight  = 0.75;
const double PrintScalebarPadding = 0.5;
const double PrintFooterHeight    = 0.5;
const double PrintGroupIndent     = 0.3;

#endif