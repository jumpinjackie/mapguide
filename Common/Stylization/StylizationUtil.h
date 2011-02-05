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

#ifndef STYLEPREVIEW_H_
#define STYLEPREVIEW_H_

#include "Stylization.h"
#include "RendererStyles.h"
#include "Bounds.h"
using namespace MdfModel;

// fwd declare
class Renderer;
class SE_Renderer;
class SE_SymbolManager;
struct SE_SymbolInstance;

// Provides helper methods for:
// - parsing of values from strings
// - drawing image previews of styles
class STYLIZATION_API StylizationUtil
{
public:
    static void ParseColor(const MdfString& scolor, RS_Color& rscol);
    static bool ParseDouble(const MdfString& valstr, double& val);

    static void DrawStylePreview(int imgWidth, int imgHeight, int themeCategory, FeatureTypeStyle* fts,
                                 SE_Renderer* pSERenderer, SE_SymbolManager* sman);

    static void RenderPointSymbolization(PointSymbolization2D* psym,
                                         SE_Renderer* pSERenderer,
                                         double x, double y,
                                         double width, double height);

    static void RenderLineSymbolization(LineSymbolization2D* lsym,
                                        SE_Renderer* pSERenderer,
                                        double x, double y,
                                        double width, double height,
                                        double maxLineWidth);

    static void RenderAreaSymbolization(AreaSymbolization2D* asym,
                                        SE_Renderer* pSERenderer,
                                        double x, double y,
                                        double width, double height);

    static void RenderCompositeSymbolization(CompositeSymbolization* csym,
                                             SE_Renderer* pSERenderer,
                                             SE_SymbolManager* sman,
                                             double x, double y,
                                             double width, double height);

    static RS_Bounds GetCompositeSymbolizationBounds(CompositeSymbolization* csym,
                                                     SE_Renderer* pSERenderer,
                                                     SE_SymbolManager* sman);

    static double GetMaxMappingSpaceLineWidth(FeatureTypeStyle* fts, int themeCategory);

private:
    static void GetCompositeSymbolizationBoundsInternal(std::vector<SE_SymbolInstance*> symbolInstances,
                                                        SE_Renderer* pSERenderer,
                                                        SE_SymbolManager* sman,
                                                        FdoExpressionEngine* exec,
                                                        RS_Bounds& bounds);

    static void GetCompositeSymbolizationPreviewBounds(std::vector<SE_SymbolInstance*> symbolInstances,
                                                       SE_Renderer* pSERenderer,
                                                       SE_SymbolManager* sman,
                                                       FdoExpressionEngine* exec,
                                                       RS_Bounds& bounds);
};

#endif
