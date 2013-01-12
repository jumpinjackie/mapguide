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

#include "stdafx.h"
#include "StylizationEngine.h"
#include "SE_BufferPool.h"
#include "SE_StyleVisitor.h"
#include "SE_Renderer.h"
#include "SE_SymbolManager.h"
#include "SE_PositioningAlgorithms.h"
#include "SE_SymbolDefProxies.h"
#include "FeatureTypeStyleVisitor.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#include <cstdio>
#endif

using namespace MDFMODEL_NAMESPACE;

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

StylizationEngine::StylizationEngine(SE_SymbolManager* resources, SE_BufferPool* pool) :
    m_resources(resources),
    m_pool(pool),
    m_serenderer(NULL),
    m_reader(NULL)
{
    m_visitor = new SE_StyleVisitor(resources, m_pool);
}


StylizationEngine::~StylizationEngine()
{
    ClearCache();
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

    double drawingScale = m_serenderer->GetDrawingScale();

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    if (se_renderer->SupportsTooltips())
        m_visitor->ParseStringExpression(layer->GetToolTip(), seTip, L"");
    if (se_renderer->SupportsHyperlinks())
        m_visitor->ParseStringExpression(layer->GetUrlData() ? layer->GetUrlData()->GetUrlContent(): L"", seUrl, L"");

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

    // ignore Z values if the renderer doesn't need them
    bool ignoreZ = !se_renderer->SupportsZ();

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

            LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pool, 8, FdoDimensionality_Z, ignoreZ);
            if (!lb)
                continue;

            std::auto_ptr<LineBuffer> spLB(lb);

            // tell line buffer the current drawing scale (used for arc tessellation)
            lb->SetDrawingScale(drawingScale);

            try
            {
                if (!reader->IsNull(gpName))
                    reader->GetGeometry(gpName, lb, xformer);
                else
                {
                    // just move on to the next feature
                    LineBufferPool::FreeLineBuffer(m_pool, spLB.release());
                    continue;
                }
            }
            catch (FdoException* e)
            {
                // just move on to the next feature
                e->Release();
                LineBufferPool::FreeLineBuffer(m_pool, spLB.release());
                continue;
            }

            // stylize once for each composite type style
            for (size_t i=0; i<numTypeStyles; ++i)
            {
                bool initialPass = (i == 0 && instanceRenderingPass == 0 && symbolRenderingPass == 0);
                Stylize(reader, exec, lb, compTypeStyles[i], &seTip, &seUrl, NULL,
                        initialPass, instanceRenderingPass, symbolRenderingPass,
                        nextInstanceRenderingPass, nextSymbolRenderingPass);
            }

            // free geometry when done stylizing
            LineBufferPool::FreeLineBuffer(m_pool, spLB.release());

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


// opaque is a double between 0 and 1.
// 0 means totally transparent, while 1 means totally opaque.
// The caller should be responsible for validating opaque value.
inline unsigned int TransparentColor(unsigned int argb, double opaque)
{
    //unsigned int alpha = (unsigned int)(((argb >> 24) & 0xFF) * opaque);
    return argb & 0xFFFFFF
        | (((unsigned int)(((argb >> 24) & 0xFF)* opaque)) << 24);
}


// Unit can't be Pixels.
// The caller should be responsible for not input pixel
inline double GetUnitPerMeter(WatermarkOffset::WatermarkOffsetUnit unit)
{
    switch(unit)
    {
    case WatermarkOffset::Centimeters:
        return LengthConverter::MetersToUnit(MdfModel::Centimeters, 1);
        break;
    case WatermarkOffset::Inches:
        return LengthConverter::MetersToUnit(MdfModel::Inches, 1);
        break;
    case WatermarkOffset::Millimeters:
        return LengthConverter::MetersToUnit(MdfModel::Millimeters, 1);
        break;
    case WatermarkOffset::Points:
        return LengthConverter::MetersToUnit(MdfModel::Points, 1);
        break;
    default:
        return 1;
        break;
    }
}


