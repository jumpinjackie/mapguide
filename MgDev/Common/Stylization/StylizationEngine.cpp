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


StylizationEngine::StylizationEngine(SE_SymbolManager* resources) :
    m_resources(resources),
    m_renderer(NULL),
    m_serenderer(NULL),
    m_reader(NULL)
{
    m_pool = new SE_LineBufferPool;
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

    m_serenderer->SetLineBufferPool(m_pool);
    bool bClip = m_renderer->RequiresClipping();

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    m_visitor->ParseStringExpression(layer->GetToolTip(), seTip);
    m_visitor->ParseStringExpression(layer->GetUrl(), seUrl);

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

            //get the geometry just once
            //all types of geometry
            LineBuffer* lb = m_lbPool->NewLineBuffer(8);
            reader->GetGeometry(gpName, lb, xformer);

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
        const wchar_t* strTip = seTip->evaluate(executor);
        const wchar_t* strUrl = seUrl->evaluate(executor);
        RS_String rs_tip = strTip? strTip : L"";
        RS_String rs_url = strUrl? strUrl : L"";
        RS_String& rs_thm = rule->legendLabel;

        m_renderer->StartFeature(reader, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);
    }

    /* TODO: Obey the indices--Get rid of the indices altogther--single pass! */

    for (std::vector<SE_Symbolization*>::const_iterator iter = symbolization->begin(); iter != symbolization->end(); iter++)
    {
        SE_Symbolization* sym = *iter;

        double mm2px = (sym->context == MappingUnits)? mm2pxw : mm2pxs;
        SE_Matrix xform;
        xform.setTransform( sym->scale[0].evaluate(executor),
                            sym->scale[1].evaluate(executor),
                            sym->absOffset[0].evaluate(executor),
                            sym->absOffset[1].evaluate(executor) );
        xform.scale(mm2px, mm2px);

        //initialize the style evaluation context
        SE_EvalContext cxt;
        cxt.exec = executor;
        cxt.mm2px = mm2px;
        cxt.mm2pxs = m_serenderer->GetPixelsPerMillimeterScreen();
        cxt.mm2pxw = m_serenderer->GetPixelsPerMillimeterWorld();
        cxt.pool = m_pool;
        cxt.fonte = m_serenderer->GetFontEngine();
        cxt.geometry = geometry; //only used by point styles, I really want to get rid of this

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

            SE_Matrix tmpxform(xform);  //TODO: this is lame, but necessary since the xform can be modified
                                        //when we evalute the style, in the case of point style set on a
                                        //non-point geometry

            cxt.xform = &tmpxform; //EXTREMELY IMPORTANT: evaluating of point styles will modify this
                                   //transform without you knowing -- beware!

            //evaluate the style (all expressions inside it) and convert to a constant screen space
            //render style
            style->evaluate(&cxt);

            //why are these in the symbolization? fix this!
            style->rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(executor);
            style->rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(executor);
            style->rstyle->drawLast = sym->drawLast.evaluate(executor);

            if (!sym->positioningAlgorithm.empty() && sym->positioningAlgorithm != L"Default")
            {
                LayoutCustomLabel(sym->positioningAlgorithm, geometry, tmpxform, style, style->rstyle, mm2px);
            }
            else
            {
                //apply the style to the geometry using the renderer
                style->apply(geometry, m_serenderer);
            }
        }
    }
}

void StylizationEngine::LayoutCustomLabel(const std::wstring& positioningAlgo, LineBuffer* geometry, SE_Matrix& xform, SE_Style* style, SE_RenderStyle* rstyle, double mm2px)
{
    //here we decide which one to call based on the name of the positioning algorithm
    if (positioningAlgo == L"EightSurrounding")
    {
        SE_PositioningAlgorithms::EightSurrounding(m_serenderer, geometry, xform, style, rstyle, mm2px);
    }
    else if (positioningAlgo == L"PathLabels")
    {
        SE_PositioningAlgorithms::PathLabels(m_serenderer, geometry, xform, style, rstyle, mm2px);
    }
    else if (positioningAlgo == L"MultipleHighwayShields")
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
