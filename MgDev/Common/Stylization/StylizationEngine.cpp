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

#include <algorithm>
#include <functional>

using namespace MDFMODEL_NAMESPACE;


StylizationEngine::StylizationEngine(SE_SymbolManager* resources) :
    m_resources(resources)
{
    m_pool = new SE_LineBufferPool;
    m_visitor = new SE_StyleVisitor(resources, m_pool);
}

StylizationEngine::~StylizationEngine()
{
    ClearCache();
    delete m_pool;
    delete m_visitor;
}

void StylizationEngine::Stylize( SE_Renderer* renderer,
                                 RS_FeatureReader* feature,
                                 RS_FilterExecutor* executor,
                                 LineBuffer* geometry,
                                 CompositeTypeStyle* style )
{
    if (m_renderer == NULL || m_reader == NULL || m_exec == NULL)
        return;

    m_renderer = renderer;
    m_reader = feature;
    m_exec = executor;

    m_renderer->GetWorldToScreenTransform(m_w2s);
    m_mm2pxs = m_renderer->GetPixelsPerMillimeterScreen();
    m_mm2pxw = m_renderer->GetPixelsPerMillimeterWorld();
    m_renderer->SetLineBufferPool(m_pool);

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
                    FdoFilter::Parse(filterstr.c_str());
                }
                catch (FdoException* e)
                {
                    e->Release();
                }
            }

            m_visitor->Convert(rulecache[i].symbolization, r->GetSymbolization());
        }
    }

    std::vector<SE_Symbolization*>* symbolization = NULL;

    for (int i = 0; i < nRules; i++)
    {
        bool match = rules[i].filter == NULL;
        
        if (!match)
        {
            rules[i].filter->Process(executor);
            match = executor->GetResult();
        }

        if (match)
        {
            symbolization = &rules[i].symbolization;
            break;
        }
    }

    if (symbolization == NULL)
        return;

    /* TODO: Obey the indices--Get rid of the indices alltogther--single pass! */
    
    SE_LineBuffer* xformGeom = m_pool->NewLineBuffer(geometry->point_count());

    xformGeom->SetToTransform(geometry, m_w2s);

    for (std::vector<SE_Symbolization*>::const_iterator iter = symbolization->begin(); iter != symbolization->end(); iter++)
    {
        SE_Symbolization* sym = *iter;

        double mm2px = (sym->context == MappingUnits ? m_mm2pxw : m_mm2pxs);
        SE_Matrix xform;
        xform.setTransform( sym->scale[0].evaluate(m_exec), 
                            sym->scale[1].evaluate(m_exec),
                            sym->absOffset[0].evaluate(m_exec)*mm2px, 
                            sym->absOffset[1].evaluate(m_exec)*mm2px );
        xform.scale(mm2px, mm2px);
        
        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            SE_Style* style = *siter;

            SE_Matrix tmpxform = xform; //TODO: this is lame, but necessary since the xform can be modified
                                        //when we evalute the style, in the case of point style set on a
                                        //non-point geometry

            //allocate a render style into which we will evaluate all styles (and the expressions inside them)
            SE_RenderStyle* rstyle = NULL;
            switch(style->type)
            {
                case SE_PointStyleType:
                    //this call may modify xform to apply the necessary rotation 
                    rstyle = EvaluatePointStyle(xformGeom, tmpxform, (SE_PointStyle*)style, mm2px);
                    break;
                case SE_LineStyleType:
                    rstyle = EvaluateLineStyle(tmpxform, (SE_LineStyle*)style);
                    break;
                case SE_AreaStyleType:
                    rstyle = EvaluateAreaStyle(tmpxform, (SE_AreaStyle*)style);
                    break;
            }
            
            //evaluate values that are common to all styles           
            rstyle->renderPass = style->renderPass.evaluate(m_exec);
            rstyle->addToExclusionRegions = sym->addToExclusionRegions.evaluate(m_exec);
            rstyle->checkExclusionRegions = sym->checkExclusionRegions.evaluate(m_exec);
            rstyle->drawLast = sym->drawLast.evaluate(m_exec);

            //evaluate the graphic elements
            EvaluateSymbols(tmpxform, style, rstyle, mm2px); 

            if (!sym->positioningAlgorithm.empty() && sym->positioningAlgorithm != L"Default")
            {
                LayoutCustomLabel(sym->positioningAlgorithm, xformGeom, tmpxform, style, rstyle, mm2px);
            }
            else
            {
                switch(style->type)
                {
                case SE_PointStyleType:
                    m_renderer->ProcessPoint(xformGeom, (SE_RenderPointStyle*)rstyle);
                    break;
                case SE_LineStyleType:
                    m_renderer->ProcessLine(xformGeom, (SE_RenderLineStyle*)rstyle);
                    break;
                case SE_AreaStyleType:
                    m_renderer->ProcessArea(xformGeom, (SE_RenderAreaStyle*)rstyle);
                    break;
                }
            }

            //TODO: LEAK: the RenderStyles used for labeling are leaked because of this -- we need to free them somehow
            if (!sym->drawLast.evaluate(m_exec))
                delete rstyle;
        }
    }
    m_pool->FreeLineBuffer(xformGeom);
}

