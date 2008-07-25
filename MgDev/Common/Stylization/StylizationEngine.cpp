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
#include "RS_FontEngine.h"
#include "FeatureTypeStyleVisitor.h"
#include "LineBuffer.h"
#include "Renderer.h"
#include "SE_SymbolDefProxies.h"

#include <algorithm>
#include <functional>

using namespace MDFMODEL_NAMESPACE;

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);


StylizationEngine::StylizationEngine(SE_SymbolManager* resources) :
    m_resources(resources),
    m_renderer(NULL),
    m_serenderer(NULL),
    m_reader(NULL)
{
    m_pool = new SE_BufferPool;
    m_lbPool = new LineBufferPool;
    m_visitor = new SE_StyleVisitor(resources, m_pool);
}


StylizationEngine::~StylizationEngine()
{
    ClearCache();
    delete m_pool;
    delete m_lbPool;
    delete m_visitor;
}


/* TODO: Stylize one CompoundSymbol feature and label per run; investigate caching
 *       possiblities to avoid filter execution on subsequent passes */
void StylizationEngine::StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                                           MdfModel::VectorScaleRange*      range,
                                           Renderer*                        renderer,
                                           RS_FeatureReader*                reader,
                                           RS_FilterExecutor*               executor,
                                           CSysTransformer*                 xformer,
                                           CancelStylization                cancel,
                                           void*                            userData)
{
    if (reader == NULL || executor == NULL)
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
    int renderingPass = 0;
    int nextRenderingPass = -1;

    #ifdef _DEBUG
    int nFeatures = 0;
    #endif

    //main loop over feature data
    while (renderingPass >= 0)
    {
        // for all but the first pass we need to reset the reader
        if (renderingPass > 0)
            reader->Reset();

        while (reader->ReadNext())
        {
            #ifdef _DEBUG
            if (renderingPass == 0)
                nFeatures++;
            #endif

            LineBuffer* lb = m_lbPool->NewLineBuffer(8);

            try
            {
                reader->GetGeometry(gpName, lb, xformer);
            }
            catch (FdoException* e)
            {
                //geometry could be null in which case FDO throws an exception
                //we move on to the next feature
                ProcessStylizerException(e, __LINE__, __WFILE__);
                m_lbPool->FreeLineBuffer(lb);
                continue;
            }

            if (lb && bClip)
            {
                //clip geometry to given map request extents
                //TODO: is this the right place to do so?
                LineBuffer* lbc = lb->Clip(renderer->GetBounds(), LineBuffer::ctAGF, m_lbPool);

                //did geom require clipping?
                //free original line buffer
                //note original geometry is still accessible to the
                //user from the RS_FeatureReader::GetGeometry
                if (lbc != lb)
                {
                    m_lbPool->FreeLineBuffer(lb);
                    lb = lbc;
                }
            }

            if (!lb) continue;

            //need to clear out the filter execution engine cache
            //some feature attributes may be cached while executing theming
            //expressions and this call flushes that
            executor->Reset();

            // stylize once for each composite type style
            for (size_t i=0; i<numTypeStyles; i++)
                Stylize(reader, executor, lb, compTypeStyles[i], &seTip, &seUrl, NULL, renderingPass, nextRenderingPass);

            if (lb)
                m_lbPool->FreeLineBuffer(lb); // free geometry when done stylizing

            if (cancel && cancel(userData)) break;
        }

        renderingPass = nextRenderingPass;
        nextRenderingPass = -1;
    }

    #ifdef _DEBUG
    printf("  StylizationEngine::StylizeVectorLayer() Layer: %S  Features: %d\n", layer->GetFeatureName().c_str(), nFeatures);
    #endif
}


