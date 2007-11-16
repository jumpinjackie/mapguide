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

#ifndef STYLIZATIONENGINE_H_
#define STYLIZATIONENGINE_H_

#include "Stylizer.h"
#include "SE_Matrix.h"

class SE_SymbolManager;
class RS_FeatureReader;
class SE_LineBuffer;
class SE_BufferPool;
class SE_Renderer;
class Renderer;
class SE_StyleVisitor;
class LineBuffer;
class LineBufferPool;
class RS_ElevationSettings;

namespace MDFMODEL_NAMESPACE
{
    class CompositeTypeStyle;
}

struct SE_Style;
struct SE_PointStyle;
struct SE_LineStyle;
struct SE_AreaStyle;
struct SE_Rule;
struct SE_String;
struct SE_RenderPointStyle;
struct SE_RenderLineStyle;
struct SE_RenderAreaStyle;
struct SE_RenderStyle;

using namespace MDFMODEL_NAMESPACE;

class StylizationEngine
{
public:
    StylizationEngine(SE_SymbolManager* resources);
    ~StylizationEngine();

    // Stylizes the supplied layer using all composite type styles in the given scale.
    void StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                            MdfModel::VectorScaleRange*      range,
                            Renderer*                        renderer,
                            RS_FeatureReader*                reader,
                            FdoExpressionEngine*             exec,
                            CSysTransformer*                 xformer,
                            CancelStylization                cancel,
                            void*                            userData);

    // Stylizes the current feature on the reader using the supplied composite type style.
    void Stylize(RS_FeatureReader* reader,
                 FdoExpressionEngine* exec,
                 LineBuffer* geometry,
                 CompositeTypeStyle* style,
                 SE_String* seTip,
                 SE_String* seUrl,
                 RS_ElevationSettings* elevSettings,
                 int instanceRenderingPass,
                 int symbolRenderingPass,
                 int& nextInstanceRenderingPass,
                 int& nextSymbolRenderingPass);

    void ClearCache();

private:
    void LayoutCustomLabel(const wchar_t* positioningAlgo, LineBuffer* geometry, SE_Matrix& xform, SE_RenderStyle* rstyle, double mm2px);

private:
    Renderer* m_renderer;
    SE_Renderer* m_serenderer;
    SE_SymbolManager* m_resources;
    SE_BufferPool* m_pool;
    SE_StyleVisitor* m_visitor;
    std::map<CompositeTypeStyle*, SE_Rule*> m_rules;
    RS_FeatureReader* m_reader;
};

#endif
