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
    m_serenderer(NULL)
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

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    m_visitor->ParseStringExpression(layer->GetToolTip(), seTip);
    m_visitor->ParseStringExpression(layer->GetUrl(), seUrl);

    MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();

    bool bClip = renderer->RequiresClipping();

    #ifdef _DEBUG
    int nFeatures = 0;
    #endif

    //main loop over feature data
    while (reader->ReadNext())
    {
        #ifdef _DEBUG
        nFeatures++;
        #endif

        LineBuffer* lb = NULL;

        //get the geometry just once
        //all types of geometry
        lb = m_lbPool->NewLineBuffer(8);
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

        // we need to stylize once for each FeatureTypeStyle that matches
        // the geometry type (Note: this may have to change to match
        // feature classes)
        for (int i=0; i<ftsc->GetCount(); i++)
        {
            MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(i);
            if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts) == FeatureTypeStyleVisitor::ftsComposite)
                Stylize(reader, executor, lb, (CompositeTypeStyle*)fts, &seTip, &seUrl, NULL);
        }

        if (lb)
            m_lbPool->FreeLineBuffer(lb); // free geometry when done stylizing

        if (cancel && cancel(userData)) break;
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
                                RS_ElevationSettings* /*elevSettings*/)
{
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

    // TODO: eliminate the need to do dynamic casts on these renderers.  We should
    //       probably ultimately have just one renderer interface class...
    Renderer* renderer = dynamic_cast<Renderer*>(m_serenderer);
    if (renderer)
    {
        const wchar_t* strTip = seTip->evaluate(executor);
        const wchar_t* strUrl = seUrl->evaluate(executor);
        RS_String rs_tip = strTip? strTip : L"";
        RS_String rs_url = strUrl? strUrl : L"";
        RS_String& rs_thm = rule->legendLabel;

        renderer->StartFeature(reader, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);
    }

    /* TODO: Obey the indices--Get rid of the indices altogther--single pass! */
    
    for (std::vector<SE_Symbolization*>::const_iterator iter = symbolization->begin(); iter != symbolization->end(); iter++)
    {
        SE_Symbolization* sym = *iter;

        double mm2px = (sym->context == MappingUnits)? mm2pxw : mm2pxs;
        SE_Matrix xform;
        xform.setTransform( sym->scale[0].evaluate(executor), 
                            sym->scale[1].evaluate(executor),
                            sym->absOffset[0].evaluate(executor)*mm2px, 
                            sym->absOffset[1].evaluate(executor)*mm2px );
        xform.scale(mm2px, mm2px);
        
        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            SE_Style* style = *siter;

            SE_Matrix tmpxform(xform);  //TODO: this is lame, but necessary since the xform can be modified
                                        //when we evalute the style, in the case of point style set on a
                                        //non-point geometry

            //allocate a render style into which we will evaluate all styles (and the expressions inside them)
            SE_RenderStyle* rstyle = NULL;
            switch(style->type)
            {
                case SE_PointStyleType:
                    //this call may modify xform to apply the necessary rotation 
                    rstyle = EvaluatePointStyle(executor, geometry, tmpxform, (SE_PointStyle*)style, mm2px);
                    break;
                case SE_LineStyleType:
                    rstyle = EvaluateLineStyle(executor, tmpxform, (SE_LineStyle*)style);
                    break;
                case SE_AreaStyleType:
                    rstyle = EvaluateAreaStyle(executor, tmpxform, (SE_AreaStyle*)style);
                    break;
            }
            
            //evaluate values that are common to all styles           
            rstyle->renderPass = style->renderPass.evaluate(executor);
            rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(executor);
            rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(executor);
            rstyle->drawLast = sym->drawLast.evaluate(executor);

            //evaluate the graphic elements
            EvaluateSymbols(executor, tmpxform, style, rstyle, mm2px); 

            if (!sym->positioningAlgorithm.empty() && sym->positioningAlgorithm != L"Default")
            {
                LayoutCustomLabel(sym->positioningAlgorithm, geometry, tmpxform, style, rstyle, mm2px);
            }
            else
            {
                switch(style->type)
                {
                case SE_PointStyleType:
                    m_serenderer->ProcessPoint(geometry, (SE_RenderPointStyle*)rstyle);
                    break;
                case SE_LineStyleType:
                    m_serenderer->ProcessLine(geometry, (SE_RenderLineStyle*)rstyle);
                    break;
                case SE_AreaStyleType:
                    m_serenderer->ProcessArea(geometry, (SE_RenderAreaStyle*)rstyle);
                    break;
                }
            }

            //Free the render style. If the style was needed for drawing as a label,
            //the renderer would have cloned it and created its own pointer
            delete rstyle;
        }
    }
}


