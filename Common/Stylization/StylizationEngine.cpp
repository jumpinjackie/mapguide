//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#include "stdafx.h"
#include "StylizationEngine.h"
#include "SE_BufferPool.h"
#include "SE_StyleVisitor.h"
#include "SE_Renderer.h"
#include "SE_SymbolManager.h"
#include "SE_PositioningAlgorithms.h"
#include "FeatureTypeStyleVisitor.h"
#include "SE_SymbolDefProxies.h"

#include <algorithm>
#include <functional>

using namespace MDFMODEL_NAMESPACE;

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

StylizationEngine::StylizationEngine(SE_SymbolManager* resources) :
    m_resources(resources),
    m_serenderer(NULL),
    m_reader(NULL)
{
    m_pool = new SE_BufferPool;
    m_visitor = new SE_StyleVisitor(resources, m_pool);
}


StylizationEngine::~StylizationEngine()
{
    ClearCache();
    delete m_pool;
    delete m_visitor;
}


// TODO: Stylize one CompoundSymbol feature and label per run; investigate caching
//       possibilities to avoid filter execution on subsequent passes
void StylizationEngine::StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                                           MdfModel::VectorScaleRange*      range,
                                           SE_Renderer*                     se_renderer,
                                           RS_FeatureReader*                reader,
                                           CSysTransformer*                 xformer,
                                           CancelStylization                cancel,
                                           void*                            userData)
{
    if (reader == NULL)
        return;

    m_serenderer = se_renderer;
    m_reader = reader;

    // get the geometry column name
    const wchar_t* gpName = reader->GetGeomPropName();
    if (NULL == gpName)
        return;

    m_serenderer->SetBufferPool(m_pool);

    // For now always clip using the new stylization - the performance impact of not
    // clipping is too high.  We also need a better approach to clipping.  Instead
    // of clipping the feature geometry we need to calculate where to start/stop
    // drawing symbols.
    bool bClip = true;  //m_serenderer->RequiresClipping();

    double drawingScale = m_serenderer->GetDrawingScale();

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    m_visitor->ParseStringExpression(layer->GetToolTip(), seTip, L"");
    m_visitor->ParseStringExpression(layer->GetUrl(), seUrl, L"");

    // extract all the composite styles once
    MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();
    std::vector<CompositeTypeStyle*> compTypeStyles;
    for (int i=0; i<ftsc->GetCount(); ++i)
    {
        MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(i);
        if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts) == FeatureTypeStyleVisitor::ftsComposite)
            compTypeStyles.push_back((CompositeTypeStyle*)fts);
    }

    size_t numTypeStyles = compTypeStyles.size();
    _ASSERT(numTypeStyles > 0);
    if (numTypeStyles == 0)
        return;

    // we always start with rendering pass 0
    int instanceRenderingPass = 0;
    int symbolRenderingPass = 0;
    int nextInstanceRenderingPass = -1;
    int nextSymbolRenderingPass = -1;

    #ifdef _DEBUG
    int nFeatures = 0;
    #endif

    // main loop over feature data
    int numPasses = 0;
    while (instanceRenderingPass >= 0 && symbolRenderingPass >= 0)
    {
        ++numPasses;

        // for all but the first pass we need to reset the reader
        if (numPasses > 1)
            reader->Reset();

        // create an expression engine with our custom functions
        // NOTE: We must create a new engine with each rendering pass.  The engine
        //       stores a weak reference to the RS_FeatureReader's internal
        //       FdoIFeatureReader, and this internal reader is different for each
        //       pass.
        FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(se_renderer, reader);

        while (reader->ReadNext())
        {
            #ifdef _DEBUG
            if (numPasses == 1)
                nFeatures++;
            #endif

            LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pool, 8, FdoDimensionality_Z, false);
            if (!lb)
                continue;

            // tell line buffer the current drawing scale (used for arc tessellation)
            lb->SetDrawingScale(drawingScale);

            try
            {
                if (!reader->IsNull(gpName))
                    reader->GetGeometry(gpName, lb, xformer);
            }
            catch (FdoException* e)
            {
                // just move on to the next feature
                e->Release();
                LineBufferPool::FreeLineBuffer(m_pool, lb);
                continue;
            }

            if (bClip)
            {
                // clip geometry to given map request extents
                LineBuffer* lbc = lb->Clip(m_serenderer->GetBounds(), LineBuffer::ctAGF, m_pool);

                // Free original line buffer if the geometry was actually clipped.
                // Note that the original geometry is still accessible using
                // RS_FeatureReader::GetGeometry.
                if (lbc != lb)
                {
                    LineBufferPool::FreeLineBuffer(m_pool, lb);

                    // if the clipped buffer is NULL just move on to the next feature
                    if (!lbc)
                        continue;

                    // otherwise continue processing with the clipped buffer
                    lb = lbc;
                }
            }

            // don't bother rendering feature geometry that has been completely clipped
            if (lb->point_count())
            {
                // stylize once for each composite type style
                for (size_t i=0; i<numTypeStyles; ++i)
                {
                    Stylize(reader, exec, lb, compTypeStyles[i], &seTip, &seUrl, NULL,
                            instanceRenderingPass, symbolRenderingPass,
                            nextInstanceRenderingPass, nextSymbolRenderingPass);
                }
            }

            // free geometry when done stylizing
            LineBufferPool::FreeLineBuffer(m_pool, lb);

            if (cancel && cancel(userData))
                break;
        }

        if (nextSymbolRenderingPass == -1)
        {
            // no more symbol rendering passes for the current instance
            // rendering pass - switch to the next instance rendering pass
            instanceRenderingPass = nextInstanceRenderingPass;
            nextInstanceRenderingPass = -1;

            // also reset the symbol rendering pass
            symbolRenderingPass = 0;
        }
        else
        {
            // switch to the next symbol rendering pass
            symbolRenderingPass = nextSymbolRenderingPass;
            nextSymbolRenderingPass = -1;
        }
    }

    #ifdef _DEBUG
    printf("  StylizationEngine::StylizeVectorLayer() Layer: %S  Features: %d\n", layer->GetFeatureName().c_str(), nFeatures);
    #endif
}


