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

#ifndef STYLEPREVIEW_H_
#define STYLEPREVIEW_H_

#include "Stylization.h"
#include "RendererStyles.h"
using namespace MdfModel;

// fwd declare
class Renderer;
class SE_Renderer;
class SE_SymbolManager;

// Provides helper methods for:
// - parsing of values from strings
// - drawing image previews of styles
class STYLIZATION_API StylizationUtil
{
public:
    static void ParseColor(const MdfString& scolor, RS_Color& rscol);
    static bool ParseDouble(const MdfString& valstr, double& val);

    static void DrawStylePreview(int imgWidth, int imgHeight, int themeCategory, FeatureTypeStyle* fts,
                                 Renderer* renderer, SE_Renderer* se_renderer, SE_SymbolManager* sman);

    static void RenderPointSymbolization(PointSymbolization2D* psym,
                                         Renderer* renderer,
                                         double width, double height, double dpi);

    static void RenderLineSymbolization(LineSymbolization2D* lsym,
                                        Renderer* renderer,
                                        double width, double height, double dpi,
                                        double maxLineWidth);

    static void RenderAreaSymbolization(AreaSymbolization2D* asym,
                                        Renderer* renderer,
                                        double width, double height, double dpi);

    static void RenderCompositeSymbolization(CompositeSymbolization* csym,
                                             Renderer* renderer,
                                             SE_Renderer* se_renderer,
                                             SE_SymbolManager* sman);

    static double GetMaxMappingSpaceLineWidth(FeatureTypeStyle* fts, int themeCategory);
};

#endif
