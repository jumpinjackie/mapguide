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

#ifndef LEGENDPLOTUTIL_H
#define LEGENDPLOTUTIL_H

class EPlotRenderer;

class MG_SERVER_MAPPING_API MgLegendPlotUtil
{
public:

    MgLegendPlotUtil(MgResourceService* svcResource);
    virtual ~MgLegendPlotUtil();

    void AddTitleElement(MgPrintLayout* layout, STRING& mapName, EPlotRenderer& dr);
    void AddScalebarElement(MgPrintLayout* layout, RS_Bounds& mapBounds, double dMapScale, double dMetersPerMapUnit, EPlotRenderer& dr, RS_LineStroke& lineStroke);
    void AddNorthArrowElement(MgPrintLayout* layout, EPlotRenderer& dr, RS_LineStroke& lineStroke);
    void AddUrlElement(MgPrintLayout* layout, STRING& mapURL, EPlotRenderer& dr);
    void AddDateTimeElement(MgPrintLayout* layout, EPlotRenderer& dr);
    void AddCustomLogoElement(MgPrintLayout* layout, EPlotRenderer& dr);
    void AddCustomTextElement(MgPrintLayout* layout, EPlotRenderer& dr);

    void AddLayoutElements(MgPrintLayout* layout, STRING mapName, STRING mapURL, MgMap* map, MgLayerCollection* layers, RS_Bounds& mapBounds, double& dMapScale, double& dMetersPerMapUnit, EPlotRenderer& dr);
    void ComputeLegendOffsetAndSize(MgPrintLayout* layout, double mapScale, EPlotRenderer& dr, MgMap* map, double &legendOffsetX, double& legendOffsetY, double& legendWidth, double& legendHeight);

    //These have been cleaned up to work with a generic renderer, rather than just EPlot.
    //Eventually the scale bar and north arrow ones need to be cleaned up also in order
    //to use them in the AJAX viewer
    void AddLegendElement(double dMapScale, Renderer& dr, MgMap* map, MgPlotSpecification* plotSpec, double legendOffsetX, double legendOffsetY);
    void ProcessLayersForLegend(MgMap* map, double mapScale, MgLayerGroup* mggroup, double startX, double& startY, RS_TextDef textDef, Renderer& dr, MgPlotSpecification* plotSpec, double legendOffsetY, double convertUnits);
    void BuildLegendContent(MgMap* map, double scale, MgPlotSpecification* legendSpec, double legendOffsetX, double legendOffsetY, Renderer& dr, double convertUnits);

    void ExtentFromMapCenter(MgMap* map, double metersPerUnit, RS_Bounds& b);

private:

    MgResourceService* m_svcResource;
    RS_String m_legendFontName;
};

#endif