void StylizationEngine::StylizeWatermark(SE_Renderer* se_renderer,
                                         WatermarkDefinition* watermark,
                                         int drawWidth, int drawHeight,
                                         int saveWidth, int saveHeight)
{
    m_serenderer = se_renderer;
    m_reader = NULL;

    double drawingScale = m_serenderer->GetDrawingScale();

    // get tooltip and url for the layer
    SE_String seTip;
    SE_String seUrl;
    if (se_renderer->SupportsTooltips())
        m_visitor->ParseStringExpression(L"", seTip, L"");
    if (se_renderer->SupportsHyperlinks())
        m_visitor->ParseStringExpression(L"", seUrl, L"");

    double transparency = watermark->GetAppearance()->GetTransparency();
    transparency = (transparency < 0.0)? 0.0 : ((transparency > 100.0)? 100.0 : transparency);
    double opacity = 1.0 - 0.01*transparency;

    const double MIMIMUM_RENDERING_OPACITY = 0.001;
    if (opacity < MIMIMUM_RENDERING_OPACITY)   // Not render when totally transparent
        return;
    double rotation = watermark->GetAppearance()->GetRotation();
    rotation = (rotation < 0.0)? 0.0 : ((rotation > 360.0)? 360.0 : rotation);

    SE_Rule rule;

    // Translate watermark source into SE_SymbolInstance list.
    // As the source is adopted into symbol, we need to detach them after
    // the rendering is done.
    CompositeSymbolization symbols;

    std::auto_ptr<SymbolInstance> instance(new SymbolInstance());
    instance->AdoptSymbolDefinition(watermark->GetContent());
    instance->SetUsageContext(SymbolInstance::ucPoint);
    symbols.GetSymbolCollection()->Adopt(instance.release());

    m_visitor->Convert(rule.symbolInstances, &symbols);
    _ASSERT(rule.symbolInstances.size() == 1u);

    // translate appearance (transparency / rotation) into symbol instance
    SE_SymbolInstance* sym = rule.symbolInstances[0];
    size_t nStyles = sym->styles.size();
    for (size_t styleIx=0; styleIx<nStyles; ++styleIx)
    {
        SE_PointStyle* style = (SE_PointStyle*)(sym->styles[styleIx]);
        style->angleDeg.value = style->angleDeg.defValue = style->angleDeg.defValue + rotation;
        if (style->symbol.size() == 0)
            continue;

        size_t nPrimitives = style->symbol.size();
        for (size_t primitiveIx=0; primitiveIx<nPrimitives; ++primitiveIx)
        {
            SE_Primitive* primitive = style->symbol[primitiveIx];
            SE_Text* textPri = dynamic_cast<SE_Text*>(primitive);
            SE_Polygon* polygonPri = dynamic_cast<SE_Polygon*>(primitive);
            SE_Polyline* linePri = dynamic_cast<SE_Polyline*>(primitive);
            SE_Raster* rasterPri = dynamic_cast<SE_Raster*>(primitive);
            if (textPri)
            {
                // text needs to change color
                textPri->textColor.value.argb      = textPri->textColor.defValue.argb      = TransparentColor(textPri->textColor.value.argb, opacity);
                textPri->ghostColor.value.argb     = textPri->ghostColor.defValue.argb     = TransparentColor(textPri->ghostColor.value.argb, opacity);
                textPri->frameLineColor.value.argb = textPri->frameLineColor.defValue.argb = TransparentColor(textPri->frameLineColor.value.argb, opacity);
                textPri->frameFillColor.value.argb = textPri->frameFillColor.defValue.argb = TransparentColor(textPri->frameFillColor.value.argb, opacity);
            }
            else if (linePri)
            {
                linePri->color.value.argb = linePri->color.defValue.argb = TransparentColor(linePri->color.value.argb, opacity);
                if (polygonPri)
                    polygonPri->fill.value.argb = polygonPri->fill.defValue.argb = TransparentColor(polygonPri->fill.value.argb, opacity);
            }
            else if (rasterPri)
            {
                rasterPri->opacity = opacity;
            }
        }
    }

    // prepare some rendering context variable
    double mm2sud = m_serenderer->GetScreenUnitsPerMillimeterDevice();
    double mm2suw = m_serenderer->GetScreenUnitsPerMillimeterWorld();
    double px2su  = m_serenderer->GetScreenUnitsPerPixel();
    bool yUp = m_serenderer->YPointsUp();

    // the factor to convert screen units to mapping units
    double su2wu = 0.001 / (mm2suw * m_serenderer->GetMetersPerUnit());

    // prepare the position list
    XYWatermarkPosition* xyPosition = dynamic_cast<XYWatermarkPosition*>(watermark->GetPosition());
    TileWatermarkPosition* tilePosition = dynamic_cast<TileWatermarkPosition*>(watermark->GetPosition());
    WatermarkXOffset::HorizontalAlignment hAlignment = WatermarkXOffset::Center;
    WatermarkYOffset::VerticalAlignment vAlignment = WatermarkYOffset::Center;
    WatermarkOffset::WatermarkOffsetUnit hUnit = WatermarkOffset::Pixels;
    WatermarkOffset::WatermarkOffsetUnit vUnit = WatermarkOffset::Pixels;
    double xOffset = 0.0;
    double yOffset = 0.0;
    if (xyPosition)
    {
        hAlignment = xyPosition->GetXPosition()->GetAlignment();
        vAlignment = xyPosition->GetYPosition()->GetAlignment();
        hUnit = xyPosition->GetXPosition()->GetUnit();
        vUnit = xyPosition->GetYPosition()->GetUnit();
        xOffset = xyPosition->GetXPosition()->GetOffset();              // in watermark units
        yOffset = xyPosition->GetYPosition()->GetOffset();              // in watermark units
    }
    else if (tilePosition)
    {
        hAlignment = tilePosition->GetHorizontalPosition()->GetAlignment();
        vAlignment = tilePosition->GetVerticalPosition()->GetAlignment();
        hUnit = tilePosition->GetHorizontalPosition()->GetUnit();
        vUnit = tilePosition->GetVerticalPosition()->GetUnit();
        xOffset = tilePosition->GetHorizontalPosition()->GetOffset();   // in watermark units
        yOffset = tilePosition->GetVerticalPosition()->GetOffset();     // in watermark units
    }
    double pixelPerMeterDevice = 1000.0 * mm2sud / px2su;
    double suPerhUnit = ((hUnit == WatermarkOffset::Pixels)? 1.0 : pixelPerMeterDevice / GetUnitPerMeter(hUnit)) * px2su;
    double suPervUnit = ((vUnit == WatermarkOffset::Pixels)? 1.0 : pixelPerMeterDevice / GetUnitPerMeter(vUnit)) * px2su;
    xOffset *= suPerhUnit;      // in screen units
    yOffset *= suPervUnit;      // in screen units

    std::vector<double> watermarkPosList;
    if (xyPosition)
    {
        switch (hAlignment)
        {
        case WatermarkXOffset::Right:
            xOffset = saveWidth - xOffset;
            break;
        case WatermarkXOffset::Left:
            break;
        default:
            xOffset += 0.5*saveWidth;
            break;
        }

        switch (vAlignment)
        {
        case WatermarkYOffset::Bottom:
            yOffset = yUp? yOffset : saveHeight - yOffset;
            break;
        case WatermarkYOffset::Top:
            yOffset = yUp? saveHeight - yOffset : yOffset;
            break;
        default:
            yOffset = 0.5*saveHeight + (yUp? yOffset : -yOffset);
            break;
        }

        xOffset *= drawWidth / saveWidth;
        yOffset *= drawHeight / saveHeight;
        double mapPosX, mapPosY;
        m_serenderer->ScreenToWorldPoint(xOffset, yOffset, mapPosX, mapPosY);
        watermarkPosList.push_back(mapPosX);
        watermarkPosList.push_back(mapPosY);
    }
    else if (tilePosition)
    {
        double tileWidth  = tilePosition->GetTileWidth()*px2su;
        double tileHeight = tilePosition->GetTileHeight()*px2su;

        switch (hAlignment)
        {
        case WatermarkXOffset::Right:
            xOffset = tileWidth - xOffset;
            break;
        case WatermarkXOffset::Left:
            break;
        default:
            xOffset += 0.5*tileWidth;
            break;
        }

        switch (vAlignment)
        {
        case WatermarkYOffset::Bottom:
            yOffset = yUp? yOffset : tileHeight - yOffset;
            break;
        case WatermarkYOffset::Top:
            yOffset = yUp? tileHeight - yOffset : yOffset;
            break;
        default:
            yOffset = 0.5*tileHeight + (yUp? yOffset : -yOffset);
            break;
        }

        double drawPosX, drawPosY, mapPosX, mapPosY;
        for (int i=0; i<=(int)(saveWidth/tileWidth); ++i)
        {
            for (int j=0; j<=(int)(saveHeight/tileHeight); ++j)
            {
                drawPosX = i*tileWidth+xOffset;
                drawPosY = yUp? (saveHeight-(j+1)*tileHeight+yOffset) : j*tileHeight+yOffset;
                xOffset *= drawWidth / saveWidth;
                yOffset *= drawHeight / saveHeight;
                m_serenderer->ScreenToWorldPoint(drawPosX, drawPosY, mapPosX, mapPosY);
                watermarkPosList.push_back(mapPosX);
                watermarkPosList.push_back(mapPosY);
            }
        }
    }

    // we always start with rendering pass 0
    int symbolRenderingPass = 0;
    int nextSymbolRenderingPass = -1;

    // main loop over feature data
    int numPasses = 0;
    while (symbolRenderingPass >= 0)
    {
        ++numPasses;

        // create an expression engine with our custom functions
        // NOTE: We must create a new engine with each rendering pass.  The engine
        //       stores a weak reference to the RS_FeatureReader's internal
        //       FdoIFeatureReader, and this internal reader is different for each
        //       pass.
        FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(se_renderer, NULL);

        std::auto_ptr<LineBuffer> spLB;
        size_t nPos = watermarkPosList.size();
        for (size_t posIx=0; posIx<nPos; posIx+=2)
        {
            //Get geometry
            LineBuffer* lb = LineBufferPool::NewLineBuffer(m_pool, 8, FdoDimensionality_Z);
            spLB.reset(lb);
            lb->MoveTo(watermarkPosList[posIx], watermarkPosList[posIx+1]);
            // tell line buffer the current drawing scale (used for arc tessellation)
            lb->SetDrawingScale(drawingScale);

            //Render line buffer

            // -------------------------------------------------------------------------
            //
            // Here's a description of how the transforms work for point symbols.
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
            // -------------------------------------------------------------------------

            // TODO: Obey the indices - get rid of the indices altogther - single pass!

            // For now always clip using the new stylization - the performance impact of not
            // clipping is too high.  We also need a better approach to clipping.  Instead
            // of clipping the feature geometry we need to calculate where to start/stop
            // drawing symbols.
            bool bClip = true;  //m_serenderer->RequiresClipping();
            double clipOffsetSU = 0.0;

            // Make a pass over all the instances.  During this pass we:
            // - evaluate the active styles
            // - compute the overall clip offset

            SE_Matrix xformScale;
            xformScale.scale(sym->scale[0].evaluate(exec),
                             sym->scale[1].evaluate(exec));

            // The symbol geometry needs to be inverted if the y coordinate in the renderer
            // points down.  This is so that in symbol definitions y points up consistently
            // no matter what the underlying renderer is doing.  Normally we could just apply
            // the world to screen transform to everything, but in some cases we only apply
            // it to the position of the symbol and then offset the symbol geometry from
            // there - so the symbol geometry needs to be pre-inverted.
            double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
            double mm2suY = yUp? mm2suX : -mm2suX;
            xformScale.scale(mm2suX, mm2suY);

            // initialize the style evaluation context
            SE_EvalContext evalCtx;
            evalCtx.exec = exec;
            evalCtx.mm2su = mm2suX;
            evalCtx.mm2sud = mm2sud;
            evalCtx.mm2suw = mm2suw;
            evalCtx.px2su = px2su;
            evalCtx.pool = m_pool;
            evalCtx.fonte = m_serenderer->GetRSFontEngine();
            evalCtx.xform = &xformScale;
            evalCtx.resources = m_resources;

            // iterate over all styles in the instance
            for (size_t styIx=0; styIx<nStyles; ++styIx)
            {
                SE_Style* style = sym->styles[styIx];

                // process the symbol rendering pass - negative rendering passes are
                // rendered with pass 0
                int symbolRenderPass = style->renderPass.evaluate(exec);
                if (symbolRenderPass < 0)
                    symbolRenderPass = 0;

                // if the rendering pass for the style doesn't match the current pass
                // then don't render using it
                if (symbolRenderPass != symbolRenderingPass)
                    continue;

                // evaluate the style (all expressions inside it) and convert to a
                // constant screen space render style
                style->evaluate(&evalCtx);
            }

            // Adjust the offset according to watermark position
            // For example, the watermark is on top/left, the original offset is enough.
            // However, if the watermark is on bottom/right, the symbols has to be added
            // an offset to make the bottom/right of their bounds to be the position.
            RS_F_Point bounds[4];
            bounds[0].x = bounds[3].x = +DBL_MAX;
            bounds[1].x = bounds[2].x = -DBL_MAX;
            bounds[0].y = bounds[1].y = +DBL_MAX;
            bounds[2].y = bounds[3].y = -DBL_MAX;
            for (size_t styIx=0; styIx<nStyles; ++styIx)
            {
                SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(
                    sym->styles[styIx]->rstyle);
                SE_Matrix xformStyle;

                // point usage offset (already scaled)
                xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);
                // point usage rotation - assume geometry angle is zero
                xformStyle.rotate(ptStyle->angleRad);
                // compute the offset
                for (int i=0; i<4; ++i)
                {
                    // account for the style-specific transform
                    RS_F_Point pt = ptStyle->bounds[i];
                    xformStyle.transform(pt.x, pt.y);
                    bounds[0].x = bounds[3].x = rs_min(bounds[0].x, pt.x);
                    bounds[1].x = bounds[2].x = rs_max(bounds[2].x, pt.x);
                    bounds[0].y = bounds[1].y = rs_min(bounds[0].y, pt.y);
                    bounds[2].y = bounds[3].y = rs_max(bounds[2].y, pt.y);
                }
            }

            // bounds[0].x is left, bounds[1].x is right
            switch (hAlignment)
            {
            case WatermarkXOffset::Right:
                sym->absOffset[0].value = sym->absOffset[0].defValue = -bounds[1].x / mm2suX;
                break;
            case WatermarkXOffset::Left:
                sym->absOffset[0].value = sym->absOffset[0].defValue = -bounds[0].x / mm2suX;
                break;
            default:
                sym->absOffset[0].value = sym->absOffset[0].defValue = -0.5*(bounds[0].x + bounds[1].x) / mm2suX;
                break;
            }

            // bounds[1].y is bottom, bounds[2].y is top
            switch (vAlignment)
            {
            case WatermarkYOffset::Bottom:
                sym->absOffset[1].value = sym->absOffset[1].defValue = (yUp? -1.0: 1.0) * bounds[1].y / mm2suY;
                break;
            case WatermarkYOffset::Top:
                sym->absOffset[1].value = sym->absOffset[1].defValue = (yUp? -1.0: 1.0) * bounds[2].y / mm2suY;
                break;
            default:
                sym->absOffset[1].value = sym->absOffset[1].defValue = (yUp? -1.0: 1.0) * 0.5*(bounds[1].y + bounds[2].y) / mm2suY;
                break;
            }

            // prepare the geometry on which we will apply the styles
            if (bClip)
            {
                // compute offset to apply to the clipping bounds
                for (size_t styIx=0; styIx<nStyles; ++styIx)
                {
                    SE_Style* style = sym->styles[styIx];
                    double styleClipOffsetSU = GetClipOffset(sym, style, exec, mm2suX, mm2suY);
                    clipOffsetSU = rs_max(styleClipOffsetSU, clipOffsetSU);
                }

                // compute the clip region to use - start with the map request extents
                RS_Bounds clip = m_serenderer->GetBounds();

                // add one pixel's worth to handle any roundoff
                clipOffsetSU += px2su;

                // limit the offset to something reasonable
                if (clipOffsetSU > MAX_CLIPOFFSET_IN_MM * mm2sud)
                    clipOffsetSU = MAX_CLIPOFFSET_IN_MM * mm2sud;

                // expand clip region by the offset
                double clipOffsetWU = clipOffsetSU * su2wu;
                clip.minx -= clipOffsetWU;
                clip.miny -= clipOffsetWU;
                clip.maxx += clipOffsetWU;
                clip.maxy += clipOffsetWU;

                // clip geometry to given extents
                LineBuffer* lbc = lb->Clip(clip, LineBuffer::ctAGF, m_pool);
                if (lbc != lb)
                {
                    // if the clipped buffer is NULL (completely clipped) just move on to
                    // the next feature
                    if (!lbc) continue;

                    // otherwise continue processing with the clipped buffer
                    lb = lbc;
                    spLB.reset(lb);
                }
            }

            // Make another pass over all the instances.  During this pass we:
            // - compute the next symbol rendering pass
            // - apply the styles to the geometry (original or clipped)

            // initialize the style application context
            SE_Matrix xformTrans;
            xformTrans.translate(sym->absOffset[0].evaluate(exec) * mm2suX,
                                 sym->absOffset[1].evaluate(exec) * mm2suY);

            SE_ApplyContext applyCtx;
            applyCtx.geometry = lb;
            applyCtx.renderer = m_serenderer;
            applyCtx.xform = &xformTrans;
            applyCtx.sizeContext = sym->sizeContext;

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

                // TODO: why are these in the symbol instance?
                style->rstyle->addToExclusionRegion = sym->addToExclusionRegion.evaluate(exec);
                style->rstyle->checkExclusionRegion = sym->checkExclusionRegion.evaluate(exec);
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


            if(spLB.get())
                LineBufferPool::FreeLineBuffer(m_pool, spLB.release());
        }

        // switch to the next symbol rendering pass
        symbolRenderingPass = nextSymbolRenderingPass;
        nextSymbolRenderingPass = -1;
    }

    // Detach symbol definition from the created composite symbol so that
    // it will not be finalized when composite symbol is finalized.
    // The code is sure there is only one symbol instance.
    _ASSERT(symbols.GetSymbolCollection()->GetCount() == 1);
    symbols.GetSymbolCollection()->GetAt(0)->OrphanSymbolDefinition();
}