SE_RenderPointStyle* StylizationEngine::EvaluatePointStyle(RS_FilterExecutor* executor,
                                                           LineBuffer* geometry,
                                                           SE_Matrix& xform,
                                                           SE_PointStyle* style,
                                                           double mm2px)
{
    SE_RenderPointStyle* render = new SE_RenderPointStyle();

    LineBuffer::GeomOperationType type;
    switch(geometry->geom_type())
    {
    case FdoGeometryType_LineString:
    case FdoGeometryType_MultiLineString:
        type = LineBuffer::ctLine;
        break;
    case FdoGeometryType_Polygon:
    case FdoGeometryType_MultiPolygon:
        type = LineBuffer::ctArea;
        break;
    case FdoGeometryType_Point:
    case FdoGeometryType_MultiPoint:
        type = LineBuffer::ctPoint;
        break;
    default:
        type = LineBuffer::ctNone;
        break;
    }

    double angle = 0.0;
    const wchar_t* angleControl = style->angleControl.evaluate(executor);
    if (wcscmp(L"FromGeometry", angleControl) == 0)
    {
        if (type == LineBuffer::ctLine || type == LineBuffer::ctArea)
        {
            double x0, y0;
            double slope_rad = 0.0;
            geometry->Centroid(LineBuffer::ctLine, &x0, &y0, &slope_rad);

            angle = slope_rad; 

            //TODO: do we really need to invert this in case of y-down?
            if (xform.y1 < 0)
                angle = -angle;
        }
    }
    else
        angle = style->angle.evaluate(executor) * M_PI180;

    double originOffsetX = style->originOffset[0].evaluate(executor)*mm2px;
    double originOffsetY = style->originOffset[1].evaluate(executor)*mm2px;

    SE_Matrix sxform;
    sxform.translate(originOffsetX, originOffsetY);
    sxform.rotate(angle);
    sxform.premultiply(xform);
    xform = sxform;

    return render;
}


SE_RenderLineStyle* StylizationEngine::EvaluateLineStyle(RS_FilterExecutor* executor, SE_Matrix& xform, SE_LineStyle* style)
{
    SE_RenderLineStyle* render = new SE_RenderLineStyle();

    render->angleControl = style->angleControl.evaluate(executor);
    render->unitsControl = style->unitsControl.evaluate(executor);
    render->vertexControl = style->vertexControl.evaluate(executor);
//  render->join = style->join.evaluate(m_exec);

    render->angle = style->angle.evaluate(executor) * M_PI180;
    render->startOffset = style->startOffset.evaluate(executor)*xform.x0; // x0 is x scale * mm2px
    render->endOffset = style->endOffset.evaluate(executor)*xform.x0;
    render->repeat = style->repeat.evaluate(executor)*xform.x0;
    render->vertexAngleLimit = style->vertexAngleLimit.evaluate(executor) * M_PI180;

    return render;
}