void StylizationEngine::Stylize(RS_FeatureReader* reader,
                                FdoExpressionEngine* exec,
                                LineBuffer* geometry,
                                CompositeTypeStyle* style,
                                SE_String* seTip,
                                SE_String* seUrl,
                                RS_ElevationSettings* /*elevSettings*/,
                                int instanceRenderingPass,
                                int symbolRenderingPass,
                                int& nextInstanceRenderingPass,
                                int& nextSymbolRenderingPass)
{
    m_reader = reader;

    SE_Rule*& rules = m_rules[style];
    RuleCollection* rulecoll = style->GetRules();
    int nRules = rulecoll->GetCount();

    if (rules == NULL)
    {
        SE_Rule* rulecache = new SE_Rule[nRules];
        rules = rulecache;

        for (int i=0; i<nRules; ++i)
        {
            CompositeRule* r = static_cast<CompositeRule*>(rulecoll->GetAt(i));
            const MdfString& filterstr = r->GetFilter();
            rulecache[i].filter = NULL;
            if (!filterstr.empty())
            {
                try
                {
                    rulecache[i].filter = FdoFilter::Parse(filterstr.c_str());
                }
                catch (FdoException* e)
                {
                    ProcessStylizerException(e, __LINE__, __WFILE__);
                }
            }

            rulecache[i].legendLabel= r->GetLegendLabel();

            m_visitor->Convert(rulecache[i].symbolInstances, r->GetSymbolization());
        }
    }

    SE_Rule* rule = NULL;
    for (int i=0; i<nRules; ++i)
    {
        bool match = (rules[i].filter == NULL);

        if (!match)
        {
            try
            {
                match = exec->ProcessFilter(rules[i].filter);
            }
            catch (FdoException* e)
            {
                ProcessStylizerException(e, __LINE__, __WFILE__);
            }
        }

        if (match)
        {
            rule = &rules[i];
            break;
        }
    }

    if (rule == NULL)
        return;

    std::vector<SE_SymbolInstance*>* symbolInstances = &rule->symbolInstances;

    bool initialPass = (instanceRenderingPass == 0 && symbolRenderingPass == 0);
    RS_String rs_tip, rs_url;
    if (seTip->expression || wcslen(seTip->getValue()) > 0)
        rs_tip = seTip->evaluate(exec);
    if (seUrl->expression || wcslen(seUrl->getValue()) > 0)
        rs_url = seUrl->evaluate(exec);
    RS_String& rs_thm = rule->legendLabel;
    m_serenderer->StartFeature(reader, initialPass, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);

    // it's possible to end up with no symbols - we're done in that case
    if (symbolInstances->size() == 0)
        return;

    double mm2sud = m_serenderer->GetScreenUnitsPerMillimeterDevice();
    double mm2suw = m_serenderer->GetScreenUnitsPerMillimeterWorld();
    bool yUp = m_serenderer->YPointsUp();

    // Here's a description of how the transforms work for point / line / area symbols.
    //
    // =============
    // Point Symbols
    // =============
    //
    // For point symbols we have the following transform stack:
    //
    //   [T_fe] [S_mm] [T_si] [R_pu] [S_si] [T_pu] {Geom}
    //
    // where:
    //   T_pu = point usage origin offset (a translation)
    //   S_si = symbol instance scaling
    //   R_pu = point usage rotation
    //   T_si = symbol instance insertion offset
    //   S_mm = scaling converting mm to screen units (also includes inverting y, if necessary)
    //   T_fe = translation to the point feature
    //
    // This can be rewritten as:
    //
    //   [T_fe] [T_si*] [R_pu*] [T_pu*] [S_mm] [S_si] {Geom}
    //
    // where:
    //   T_si* = symbol instance insertion offset, using offsets scaled by S_mm
    //   R_pu* = point usage rotation, with angle accounting for y-up or y-down
    //   T_pu* = point usage origin offset, using offsets scaled by S_mm and S_si
    //
    // We store [S_mm] [S_si] in xformScale below, and apply it to the symbol geometry
    // during symbol evaluation.  The remaining transforms get applied in SE_Renderer::
    // ProcessPoint.
    //
    // ============
    // Line Symbols
    // ============
    //
    // For line symbols it's simple since the symbol instance insertion offset doesn't
    // apply, nor is there a line usage offset.  The stack looks like:
    //
    //   [T_fe] [S_mm] [R_lu] [S_si] {Geom}
    //
    // where:
    //   S_si = symbol instance scaling
    //   R_lu = line usage rotation
    //   S_mm = scaling converting mm to screen units (also includes inverting y, if necessary)
    //   T_fe = translation along the line feature
    //
    // This is rewritten as:
    //
    //   [T_fe] [R_lu*] [S_mm] [S_si] {Geom}
    //
    // where:
    //   R_lu* = line usage rotation, with angle accounting for y-up or y-down
    //
    // As with point symbols we apply [S_mm] and S_si] to the symbol during evaluation,
    // and the remaining transforms get applied in SE_Renderer::ProcessLine.
    //
    // ============
    // Area Symbols
    // ============
    //
    // Area symbols work the same as line symbols.  The stack looks like:
    //
    //   [T_fe] [S_mm] [R_au] [S_si] {Geom}
    //
    // where:
    //   S_si = symbol instance scaling
    //   R_au = area usage rotation
    //   S_mm = scaling converting mm to screen units (also includes inverting y, if necessary)
    //   T_fe = translation within the area feature
    //
    // This is rewritten as:
    //
    //   [T_fe] [R_au*] [S_mm] [S_si] {Geom}
    //
    // where:
    //   R_lu* = area usage rotation, with angle accounting for y-up or y-down
    //
    // As with point symbols we apply [S_mm] and S_si] to the symbol during evaluation,
    // and the remaining transforms get applied in SE_Renderer::ProcessArea.

    // TODO: Obey the indices - get rid of the indices altogther - single pass!

    SE_SymbolInstance* sym = NULL;
    size_t nSyms = symbolInstances->size();
    for (size_t symIx=0; symIx<nSyms; ++symIx)
    {
        sym = (*symbolInstances)[symIx];

        // process the instance rendering pass - negative rendering passes are
        // rendered with pass 0
        int instanceRenderPass = sym->renderPass.evaluate(exec);
        if (instanceRenderPass < 0)
            instanceRenderPass = 0;

        // If the rendering pass for the instance doesn't match the current
        // instance pass then don't render using it.
        if (instanceRenderPass != instanceRenderingPass)
        {
            // if the instance's rendering pass is greater than the current
            // instance pass, then update nextInstanceRenderingPass to account
            // for it
            if (instanceRenderPass > instanceRenderingPass)
            {
                // update nextInstanceRenderingPass if it hasn't yet been set,
                // or if the instance's pass is less than the current next pass
                if (nextInstanceRenderingPass == -1 || instanceRenderPass < nextInstanceRenderingPass)
                    nextInstanceRenderingPass = instanceRenderPass;
            }

            continue;
        }

        // enforce the geometry context
        if (sym->geomContext != SymbolInstance::gcUnspecified)
        {
            switch (geometry->geom_type())
            {
            case FdoGeometryType_Point:
            case FdoGeometryType_MultiPoint:
                if (sym->geomContext != SymbolInstance::gcPoint)
                    continue;
                break;

            case FdoGeometryType_LineString:
            case FdoGeometryType_MultiLineString:
            case FdoGeometryType_CurveString:
            case FdoGeometryType_MultiCurveString:
                if (sym->geomContext != SymbolInstance::gcLineString)
                    continue;
                break;

            case FdoGeometryType_Polygon:
            case FdoGeometryType_MultiPolygon:
            case FdoGeometryType_CurvePolygon:
            case FdoGeometryType_MultiCurvePolygon:
                if (sym->geomContext != SymbolInstance::gcPolygon)
                    continue;
                break;

//          case FdoGeometryType_MultiGeometry:
//              continue;
//              break;
            }
        }

        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));

        // The symbol geometry needs to be inverted if the y coordinate in the renderer points down.
        // This is so that in symbol definitions y points up consistently no matter what the underlying
        // renderer is doing.  Normally we could just apply the world to screen transform to everything,
        // but in some cases we only apply it to the position of the symbol and then offset the symbol
        // geometry from there - so the symbol geometry needs to be pre-inverted.
        double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
        double mm2suY = yUp? mm2suX : -mm2suX;
        xformScale.scale(mm2suX, mm2suY);

        // initialize the style evaluation context
        SE_EvalContext evalCtx;
        evalCtx.exec = exec;
        evalCtx.mm2su = mm2suX;
        evalCtx.mm2sud = mm2sud;
        evalCtx.mm2suw = mm2suw;
        evalCtx.px2su = m_serenderer->GetScreenUnitsPerPixel();
        evalCtx.pool = m_pool;
        evalCtx.fonte = m_serenderer->GetRSFontEngine();
        evalCtx.xform = &xformScale;
        evalCtx.resources = m_resources;

        // initialize the style application context
        SE_Matrix xformTrans;
        xformTrans.translate(sym->absOffset[0].evaluate(exec) * mm2suX,
                             sym->absOffset[1].evaluate(exec) * mm2suY);

        SE_ApplyContext applyCtx;
        applyCtx.geometry = geometry;
        applyCtx.renderer = m_serenderer;
        applyCtx.xform = &xformTrans;

        size_t nStyles = sym->styles.size();
        for (size_t styIx=0; styIx<nStyles; ++styIx)
        {
            SE_Style* style = sym->styles[styIx];

            // process the symbol rendering pass - negative rendering passes are
            // rendered with pass 0
            int symbolRenderPass = style->renderPass.evaluate(exec);
            if (symbolRenderPass < 0)
                symbolRenderPass = 0;

            // If the rendering pass for the style doesn't match the current pass
            // then don't render using it.
            if (symbolRenderPass != symbolRenderingPass)
            {
                // if the style's rendering pass is greater than the current pass,
                // then update nextRenderingPass to account for it
                if (symbolRenderPass > symbolRenderingPass)
                {
                    // update nextRenderingPass if it hasn't yet been set, or if
                    // the style's pass is less than the current next pass
                    if (nextSymbolRenderingPass == -1 || symbolRenderPass < nextSymbolRenderingPass)
                        nextSymbolRenderingPass = symbolRenderPass;
                }

                continue;
            }

            // evaluate the style (all expressions inside it) and convert to a
            // constant screen space render style
            style->evaluate(&evalCtx);

            // TODO: why are these in the symbol instance?
            style->rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(exec);
            style->rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(exec);
            style->rstyle->drawLast = sym->drawLast.evaluate(exec);

            const wchar_t* positioningAlgo = sym->positioningAlgorithm.evaluate(exec);
            if (wcslen(positioningAlgo) > 0)
            {
                LayoutCustomLabel(positioningAlgo, &applyCtx, style->rstyle, mm2suX);
            }
            else
            {
                // apply the style to the geometry using the renderer
                style->apply(&applyCtx);
            }
        }
    }
}