void StylizationEngine::Stylize(RS_FeatureReader* reader,
                                FdoExpressionEngine* exec,
                                LineBuffer* geometry,
                                CompositeTypeStyle* style,
                                SE_String* seTip,
                                SE_String* seUrl,
                                RS_ElevationSettings* /*elevSettings*/,
                                bool initialPass,
                                int instanceRenderingPass,
                                int symbolRenderingPass,
                                int& nextInstanceRenderingPass,
                                int& nextSymbolRenderingPass)
{
    m_reader = reader;

    SE_Rule*& rules = m_rules[style];
    RuleCollection* rulecoll = style->GetRules();
    int nRules = rulecoll->GetCount();

    // populate the rule collection if this is our first time
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

    // get the active rule for the current feature
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

    // we found a valid rule - send a StartFeature notification
    RS_String rs_tip, rs_url;
    if (seTip->expression || wcslen(seTip->getValue()) > 0)
        rs_tip = seTip->evaluate(exec);
    if (seUrl->expression || wcslen(seUrl->getValue()) > 0)
        rs_url = seUrl->evaluate(exec);
    RS_String& rs_thm = rule->legendLabel;
    m_serenderer->StartFeature(reader, initialPass, rs_tip.empty()? NULL : &rs_tip, rs_url.empty()? NULL : &rs_url, rs_thm.empty()? NULL : &rs_thm);

    // Get the symbol instances from the rule.  It's possible to end
    // up with no symbols - we're done in that case.
    std::vector<SE_SymbolInstance*>* symbolInstances = &rule->symbolInstances;
    if (symbolInstances->size() == 0)
        return;

    double mm2sud = m_serenderer->GetScreenUnitsPerMillimeterDevice();
    double mm2suw = m_serenderer->GetScreenUnitsPerMillimeterWorld();
    double px2su  = m_serenderer->GetScreenUnitsPerPixel();
    bool yUp = m_serenderer->YPointsUp();

    // the factor to convert screen units to mapping units
    double su2wu = 0.001 / (mm2suw * m_serenderer->GetMetersPerUnit());

    // -------------------------------------------------------------------------
    //
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
    //
    // -------------------------------------------------------------------------

    // TODO: Obey the indices - get rid of the indices altogther - single pass!

    // For now always clip using the new stylization - the performance impact of not
    // clipping is too high.  We also need a better approach to clipping.  Instead
    // of clipping the feature geometry we need to calculate where to start/stop
    // drawing symbols.
    bool bClip = true;  //m_serenderer->RequiresClipping();
    double clipOffsetSU = 0.0;

    // Make a pass over all the instances.  During this pass we:
    // - evaluate the active styles
    // - compute the overall clip offset
    SE_SymbolInstance* sym = NULL;
    size_t nSyms = symbolInstances->size();
    for (size_t symIx=0; symIx<nSyms; ++symIx)
    {
        sym = (*symbolInstances)[symIx];

        // process the instance rendering pass - negative rendering passes
        // are rendered with pass 0
        int instanceRenderPass = sym->renderPass.evaluate(exec);
        if (instanceRenderPass < 0)
            instanceRenderPass = 0;

        // if the rendering pass for the instance doesn't match the current
        // instance pass then don't render using it
        if (instanceRenderPass != instanceRenderingPass)
            continue;

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

        // The symbol geometry needs to be inverted if the y coordinate in the renderer
        // points down.  This is so that in symbol definitions y points up consistently
        // no matter what the underlying renderer is doing.  Normally we could just apply
        // the world to screen transform to everything, but in some cases we only apply
        // it to the position of the symbol and then offset the symbol geometry from
        // there - so the symbol geometry needs to be pre-inverted.
        double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
        double mm2suY = yUp? mm2suX : -mm2suX;
        xformScale.scale(mm2suX, mm2suY);

        // initialize the style evaluation context
        SE_EvalContext evalCtx;
        evalCtx.exec = exec;
        evalCtx.mm2su = mm2suX;
        evalCtx.mm2sud = mm2sud;
        evalCtx.mm2suw = mm2suw;
        evalCtx.px2su = px2su;
        evalCtx.pool = m_pool;
        evalCtx.fonte = m_serenderer->GetRSFontEngine();
        evalCtx.xform = &xformScale;
        evalCtx.resources = m_resources;

        // iterate over all styles in the instance
        size_t nStyles = sym->styles.size();
        for (size_t styIx=0; styIx<nStyles; ++styIx)
        {
            SE_Style* style = sym->styles[styIx];

            // process the symbol rendering pass - negative rendering passes are
            // rendered with pass 0
            int symbolRenderPass = style->renderPass.evaluate(exec);
            if (symbolRenderPass < 0)
                symbolRenderPass = 0;

            // if the rendering pass for the style doesn't match the current pass
            // then don't render using it
            if (symbolRenderPass != symbolRenderingPass)
                continue;

            // evaluate the style (all expressions inside it) and convert to a
            // constant screen space render style
            style->evaluate(&evalCtx);

            // compute offset to apply to the clipping bounds
            if (bClip)
            {
                double styleClipOffsetSU = GetClipOffset(sym, style, exec, mm2suX, mm2suY);
                clipOffsetSU = rs_max(styleClipOffsetSU, clipOffsetSU);
            }
        }
    }

    // prepare the geometry on which we will apply the styles
    LineBuffer* lb = geometry;
    std::auto_ptr<LineBuffer> spClipLB;

    if (bClip)
    {
        // compute the clip region to use - start with the map request extents
        RS_Bounds clip = m_serenderer->GetBounds();

        // add one pixel's worth to handle any roundoff
        clipOffsetSU += px2su;

        // limit the offset to something reasonable
        if (clipOffsetSU > MAX_CLIPOFFSET_IN_MM * mm2sud)
            clipOffsetSU = MAX_CLIPOFFSET_IN_MM * mm2sud;

        // expand clip region by the offset
        double clipOffsetWU = clipOffsetSU * su2wu;
        clip.minx -= clipOffsetWU;
        clip.miny -= clipOffsetWU;
        clip.maxx += clipOffsetWU;
        clip.maxy += clipOffsetWU;

        // clip geometry to given extents
        LineBuffer* lbc = lb->Clip(clip, LineBuffer::ctAGF, m_pool);
        if (lbc != lb)
        {
            // if the clipped buffer is NULL (completely clipped) just move on to
            // the next feature
            if (!lbc)
                return;

            // otherwise continue processing with the clipped buffer
            lb = lbc;
            if (lb != geometry)
                spClipLB.reset(lb);
        }
    }

    // don't bother rendering empty feature geometry
    if (lb->point_count())
    {
        // Make another pass over all the instances.  During this pass we:
        // - compute the next instance / symbol rendering pass
        // - apply the styles to the geometry (original or clipped)
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

//              case FdoGeometryType_MultiGeometry:
//                  continue;
//                  break;
                }
            }

            double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
            double mm2suY = yUp? mm2suX : -mm2suX;

            // initialize the style application context
            SE_Matrix xformTrans;
            xformTrans.translate(sym->absOffset[0].evaluate(exec) * mm2suX,
                                 sym->absOffset[1].evaluate(exec) * mm2suY);

            SE_ApplyContext applyCtx;
            applyCtx.geometry = lb;
            applyCtx.renderer = m_serenderer;
            applyCtx.xform = &xformTrans;
            applyCtx.sizeContext = sym->sizeContext;

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

                // TODO: why are these in the symbol instance?
                style->rstyle->addToExclusionRegion = sym->addToExclusionRegion.evaluate(exec);
                style->rstyle->checkExclusionRegion = sym->checkExclusionRegion.evaluate(exec);
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

    // free clipped line buffer if the geometry was clipped
    if (spClipLB.get())
        LineBufferPool::FreeLineBuffer(m_pool, spClipLB.release());
}


