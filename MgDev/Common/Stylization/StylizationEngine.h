//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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


// forward declare
class SE_Renderer;
class SE_SymbolManager;
class SE_LineBuffer;
class SE_BufferPool;
class SE_StyleVisitor;
class SE_ApplyContext;
class RS_FeatureReader;
class RS_ElevationSettings;
class LineBuffer;
class LineBufferPool;

namespace MDFMODEL_NAMESPACE
{
    class CompositeTypeStyle;
}

struct SE_SymbolInstance;
struct SE_Style;
struct SE_PointStyle;
struct SE_LineStyle;
struct SE_AreaStyle;
struct SE_Rule;
struct SE_String;
struct SE_RenderStyle;
struct SE_RenderPointStyle;
struct SE_RenderLineStyle;
struct SE_RenderAreaStyle;

using namespace MDFMODEL_NAMESPACE;


class StylizationEngine
{
public:
    StylizationEngine(SE_SymbolManager* resources, SE_BufferPool* pool);
    ~StylizationEngine();

    // Stylizes the supplied layer using all composite type styles in the given scale.
    void StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                            MdfModel::VectorScaleRange*      range,
                            SE_Renderer*                     se_renderer,
                            RS_FeatureReader*                reader,
                            CSysTransformer*                 xformer,
                            CancelStylization                cancel,
                            void*                            userData);

    //Stylize the supplied watermark
    void StylizeWatermark(SE_Renderer* se_renderer,
                          WatermarkDefinition* watermark,
                          int drawWidth,
                          int drawHeight,
                          int saveWidth,
                          int saveHeight);

    // Stylizes the current feature on the reader using the supplied composite type style.
    void Stylize(RS_FeatureReader* reader,
                 FdoExpressionEngine* exec,
                 LineBuffer* geometry,
                 CompositeTypeStyle* style,
                 SE_String* seTip,
                 SE_String* seUrl,
                 RS_ElevationSettings* elevSettings,
                 bool initialPass,
                 int instanceRenderingPass,
                 int symbolRenderingPass,
                 int& nextInstanceRenderingPass,
                 int& nextSymbolRenderingPass);

    void ClearCache();

private:
    void LayoutCustomLabel(const wchar_t* positioningAlgo, SE_ApplyContext* applyCtx, SE_RenderStyle* rstyle, double mm2su);
    double GetClipOffset(SE_SymbolInstance* sym, SE_Style* style, FdoExpressionEngine* exec, double mm2suX, double mm2suY);

private:
    SE_Renderer* m_serenderer;
    SE_SymbolManager* m_resources;
    SE_BufferPool* m_pool;
    SE_StyleVisitor* m_visitor;
    std::map<CompositeTypeStyle*, SE_Rule*> m_rules;
    RS_FeatureReader* m_reader;
};

#endif