void StylizationEngine::LayoutCustomLabel(const wchar_t* positioningAlgo, SE_ApplyContext* applyCtx, SE_RenderStyle* rstyle, double mm2su)
{
    // call the appropriate positioning algorithm based on the name
    if (wcscmp(positioningAlgo, L"EightSurrounding") == 0)
    {
        SE_PositioningAlgorithms::EightSurrounding(applyCtx, rstyle, mm2su);
    }
    else if (wcscmp(positioningAlgo, L"PathLabels") == 0)
    {
        SE_PositioningAlgorithms::PathLabels(applyCtx, rstyle);
    }
    else if (wcscmp(positioningAlgo, L"MultipleHighwayShields") == 0)
    {
        SE_PositioningAlgorithms::MultipleHighwaysShields(applyCtx, rstyle, mm2su, m_reader, m_resources);
    }
    else if (wcscmp(positioningAlgo, L"Default") == 0)
    {
        SE_PositioningAlgorithms::Default(applyCtx, rstyle);
    }
}


//clears cached filters/styles/etc
void StylizationEngine::ClearCache()
{
    std::map<CompositeTypeStyle*, SE_Rule*>::iterator iter = m_rules.begin();

    for (; iter != m_rules.end(); iter++)
        delete [] iter->second;

    m_rules.clear();
}