// This method returns the amount that the clip region needs to be expanded
// to ensure that the clipped geometry renders correctly.  For example, if
// we clip a polygon directly to the edge of the screen then the polygon's
// edge style will be visible along any clipped edges.  But we don't want
// any style to display on these clipped edges since they are not part of
// the actual feature geometry.  To fix this we expand the clip region so
// that all clipped edges are sufficiently offscreen and any style applied
// to them does not display in the map region.  This method computes the
// required clip offset for the supplied style.  The value returned is in
// screen units.
double StylizationEngine::GetClipOffset(SE_SymbolInstance* sym, SE_Style* style, FdoExpressionEngine* exec, double mm2suX, double mm2suY)
{
    double clipOffsetSU = 0.0;

    // if the render style has primitives then we can check their bounds
    SE_RenderStyle* rStyle = style->rstyle;
    if (rStyle->symbol.size() > 0)
    {
        // Each style type has additional transformations associated with it.  See
        // StylizationEngine::Stylize for a detailed explanation of these transforms.
        switch (rStyle->type)
        {
            case SE_RenderStyle_Point:
            {
                SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(rStyle);
                SE_Matrix xformStyle;

                // point usage offset (already scaled)
                xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);

                // point usage rotation - assume geometry angle is zero
                xformStyle.rotate(ptStyle->angleRad);

                // symbol instance offset
                xformStyle.translate(sym->absOffset[0].evaluate(exec) * mm2suX,
                                     sym->absOffset[1].evaluate(exec) * mm2suY);

                // compute the offset
                for (int i=0; i<4; ++i)
                {
                    // account for the style-specific transform
                    RS_F_Point pt = rStyle->bounds[i];
                    xformStyle.transform(pt.x, pt.y);

                    // for point styles the clip offset is the maximum
                    // x-/y-extent of the symbol
                    clipOffsetSU = rs_max(fabs(pt.x), clipOffsetSU);
                    clipOffsetSU = rs_max(fabs(pt.y), clipOffsetSU);
                }

                break;
            }

            case SE_RenderStyle_Line:
            {
                SE_RenderLineStyle* lnStyle = (SE_RenderLineStyle*)(rStyle);
                SE_Matrix xformStyle;

                // line usage rotation - assume geometry angle is zero
                xformStyle.rotate(lnStyle->angleRad);

                // compute the offset
                for (int i=0; i<4; ++i)
                {
                    // account for the style-specific transform
                    RS_F_Point pt = rStyle->bounds[i];
                    xformStyle.transform(pt.x, pt.y);

                    // for line styles the clip offset is the maximum
                    // y-extent of the symbol
                    clipOffsetSU = rs_max(fabs(pt.y), clipOffsetSU);
                }

                break;
            }

            case SE_RenderStyle_Area:
            {
                SE_RenderAreaStyle* arStyle = (SE_RenderAreaStyle*)(rStyle);

                // only area styles with inside clipping control require an offset
                if (arStyle->clippingControl == SE_ClippingControl_Inside)
                {
                    SE_Matrix xformStyle;

                    // area usage rotation - assume geometry angle is zero
                    xformStyle.rotate(arStyle->angleRad);

                    // compute the offset
                    for (int i=0; i<4; ++i)
                    {
                        // account for the style-specific transform
                        RS_F_Point pt = rStyle->bounds[i];
                        xformStyle.transform(pt.x, pt.y);

                        // for area styles the clip offset is the maximum
                        // x-/y-extent of the symbol
                        clipOffsetSU = rs_max(fabs(pt.x), clipOffsetSU);
                        clipOffsetSU = rs_max(fabs(pt.y), clipOffsetSU);
                    }
                }
            }

            default:
                break;
        }
    }

    return clipOffsetSU;
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

    for (; iter != m_rules.end(); ++iter)
        delete [] iter->second;

    m_rules.clear();
}