SE_RenderAreaStyle* StylizationEngine::EvaluateAreaStyle(RS_FilterExecutor* executor, SE_Matrix& /*xform*/, SE_AreaStyle* style)
{
    SE_RenderAreaStyle* render = new SE_RenderAreaStyle();

    render->angleControl = style->angleControl.evaluate(executor);
    render->originControl = style->originControl.evaluate(executor);
    render->clippingControl = style->clippingControl.evaluate(executor);

    render->angle = style->angle.evaluate(executor) * M_PI180;
    render->origin[0] = style->origin[0].evaluate(executor);
    render->origin[1] = style->origin[1].evaluate(executor);
    render->repeat[0] = style->repeat[0].evaluate(executor);
    render->repeat[1] = style->repeat[1].evaluate(executor);
    render->bufferWidth = style->bufferWidth.evaluate(executor);

    return render;
}


void StylizationEngine::EvaluateSymbols(RS_FilterExecutor* executor, SE_Matrix& xform, SE_Style* style, SE_RenderStyle* renderStyle, double mm2px)
{
    double dx, dy, sx, sy;
    double minx, maxx, miny, maxy;
    double growx, growy;
    if (style->useBox)
    {
        dx = style->resizePosition[0].evaluate(executor);
        dy = style->resizePosition[1].evaluate(executor);
        sx = style->resizeSize[0].evaluate(executor)/2.0;
        sy = style->resizeSize[1].evaluate(executor)/2.0;

        xform.transform(dx - sx, dy - sy, minx, miny);
        xform.transform(dx + sx, dy + sy, maxx, maxy);
        xform.transform(dx, dy);

        growx = 0.0;
        growy = 0.0;
    }

    double mm2pxs = m_serenderer->GetPixelsPerMillimeterScreen();
    double mm2pxw = m_serenderer->GetPixelsPerMillimeterWorld();

    for (SE_PrimitiveList::const_iterator src = style->symbol.begin(); src != style->symbol.end(); src++)
    {
        SE_Primitive* sym = *src;
        SE_RenderPrimitive* rsym = NULL;

        switch(sym->type)
        {
        case SE_PolygonPrimitive:
            rsym = new SE_RenderPolygon();
            ((SE_RenderPolygon*)rsym)->fill = ((SE_Polygon*)sym)->fill.evaluate(executor);

        case SE_PolylinePrimitive:
            {
                if (!rsym) rsym = new SE_RenderPolyline();
                SE_Polyline* p = (SE_Polyline*)sym;
                SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;
                double wx = p->weightScalable.evaluate(executor)? mm2pxw : mm2pxs;
                rp->weight = p->weight.evaluate(executor) * wx;
                rp->geometry = p->geometry->Clone();
                rp->color = p->color.evaluate(executor);
                /* Defer transformation */
                if (sym->resize != GraphicElement::AdjustToResizeBox)
                {
                    rp->geometry->Transform(xform, rp->weight);
                    rp->bounds = rp->geometry->xf_bounds()->Clone();
                }
                else
                    rp->bounds = NULL;
            }
            break;

        case SE_TextPrimitive:
            {
                rsym = new SE_RenderText();
                SE_Text* t = (SE_Text*)sym;
                SE_RenderText* rt = (SE_RenderText*)rsym;
                rt->text = t->textExpr.evaluate(executor);
                rt->position[0] = t->position[0].evaluate(executor);
                rt->position[1] = t->position[1].evaluate(executor);
                xform.transform(rt->position[0], rt->position[1]);

                rt->tdef.rotation() = t->angle.evaluate(executor);;

                int style = RS_FontStyle_Regular;
                if (t->underlined.evaluate(executor)) style |= (int)RS_FontStyle_Underline;
                if (t->italic.evaluate(executor)) style |= (int)RS_FontStyle_Italic;
                if (t->bold.evaluate(executor)) style |= (int)RS_FontStyle_Bold;

                rt->tdef.font().style() = (RS_FontStyle_Mask)style;
                rt->tdef.font().name() = t->fontExpr.evaluate(executor);
                rt->tdef.font().height() = t->size.evaluate(executor)*0.001*xform.y1/mm2px; //convert mm to meters which is what RS_TextDef expects
                rt->tdef.linespace() = t->lineSpacing.evaluate(executor);

                rt->tdef.color() = RS_Color::FromARGB(t->textColor.evaluate(executor));
                rt->tdef.bgcolor() = RS_Color::FromARGB(t->ghostColor.evaluate(executor));

                const wchar_t* hAlign = t->hAlignment.evaluate(executor);
                if (hAlign)
                {
                    if (wcscmp(hAlign, L"Left") == 0)           
                        rt->tdef.halign() = RS_HAlignment_Left;
                    else if (wcscmp(hAlign, L"Center") == 0)    
                        rt->tdef.halign() = RS_HAlignment_Center;
                    else if (wcscmp(hAlign, L"Right") == 0)     
                        rt->tdef.halign() = RS_HAlignment_Right;
                }

                const wchar_t* vAlign = t->vAlignment.evaluate(executor);
                if (vAlign)
                {
                    if (wcscmp(vAlign, L"Bottom") == 0)         
                        rt->tdef.valign() = RS_VAlignment_Descent;
                    else if (wcscmp(vAlign, L"Baseline") == 0)  
                        rt->tdef.valign() = RS_VAlignment_Base;
                    else if (wcscmp(vAlign, L"Halfline") == 0)  
                        rt->tdef.valign() = RS_VAlignment_Half;
                    else if (wcscmp(vAlign, L"Capline") == 0)   
                        rt->tdef.valign() = RS_VAlignment_Cap;
                    else if (wcscmp(vAlign, L"Top") == 0)       
                        rt->tdef.valign() = RS_VAlignment_Ascent;
                }

                RS_TextMetrics tm;
                SE_Matrix txf;
                m_serenderer->GetFontEngine()->GetTextMetrics(rt->text, rt->tdef, tm, false);
                txf.rotate(rt->tdef.rotation() * M_PI180);
                txf.translate(rt->position[0], rt->position[1]);

                double* buffer = (double*)alloca(tm.line_pos.size()*sizeof(double)*8);
                double* ptr = buffer;
                for(size_t k = 0; k < tm.line_pos.size(); k++)
                {
                    const RS_F_Point* pts = tm.line_pos[k].ext;
                    txf.transform(pts[0].x, pts[0].y, ptr[0], ptr[1]);
                    txf.transform(pts[1].x, pts[1].y, ptr[2], ptr[3]);
                    txf.transform(pts[2].x, pts[2].y, ptr[4], ptr[5]);
                    txf.transform(pts[3].x, pts[3].y, ptr[6], ptr[7]);
                    ptr += 8;
                }

                std::sort((std::pair<double,double>*)buffer, (std::pair<double,double>*)(ptr - 2), PointLess( ));
                rt->bounds = AndrewHull<std::pair<double,double>*,SimplePOINT>
                    ((std::pair<double,double>*)buffer, 
                    ((std::pair<double,double>*)ptr) - 1, 
                    (int)tm.line_pos.size()*4, m_pool);
            }
            break;

        case SE_RasterPrimitive:
            {
                rsym = new SE_RenderRaster();
                SE_Raster* r = (SE_Raster*)sym;
                SE_RenderRaster* rr = (SE_RenderRaster*)rsym;

                if (!r->pngPtr)
                {
                    rr->pngPtr = m_resources->GetImageData(r->pngPath.evaluate(executor), rr->pngSize);
                }
                else
                {
                    rr->pngPtr = r->pngPtr;
                    rr->pngSize = r->pngSize;
                }

                rr->position[0] = r->position[0].evaluate(executor);
                rr->position[1] = r->position[1].evaluate(executor);
                xform.transform(rr->position[0], rr->position[1]);
                rr->extent[0] = r->extent[0].evaluate(executor)*xform.x0;
                rr->extent[1] = r->extent[1].evaluate(executor)*xform.y1;
                rr->angle = r->angle.evaluate(executor) * M_PI180;

                SE_Matrix rxf;
                rxf.rotate(rr->angle);
                rxf.translate(rr->position[0], rr->position[1]);

                double w = 0.5 * rr->extent[0];
                double h = 0.5 * rr->extent[1];

                RS_F_Point pts[4];
                rxf.transform( w,  h, pts[0].x, pts[0].y);
                rxf.transform( w, -h, pts[1].x, pts[1].y);
                rxf.transform(-w, -h, pts[2].x, pts[2].y);
                rxf.transform(-w,  h, pts[3].x, pts[3].y);
                
                std::sort((std::pair<double,double>*)pts, (std::pair<double,double>*)(pts + 3), PointLess( ));
                rr->bounds = AndrewHull<std::pair<double,double>*,SimplePOINT>
                    ((std::pair<double,double>*)pts, 
                    ((std::pair<double,double>*)(pts+3)) - 1, 
                    4, m_pool);
            }
            break;

        default:
            break;
        }

        if (rsym)
        {
            rsym->resize = (sym->resize == GraphicElement::AdjustToResizeBox);

            if (!rsym->resize)
            {
                if (renderStyle->bounds)
                {
                    SE_Bounds* bounds = renderStyle->bounds;
                    renderStyle->bounds = bounds->Union(rsym->bounds);
                    bounds->Free();
                }
                else
                    renderStyle->bounds = rsym->bounds->Clone();
            }

            renderStyle->symbol.push_back(rsym);

            if (style->useBox && sym->resize == GraphicElement::AddToResizeBox)
                rsym->bounds->Contained(minx, miny, maxx, maxy, growx, growy);
        }
    }

    if (style->useBox)
    {
        switch(style->resize)
        {
        case ResizeBox::GrowInX:
            growy = 0.0;
            break;

        case ResizeBox::GrowInY:
            growx = 0.0;
            break;

        case ResizeBox::GrowInXYMaintainAspect:
            if (growy > growx)
                growx = growy;
            else if (growx > growy)
                growy = growx;
        }

        SE_Matrix totalxf(xform);
        SE_Matrix growxf;
        growxf.translate(-dx, -dy);
        growxf.scale(1.0 + growx, 1.0 + growy);
        growxf.translate(dx, dy);
        totalxf.premultiply(growxf);

        for (SE_RenderPrimitiveList::iterator rs = renderStyle->symbol.begin(); rs != renderStyle->symbol.end(); rs++)
        {
            SE_RenderPrimitive* rsym = *rs;
            if (rsym->resize)
            {
                switch(rsym->type)
                {
                case SE_PolygonPrimitive:
                case SE_PolylinePrimitive:
                    {
                        SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;
                        rp->geometry->Transform(totalxf, rp->weight);
                        rp->bounds = rp->geometry->xf_bounds()->Clone();
                        break;
                    }
                case SE_TextPrimitive:
                    {
                        SE_RenderText* rt = (SE_RenderText*)rsym;
                        growxf.transform(rt->position[0], rt->position[1]);
                        rt->tdef.font().height() *= growxf.y1;
                        rt->bounds->Transform(growxf);
                        break;
                    }
                case SE_RasterPrimitive:
                    {
                        SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                        growxf.transform(rr->position[0], rr->position[1]);
                        rr->extent[0] *= growxf.x0;
                        rr->extent[1] *= growxf.y1;
                        rr->bounds->Transform(growxf);
                        break;
                    }
                }
                
                if (renderStyle->bounds)
                {
                    SE_Bounds* bounds = renderStyle->bounds;
                    renderStyle->bounds = bounds->Union(rsym->bounds);
                    bounds->Free();
                }
                else
                    renderStyle->bounds = rsym->bounds->Clone();
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
}


//clears cached filters/styles/etc
void StylizationEngine::ClearCache()
{
    std::map<CompositeTypeStyle*, SE_Rule*>::iterator iter = m_rules.begin();

    for (; iter != m_rules.end(); iter++)
        delete [] iter->second;

    m_rules.clear();
}