SE_RenderPointStyle* StylizationEngine::EvaluatePointStyle(SE_LineBuffer* geometry, SE_Matrix& xform, SE_PointStyle* style, double mm2px)
{
    SE_RenderPointStyle* render = new SE_RenderPointStyle();

    double offsetX = 0, offsetY = 0;
    double rotation = 0;

    LineBuffer::GeomOperationType type;

    switch(geometry->xf_geom_type())
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

    const wchar_t* orientation = style->orientation.evaluate(m_exec);
    if (wcscmp(L"FromGeometry", orientation) == 0)
    {
        if (type == LineBuffer::ctLine || type == LineBuffer::ctArea)
        {
            double x0, x1, y0, y1;
            geometry->xf_longest_edge(x0, y0, x1, y1);
            rotation = atan2(y1 - y0, x1 - x0); //TODO: this is probably affected by which way y goes in the renderer (yUp or yDown)
            if (rotation < 0)
                rotation += 2*M_PI;
        }
        else
            rotation = 0.0;
    }
    else
        rotation = style->angle.evaluate(m_exec)*M_PI/180.0;

    SE_Matrix sxform;

    offsetX = style->offset[0].evaluate(m_exec)*mm2px;
    offsetY = style->offset[1].evaluate(m_exec)*mm2px;
    sxform.translate(offsetX, offsetY);
    sxform.rotate(rotation);
    sxform.premultiply(xform);
    xform = sxform;

    return render;
}

SE_RenderLineStyle* StylizationEngine::EvaluateLineStyle(SE_Matrix& xform, SE_LineStyle* style)
{
    SE_RenderLineStyle* render = new SE_RenderLineStyle();

    render->angle = style->angle.evaluate(m_exec)*(M_PI/180.0);
    render->angleLimit = style->angleLimit.evaluate(m_exec)*(M_PI/180.0);
    render->endOffset = style->endOffset.evaluate(m_exec)*xform.x0; // x0 is x scale * mm2px
    render->repeat = style->repeat.evaluate(m_exec)*xform.x0;
    render->startOffset = style->startOffset.evaluate(m_exec)*xform.x0;

    render->units = style->units.evaluate(m_exec);
    render->orientation = style->orientation.evaluate(m_exec);
    render->overlap = style->overlap.evaluate(m_exec);

    return render;
}

SE_RenderAreaStyle* StylizationEngine::EvaluateAreaStyle(SE_Matrix& xform, SE_AreaStyle* style)
{
    SE_RenderAreaStyle* render = new SE_RenderAreaStyle();

    render->orientation = style->orientation.evaluate(m_exec);
    render->clipping = style->clipping.evaluate(m_exec);
    render->origincontrol = style->origincontrol.evaluate(m_exec);

    return render;
}


