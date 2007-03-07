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

#ifndef STYLIZATION_ENGINE_H
#define STYLIZATION_ENGINE_H

#include "SE_Matrix.h"
#include "SE_Include.h"

class SE_SymbolManager;
class RS_FeatureReader;
class RS_FilterExecutor;
class SE_LineBuffer;
class SE_LineBufferPool;
class SE_Renderer;
class SE_StyleVisitor;
class LineBuffer;

namespace MDFMODEL_NAMESPACE
{
    class CompositeTypeStyle;
}

using namespace MDFMODEL_NAMESPACE;

class StylizationEngine
{
public:
    StylizationEngine(SE_SymbolManager* resources);
    /* TODO: don't leak basically everything */
    ~StylizationEngine();
    /* TODO: add stylize layer to this class, and reset functionality to RS_FeatureReader API & implementations
     * Stylizize one CompoundSymbol feature and label per run; investigate caching possiblities to avoid
     * filter execution on subsequent passes */
    void Stylize(SE_Renderer* renderer,
                 RS_FeatureReader* feature,
                 RS_FilterExecutor* executor,
                 LineBuffer* geometry,
                 CompositeTypeStyle* style);

    void ClearCache();

private:
    SE_RenderPointStyle* EvaluatePointStyle(SE_LineBuffer* geometry, SE_Matrix& xform, SE_PointStyle* style, double mm2px);
    SE_RenderAreaStyle* EvaluateAreaStyle(SE_Matrix& xform, SE_AreaStyle* style);
    SE_RenderLineStyle* EvaluateLineStyle(SE_Matrix& xform, SE_LineStyle* style);

    void LayoutCustomLabel(SE_LineBuffer* geometry, SE_Matrix& xform, SE_Style* style, SE_RenderStyle* rstyle, double mm2px);
    void EvaluateSymbols(SE_Matrix& xform, SE_Style* style, SE_RenderStyle* renderStyle, double mm2px);
    SE_Renderer* m_renderer;
    RS_FilterExecutor* m_exec;
    RS_FeatureReader* m_reader;
    SE_Matrix m_w2s;
    double m_mm2pxs;
    double m_mm2pxw;
    SE_SymbolManager* m_resources;
    SE_LineBufferPool* m_pool;
    SE_StyleVisitor* m_visitor;
    std::map<CompositeTypeStyle*, SE_Rule*> m_rules;
};

#endif // STYLIZATION_ENGINE_H
