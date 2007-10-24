//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "SE_LineBuffer.h"
#include "SE_ConvexHull.h"
#include "SE_Bounds.h"
#include "SE_StyleVisitor.h"
#include "SE_Renderer.h"
#include "SE_SymbolManager.h"
#include "SE_PositioningAlgorithms.h"
#include "FeatureTypeStyleVisitor.h"
#include "LineBuffer.h"
#include "Renderer.h"
#include "SE_SymbolDefProxies.h"

#include <algorithm>
#include <functional>

using namespace MDFMODEL_NAMESPACE;


StylizationEngine::StylizationEngine(SE_SymbolManager* resources) :
    m_resources(resources),
    m_renderer(NULL),
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
                                           Renderer*                        renderer,
                                           RS_FeatureReader*                reader,
                                           FdoExpressionEngine*             exec,
                                           CSysTransformer*                 xformer,
                                           CancelStylization                cancel,
                                           void*                            userData)
{
    if (reader == NULL || exec == NULL)
        return;

    m_renderer = renderer;
    m_reader = reader;

    // make sure we have an SE renderer
    // TODO: eliminate the need to do dynamic casts on these renderers.  We should
    //       probably ultimately have just one renderer interface class...
    m_serenderer = dynamic_cast<SE_Renderer*>(renderer);
    if (m_serenderer == NULL)
        return;

    // get the geometry column name
    const wchar_t* gpName = reader->GetGeomPropName();
    if (NULL == gpName)
        return;

    m_serenderer->SetBufferPool(m_pool);
    bool bClip = m_renderer->RequiresClipping();
    double drawingScale = renderer->GetDrawingScale();

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    m_visitor->ParseStringExpression(layer->GetToolTip(), seTip, L"");
    m_visitor->ParseStringExpression(layer->GetUrl(), seUrl, L"");

    // extract all the composite styles once
    MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();
    std::vector<CompositeTypeStyle*> compTypeStyles;
    for (int i=0; i<ftsc->GetCount(); i++)
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

        while (reader->ReadNext())
        {
            #ifdef _DEBUG
            if (numPasses == 1)
                nFeatures++;
            #endif

            LineBuffer* lb = m_pool->NewLineBuffer(8, FdoDimensionality_Z, false);

            // tell line buffer the current drawing scale (used for arc tessellation)
            if (lb)
                lb->SetDrawingScale(drawingScale);

            try
            {
                reader->GetGeometry(gpName, lb, xformer);
            }
            catch (FdoException* e)
            {
                // geometry could be null in which case FDO throws an exception
                // we move on to the next feature
                e->Release();
                m_pool->FreeLineBuffer(lb);
                continue;
            }

            if (lb && bClip)
            {
                // clip geometry to given map request extents
                LineBuffer* lbc = lb->Clip(renderer->GetBounds(), LineBuffer::ctAGF, m_pool);

                // Free original line buffer if the geometry was actually clipped.
                // Note that the original geometry is still accessible using
                // RS_FeatureReader::GetGeometry.
                if (lbc != lb)
                {
                    m_pool->FreeLineBuffer(lb);
                    lb = lbc;
                }
            }

            if (!lb) continue;

            // stylize once for each composite type style
            for (size_t i=0; i<numTypeStyles; i++)
                Stylize(reader, exec, lb, compTypeStyles[i], &seTip, &seUrl, NULL,
                        instanceRenderingPass, symbolRenderingPass,
                        nextInstanceRenderingPass, nextSymbolRenderingPass);

            if (lb)
                m_pool->FreeLineBuffer(lb); // free geometry when done stylizing

            if (cancel && cancel(userData)) break;
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

    double mm2pxs = m_serenderer->GetPixelsPerMillimeterScreen();
    double mm2pxw = m_serenderer->GetPixelsPerMillimeterWorld();

    SE_Matrix w2s;
    m_serenderer->GetWorldToScreenTransform(w2s);

    SE_Rule*& rules = m_rules[style];
    RuleCollection* rulecoll = style->GetRules();
    int nRules = rulecoll->GetCount();

    if (rules == NULL)
    {
        SE_Rule* rulecache = new SE_Rule[nRules];
        rules = rulecache;

        for (int i = 0; i < nRules; i++)
        {
            CompositeRule* r = (CompositeRule*)rulecoll->GetAt(i);
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
                    e->Release();
                }
            }

            rulecache[i].legendLabel= r->GetLegendLabel();

            m_visitor->Convert(rulecache[i].symbolization, r->GetSymbolization());
        }
    }

    SE_Rule* rule = NULL;
    for (int i = 0; i < nRules; i++)
    {
        bool match = (rules[i].filter == NULL);

        if (!match)
            match = exec->ProcessFilter(rules[i].filter);

        if (match)
        {
            rule = &rules[i];
            break;
        }
    }

    if (rule == NULL)
        return;

    std::vector<SE_Symbolization*>* symbolization = &rule->symbolization;

    bool initialPass = (instanceRenderingPass == 0 && symbolRenderingPass == 0);
    RS_String rs_tip = seTip->evaluate(exec);
    RS_String rs_url = seUrl->evaluate(exec);
    RS_String& rs_thm = rule->legendLabel;
    m_renderer->StartFeature(reader, initialPass, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);

    // it's possible to end up with no symbols - we're done in that case
    if (symbolization->size() == 0)
        return;

    // Here's a description of how the transforms work for point and line symbols.
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
    //   S_mm = scaling converting mm to pixels (also includes inverting y, if necessary)
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
    // For line symbols it's simple since the symbol instance insertion offset doesn't
    // apply, nor is there a line usage offset.  The stack looks like:
    //
    //   [T_fe] [S_mm] [R_lu] [S_si] {Geom}
    //
    // where:
    //   S_si = symbol instance scaling
    //   R_lu = line usage rotation
    //   S_mm = scaling converting mm to pixels (also includes inverting y, if necessary)
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

    // TODO: Obey the indices - get rid of the indices altogther - single pass!

    for (std::vector<SE_Symbolization*>::const_iterator iter = symbolization->begin(); iter != symbolization->end(); iter++)
    {
        SE_Symbolization* sym = *iter;

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

        double mm2pxX = (sym->sizeContext == MappingUnits)? mm2pxw : mm2pxs;
        double mm2pxY = (w2s.y1 < 0.0)? -mm2pxX : mm2pxX;

        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));

        // TODO - does this comment still apply?
        // The symbol geometry needs to be inverted if the y coordinate in the renderer points down.
        // This is so that in symbol definitions y points up consistently no matter what the underlying
        // renderer is doing.  Normally we could just apply the world to screen transform to everything,
        // but in some cases we only apply it to the position of the symbol and then offset the symbol
        // geometry from there - so the symbol geometry needs to be pre-inverted.
        xformScale.scale(mm2pxX, mm2pxY);

        // initialize the style evaluation context
        SE_EvalContext evalCxt;
        evalCxt.exec = exec;
        evalCxt.mm2px = mm2pxX;
        evalCxt.mm2pxs = mm2pxs;
        evalCxt.mm2pxw = mm2pxw;
        evalCxt.pool = m_pool;
        evalCxt.fonte = m_serenderer->GetRSFontEngine();
        evalCxt.xform = &xformScale;
        evalCxt.resources = m_resources;

        // initialize the style application context
        SE_Matrix xformTrans;
        xformTrans.translate(sym->absOffset[0].evaluate(exec) * mm2pxX,
                             sym->absOffset[1].evaluate(exec) * mm2pxY);

        SE_ApplyContext applyCtx;
        applyCtx.geometry = geometry;
        applyCtx.renderer = m_serenderer;
        applyCtx.xform = &xformTrans;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            SE_Style* style = *siter;

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
            style->evaluate(&evalCxt);

            // why are these in the symbolization? fix this!
            style->rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(exec);
            style->rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(exec);
            style->rstyle->drawLast = sym->drawLast.evaluate(exec);

            const wchar_t* positioningAlgo = sym->positioningAlgorithm.evaluate(exec);
            if (wcslen(positioningAlgo) > 0 && wcscmp(positioningAlgo, L"Default") != 0)
            {
                LayoutCustomLabel(positioningAlgo, geometry, xformScale, style, style->rstyle, mm2pxX);
            }
            else
            {
                // apply the style to the geometry using the renderer
                style->apply(&applyCtx);
            }
        }
    }
}


void StylizationEngine::LayoutCustomLabel(const wchar_t* positioningAlgo, LineBuffer* geometry, SE_Matrix& xform, SE_Style* style, SE_RenderStyle* rstyle, double mm2px)
{
    // call the appropriate positioning algorithm based on the name
    if (wcscmp(positioningAlgo, L"EightSurrounding") == 0)
    {
        SE_PositioningAlgorithms::EightSurrounding(m_serenderer, geometry, xform, style, rstyle, mm2px);
    }
    else if (wcscmp(positioningAlgo, L"PathLabels") == 0)
    {
        SE_PositioningAlgorithms::PathLabels(m_serenderer, geometry, xform, style, rstyle, mm2px);
    }
    else if (wcscmp(positioningAlgo, L"MultipleHighwayShields") == 0)
    {
        SE_PositioningAlgorithms::MultipleHighwaysShields(m_serenderer, geometry, xform, style, rstyle, mm2px, m_reader, m_resources);
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