void StylizationEngine::EvaluateSymbols(SE_Matrix& xform, SE_Style* style, SE_RenderStyle* renderStyle, double mm2px)
{
    double dx, dy, sx, sy;
    double minx, maxx, miny, maxy;
    double growx, growy;
    if (style->useBox)
    {
        dx = style->resizePosition[0].evaluate(m_exec);
        dy = style->resizePosition[1].evaluate(m_exec);
        sx = style->resizeSize[0].evaluate(m_exec)/2.0;
        sy = style->resizeSize[1].evaluate(m_exec)/2.0;

        xform.transform(dx - sx, dy - sy, minx, miny);
        xform.transform(dx + sx, dy + sy, maxx, maxy);
        xform.transform(dx, dy);

        growx = 0.0;
        growy = 0.0;
    }

    for (SE_Symbol::const_iterator src = style->symbol.begin(); src != style->symbol.end(); src++)
    {
        SE_Primitive* sym = *src;
        SE_RenderPrimitive* rsym = NULL;

        switch(sym->type)
        {
        case SE_PolygonPrimitive:
            rsym = new SE_RenderPolygon();
            ((SE_RenderPolygon*)rsym)->fill = ((SE_Polygon*)sym)->fill.evaluate(m_exec);
        case SE_PolylinePrimitive:
            {
                if (!rsym) rsym = new SE_RenderPolyline();
                SE_Polyline* p = (SE_Polyline*) sym;
                SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;
                rp->weight = p->weight.evaluate(m_exec)*mm2px;
                rp->geometry = p->geometry;
                rp->geometry->Transform(xform, rp->weight);
                rp->bounds = p->geometry->xf_bounds();
                rp->color = p->color.evaluate(m_exec);
            }
            break;
        case SE_TextPrimitive:
            {
                rsym = new SE_RenderText();
                SE_Text* t = (SE_Text*)sym;
                SE_RenderText* rt = (SE_RenderText*)rsym;
                rt->text = t->textExpr.evaluate(m_exec);
                rt->position[0] = t->position[0].evaluate(m_exec)*mm2px;
                rt->position[1] = t->position[1].evaluate(m_exec)*mm2px;//TODO: take into account y-up or y-down!
                xform.transform(rt->position[0], rt->position[1]);

                rt->tdef.font().name() =  t->fontExpr.evaluate(m_exec);
                rt->tdef.font().height() = t->size.evaluate(m_exec)*0.001*xform.y1/mm2px; //convert mm to meters which is what RS_TextDef expects
                rt->tdef.linespace() = t->lineSpacing.evaluate(m_exec);
                rt->tdef.rotation() = t->angle.evaluate(m_exec);

                int style = RS_FontStyle_Regular;

                if (t->underlined.evaluate(m_exec)) style |= (int)RS_FontStyle_Underline;
                if (t->italic.evaluate(m_exec)) style |= (int)RS_FontStyle_Italic;
                if (t->bold.evaluate(m_exec)) style |= (int)RS_FontStyle_Bold;

                rt->tdef.font().style() = (RS_FontStyle_Mask)style;

                rt->tdef.color() = RS_Color::FromARGB(t->textColor.evaluate(m_exec));
                rt->tdef.bgcolor() = RS_Color::FromARGB(t->ghostColor.evaluate(m_exec));

                const wchar_t* hAlign = t->hAlignment.evaluate(m_exec);
                if (hAlign)
                {
                    if (wcscmp(hAlign, L"Left") == 0)           
                        rt->tdef.halign() = RS_HAlignment_Left;
                    else if (wcscmp(hAlign, L"Center") == 0)    
                        rt->tdef.halign() = RS_HAlignment_Center;
                    else if (wcscmp(hAlign, L"Right") == 0)     
                        rt->tdef.halign() = RS_HAlignment_Right;
                }

                const wchar_t* vAlign = t->vAlignment.evaluate(m_exec);
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
                m_renderer->GetFontEngine()->GetTextMetrics(rt->text, rt->tdef, tm, false);
                txf.rotate(rt->tdef.rotation()*M_PI/180.0);
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
                    rr->pngPtr = m_resources->GetImageData(r->pngPath.evaluate(m_exec), rr->pngSize);
                }
                else
                {
                    rr->pngPtr = r->pngPtr;
                    rr->pngSize = r->pngSize;
                }

                rr->position[0] = r->position[0].evaluate(m_exec)*mm2px;
                rr->position[1] = r->position[1].evaluate(m_exec)*mm2px;
                xform.transform(rr->position[0], rr->position[1]);
                rr->extent[0] = r->extent[0].evaluate(m_exec)*xform.x0;
                rr->extent[1] = r->extent[1].evaluate(m_exec)*xform.y1;
                rr->angle = r->angle.evaluate(m_exec)*(M_PI/180.0);

                rr->bounds = m_pool->NewBounds(5);
                SE_Matrix rxf;
                rxf.rotate(rr->angle);
                rxf.translate(rr->position[0], rr->position[1]);

                double w = rr->extent[0]/2.0;
                double h = rr->extent[1]/2.0;
                double sx, sy, x, y;

                rxf.transform(w, h, sx, sy); // upper right
                rr->bounds->Add(sx, sy);
                rxf.transform(w, -h, x, y); // lower right
                rr->bounds->Add(x, y);
                rxf.transform(-w, -h, x, y); // lower left
                rr->bounds->Add(x, y);
                rxf.transform(-w, h, x, y); // upper left
                rr->bounds->Add(x, y);
                rr->bounds->Add(sx, sy); // upper right
            }
            break;
        default: break;
        }

        if (rsym)
        {
            if (renderStyle->bounds)
            {
                SE_Bounds* bounds = renderStyle->bounds;
                renderStyle->bounds = bounds->Union(rsym->bounds);
                bounds->Free();
            }
            else
                renderStyle->bounds = rsym->bounds->Clone();
            rsym->resize = sym->resize == GraphicElement::AdjustToResizeBox;
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

        if (growx != 0 || growy != 0)
        {
            SE_Matrix totalxf(xform);
            SE_Matrix growxf;
            growxf.translate(-dx, -dy);
            growxf.scale(1.0 + growx, 1.0 + growy);
            growxf.translate(dx, dy);
            totalxf.premultiply(growxf);

            for (SE_RenderSymbol::iterator rs = renderStyle->symbol.begin(); rs != renderStyle->symbol.end(); rs++)
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
                            rp->bounds = rp->geometry->xf_bounds();
                            break;
                        }
                    case SE_TextPrimitive:
                        {
                            SE_RenderText* rt = (SE_RenderText*)rsym;
                            growxf.transform(rt->position[0], rt->position[1]);
                            rt->tdef.font().height() *= growxf.y1;
                            break;
                        }
                    case SE_RasterPrimitive:
                        {
                            SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                            growxf.transform(rr->position[0], rr->position[1]);
                            rr->extent[0] *= growxf.x0;
                            rr->extent[1] *= growxf.y1;
                            break;
                        }
                    }
                }
            }
        }
    }
}


void StylizationEngine::LayoutCustomLabel(const std::wstring& positioningAlgo, SE_LineBuffer* geometry, SE_Matrix& xform, SE_Style* style, SE_RenderStyle* rstyle, double mm2px)
{
    //here we decide which one to call based on the name of the positioning algorithm
    if (positioningAlgo == L"EightSurrounding")
    {
        SE_PositioningAlgorithms::EightBall(m_renderer, geometry, xform, style, rstyle, mm2px);
    }
    //else if (style->positioningAlgorithm == MultipleHighwayShields)
    //{

    //}
     
}

//clears cached filters/styles/etc
void StylizationEngine::ClearCache()
{
    
    std::map<CompositeTypeStyle*, SE_Rule*>::iterator iter = m_rules.begin();

    for (; iter != m_rules.end(); iter++)
    {
        //TODO: deleting the SE_Rule causes a heap corruption that needs to be tracked down
        //delete iter->second;
    }

    m_rules.clear();
}