void StylizationEngine::Stylize(RS_FeatureReader* reader,
                                RS_FilterExecutor* executor,
                                LineBuffer* geometry,
                                CompositeTypeStyle* style,
                                SE_String* seTip,
                                SE_String* seUrl,
                                RS_ElevationSettings* /*elevSettings*/,
                                int renderingPass,
                                int& nextRenderingPass)
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
                    ProcessStylizerException(e, __LINE__, __WFILE__);
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
        {
            rules[i].filter->Process(executor);
            match = executor->GetResult();
        }

        if (match)
        {
            rule = &rules[i];
            break;
        }
    }

    if (rule == NULL)
        return;

    std::vector<SE_Symbolization*>* symbolization = &rule->symbolization;

    // only call StartFeature for the initial rendering pass
    if (renderingPass == 0)
    {
        RS_String rs_tip = seTip->evaluate(executor);
        RS_String rs_url = seUrl->evaluate(executor);
        RS_String& rs_thm = rule->legendLabel;

        m_renderer->StartFeature(reader, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);
    }

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
    //   T_fe = transation to the point feature
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
    //   T_fe = transation along the line feature
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

    /* TODO: Obey the indices--Get rid of the indices altogther--single pass! */

    for (std::vector<SE_Symbolization*>::const_iterator iter = symbolization->begin(); iter != symbolization->end(); iter++)
    {
        SE_Symbolization* sym = *iter;

        double mm2pxX = (sym->context == MappingUnits)? mm2pxw : mm2pxs;
        double mm2pxY = (w2s.y1 < 0.0)? -mm2pxX : mm2pxX;

        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(executor),
                         sym->scale[1].evaluate(executor));

        // TODO - does this comment still apply?
        // The symbol geometry needs to be inverted if the y coordinate in the renderer points down.
        // This is so that in symbol definitions y points up consistently no matter what the underlying
        // renderer is doing.  Normally we could just apply the world to screen transform to everything,
        // but in some cases we only apply it to the position of the symbol and then offset the symbol
        // geometry from there - so the symbol geometry needs to be pre-inverted.
        xformScale.scale(mm2pxX, mm2pxY);

        //initialize the style evaluation context
        SE_EvalContext evalCxt;
        evalCxt.exec = executor;
        evalCxt.mm2px = mm2pxX;
        evalCxt.mm2pxs = mm2pxs;
        evalCxt.mm2pxw = mm2pxw;
        evalCxt.pool = m_pool;
        evalCxt.fonte = m_serenderer->GetFontEngine();
        evalCxt.xform = &xformScale;
        evalCxt.resources = m_resources;

        //initialize the style application context
        SE_Matrix xformTrans;
        xformTrans.translate(sym->absOffset[0].evaluate(executor) * mm2pxX,
                             sym->absOffset[1].evaluate(executor) * mm2pxY);

        SE_ApplyContext applyCtx;
        applyCtx.geometry = geometry;
        applyCtx.renderer = m_serenderer;
        applyCtx.xform = &xformTrans;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            SE_Style* style = *siter;

            // process the rendering pass - negative rendering passes are rendered with pass 0
            int styleRenderPass = style->renderPass.evaluate(executor);
            if (styleRenderPass < 0)
                styleRenderPass = 0;

            // If the rendering pass for the style doesn't match the current pass
            // then don't render using it.
            // TODO - how does DrawLast affect the logic?
            if (styleRenderPass != renderingPass)
            {
                // if the style's rendering pass is greater than the current pass,
                // then update nextRenderingPass to account for it
                if (styleRenderPass > renderingPass)
                {
                    // update nextRenderingPass if it hasn't yet been set, or if
                    // the style's pass is less than the current next pass
                    if (nextRenderingPass == -1 || styleRenderPass < nextRenderingPass)
                        nextRenderingPass = styleRenderPass;
                }

                continue;
            }

            //evaluate the style (all expressions inside it) and convert to a constant screen space
            //render style
            style->evaluate(&evalCxt);

            //why are these in the symbolization? fix this!
            style->rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(executor);
            style->rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(executor);
            style->rstyle->drawLast = sym->drawLast.evaluate(executor);

            const wchar_t* positioningAlgo = sym->positioningAlgorithm.evaluate(executor);
            if (wcslen(positioningAlgo) > 0 && wcscmp(positioningAlgo, L"Default") != 0)
            {
                LayoutCustomLabel(positioningAlgo, geometry, xformScale, style, style->rstyle, mm2pxX);
            }
            else
            {
                //apply the style to the geometry using the renderer
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
        SE_PositioningAlgorithms::MultipleHighwaysShields(m_serenderer, geometry, xform, style, rstyle, mm2px,
                                                          m_reader, m_resources);
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
