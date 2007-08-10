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
#include "SE_StyleVisitor.h"
#include "SE_SymbolManager.h"
#include "SE_LineBuffer.h"
#include "SE_BufferPool.h"
#include "SE_SymbolDefProxies.h"

#include <wctype.h>

using namespace MDFMODEL_NAMESPACE;


#define SMALL_ANGLE(s,e) ((s) < M_PI ? ((e) >= (s)) && ((e) < M_PI + (s)) : ((e) >= (s)) || ((s) < (s) - M_PI))


struct ArcDefinition
{
    double x0, y0, x1, y1;
    double rx, ry;
    double rotation;
    bool largeArc, clockwise;
};

struct ArcData
{
    double cx, cy;
    double startAng, endAng;
};

bool ParseArc(ArcDefinition& def, ArcData& data);


SE_StyleVisitor::SE_StyleVisitor(SE_SymbolManager* resources, SE_BufferPool* bp)
{
    m_resources = resources;
    m_bp = bp;
    m_primitive = NULL;
    m_symbolization = NULL;
    m_style = NULL;
}


SE_PointStyle* SE_StyleVisitor::ProcessPointUsage(PointUsage& pointUsage)
{
    SE_PointStyle* style = new SE_PointStyle();
    ParseStringExpression(pointUsage.GetAngleControl(), style->angleControl, L"FromAngle");
    ParseDoubleExpression(pointUsage.GetAngle(), style->angleDeg, 0.0);
    ParseDoubleExpression(pointUsage.GetOriginOffsetX(), style->originOffset[0], 0.0);
    ParseDoubleExpression(pointUsage.GetOriginOffsetY(), style->originOffset[1], 0.0);

    // set flag if all properties are constant
    style->cacheable = !(style->angleDeg.expression
                      || style->angleControl.expression
                      || style->originOffset[0].expression
                      || style->originOffset[1].expression);

    return style;
}


SE_LineStyle* SE_StyleVisitor::ProcessLineUsage(LineUsage& lineUsage)
{
    SE_LineStyle* style = new SE_LineStyle();
    ParseStringExpression(lineUsage.GetAngleControl(), style->angleControl, L"FromGeometry");
    ParseStringExpression(lineUsage.GetUnitsControl(), style->unitsControl, L"Absolute");
    ParseStringExpression(lineUsage.GetVertexControl(), style->vertexControl, L"OverlapNone");
    ParseDoubleExpression(lineUsage.GetAngle(), style->angleDeg, 0.0);
    ParseDoubleExpression(lineUsage.GetStartOffset(), style->startOffset, 0.0);
    ParseDoubleExpression(lineUsage.GetEndOffset(), style->endOffset, 0.0);
    ParseDoubleExpression(lineUsage.GetRepeat(), style->repeat, 0.0);
    ParseDoubleExpression(lineUsage.GetVertexAngleLimit(), style->vertexAngleLimit, 0.0);
    ParseStringExpression(lineUsage.GetVertexJoin(), style->vertexJoin, L"Round");
    ParseDoubleExpression(lineUsage.GetVertexMiterLimit(), style->vertexMiterLimit, 5.0);

    // set flag if all properties are constant
    style->cacheable = !(style->angleDeg.expression
                      || style->angleControl.expression
                      || style->unitsControl.expression
                      || style->vertexControl.expression
                      || style->startOffset.expression
                      || style->endOffset.expression
                      || style->repeat.expression
                      || style->vertexAngleLimit.expression
                      || style->vertexJoin.expression
                      || style->vertexMiterLimit.expression);

    return style;
}


SE_AreaStyle* SE_StyleVisitor::ProcessAreaUsage(AreaUsage& areaUsage)
{
    SE_AreaStyle* style = new SE_AreaStyle();
    ParseStringExpression(areaUsage.GetAngleControl(), style->angleControl, L"FromAngle");
    ParseStringExpression(areaUsage.GetOriginControl(), style->originControl, L"Global");
    ParseStringExpression(areaUsage.GetClippingControl(), style->clippingControl, L"Clip");
    ParseDoubleExpression(areaUsage.GetAngle(), style->angleDeg, 0.0);
    ParseDoubleExpression(areaUsage.GetOriginX(), style->origin[0], 0.0);
    ParseDoubleExpression(areaUsage.GetOriginY(), style->origin[1], 0.0);
    ParseDoubleExpression(areaUsage.GetRepeatX(), style->repeat[0], 0.0);
    ParseDoubleExpression(areaUsage.GetRepeatY(), style->repeat[1], 0.0);
    ParseDoubleExpression(areaUsage.GetBufferWidth(), style->bufferWidth, 0.0);

    // set flag if all properties are constant
    style->cacheable = !(style->angleDeg.expression
                      || style->angleControl.expression
                      || style->originControl.expression
                      || style->clippingControl.expression
                      || style->origin[0].expression
                      || style->origin[1].expression
                      || style->repeat[0].expression
                      || style->repeat[1].expression
                      || style->bufferWidth.expression);

    return style;
}


bool SE_StyleVisitor::ParseDouble(const wchar_t*& str, double& val)
{
    size_t length = 0;
    swscanf(str, L" %lf%n", &val, &length);
    str += length;
    return length > 0;
}


bool SE_StyleVisitor::ParseDoublePair(const wchar_t*& str, double& x, double& y)
{
    size_t length = 0;
    swscanf(str, L" %lf , %lf%n", &x, &y, &length);
    str += length;
    return length > 0;
}


bool SE_StyleVisitor::ParseGeometry(const MdfString& geometry, SE_LineBuffer& buffer)
{
    const wchar_t* data = geometry.c_str();
    const wchar_t* end = data + geometry.size();

    double x, y;
    double lx = 0.0, ly = 0.0;
    double sx, sy, rx, ry, rot, large, cw;

    SE_LineBuffer* lb = &buffer;

    wchar_t tag, ptag = 0;

    for (;data < end; )
    {
        if (iswspace(*data))
        {
            data++;
            continue;
        }

        tag = *data++;

TagSwitch:
        switch (tag)
        {
        case L'm': // Move relative x,y
            if (lb->Empty() || !ParseDoublePair(data, x, y))
                return false;
            lx += x;
            ly += y;
            lb->MoveTo(lx, ly);
            break;
        case L'M': // Move absolute x,y
            if (!ParseDoublePair(data, lx, ly))
                return false;
            lb->MoveTo(lx, ly);
            break;
        case L'l': // Line relative x,y
            if (lb->Empty() || !ParseDoublePair(data, x, y))
                return false;
            lx += x;
            ly += y;
            lb->LineTo(lx, ly);
            break;
        case L'L': // Line absolute x,y
            if (!ParseDoublePair(data, lx, ly))
                return false;
            lb->LineTo(lx, ly);
            break;
        case L'h': // Horizontal line relative x
            if (lb->Empty() || !ParseDouble(data, x))
                return false;
            lx += x;
            lb->LineTo(lx, ly);
            break;
        case L'H': // Horizontal line absolute x
            if (!ParseDouble(data, lx))
                return false;
            lb->LineTo(lx, ly);
            break;
        case L'v': // Vertical line relative y
            if (lb->Empty() || !ParseDouble(data, y))
                return false;
            ly += y;
            lb->LineTo(lx, ly);
            break;
        case L'V': // Vertical line absolute y
            if (!ParseDouble(data, ly))
                return false;
            lb->LineTo(lx, ly);
            break;
        case L'a': // Arc segment relative (rx,ry rotation arc,sweep x,y)
            if (lb->Empty())
                return false;
        case L'A': // Arc segment absolute (rx,ry rotation isLarge isclockwise x,y)
            if (!ParseDoublePair(data, rx, ry))
                return false;
            if (!ParseDouble(data, rot))
                return false;
            if (!ParseDouble(data, large))
                return false;
            if (!ParseDouble(data, cw))
                return false;
            if (!ParseDoublePair(data, x, y))
                return false;

            sx = lx;
            sy = ly;

            if (tag == 'a')
            {
                lx += x;
                ly += y;
            }
            else
            {
                lx = x;
                ly = y;
            }

            if (rx == 0.0 || ry == 0.0)
            {
                lb->LineTo(lx, ly);
                break;
            }

            ArcDefinition arcdef;
            ArcData arcdata;
            arcdef.x0 = sx; arcdef.y0 = sy;
            arcdef.x1 = lx; arcdef.y1 = ly;
            arcdef.rx = rx; arcdef.ry = ry;
            arcdef.clockwise = cw != 0; arcdef.largeArc = large != 0;
            arcdef.rotation = rot;
            if (!ParseArc(arcdef, arcdata))
                return false;

            lb->EllipticalArcTo(arcdata.cx, arcdata.cy, arcdef.rx, arcdef.ry, arcdata.startAng, arcdata.endAng, arcdef.rotation);
            break;
        case L'z': // Line segment to start point of figure
        case L'Z':
            lb->Close();
            break;
        // Instead of another tag, more data representing another record of the same type
        default:
            if (ptag == 0)
                return false;
            data--;
            tag = ptag;
            goto TagSwitch;
        }
        ptag = tag;
    }

    return true;
}


bool ParseArc(ArcDefinition& def, ArcData& data)
{
    double x0 = def.x0, y0 = def.y0;
    double x1 = def.x1, y1 = def.y1;
    double rx = def.rx, ry = def.ry;
    double rotrad = 0.0;

    if (def.rotation != 0.0)
    {
        rotrad = def.rotation * M_PI180;
        /* Derotate the points, so we can handle only the axis-oriented case */
        double sine = sin(-rotrad);
        double cosine = cos(-rotrad);
        x0 = def.x0*cosine + def.y0*sine;
        y0 = def.y0*cosine - def.x0*sine;
        x1 = def.x1*cosine + def.y1*sine;
        y1 = def.y1*cosine - def.x1*sine;
    }

    /* Laboriously calculate center */
    double x02 = x0*x0;
    double x12 = x1*x1;
    double x012 = x02 - x12;
    double rx2 = rx*rx;
    double ry2 = ry*ry;
    double rxy2 = rx2*ry2;
    double rx4 = rx2*rx2;
    double ry4 = ry2*ry2;
    double dx = (x0-x1);
    double dx2 = dx*dx;
    double dy = (y0-y1);
    double sy = (y0+y1);
    double dy2 = dy*dy;

    /* Solution to system of equations {(x0 - cx)^2/a^2 + (y0 - cy)^2/b^2 = 1,
                                        (x1 - cx)^2/a^2 + (y1 - cy)^2/b^2 = 1}
       verified with Mathematica. */

    double a = 2.0*(ry2*dx2 + rx2*dy2);
    double ay = rx2*a;
    double ax = ry2*dx*a;
    double sq = sqrt(rxy2*dx2*(-ry4*dx2*dx2 + 2.0*rxy2*dx2*(2.0*ry2 - dy2) - rx2*rx2*dy2*(-4.0*ry2 + dy2)));
    double sqy = sq;
    double sqx = (y1-y0)*sq;
    double mby = rxy2*sy*dx2 + rx4*dy2*sy;
    double mbx = rxy2*x012*dy2 + ry4*dx2*x012;

    double cx0, cx1, cy0, cy1;

    if (ax == 0.0)
    {
        // x0 equal to x1 -- vertical chord
        cx0 = x0;
        cx1 = x1;
    }
    else
    {
        cx0 = (mbx + sqx)/ax;
        cx1 = (mbx - sqx)/ax;
    }

    if (ay == 0.0)
    {
        // this means that y0 and y1 were equal so chord is horizontal
        cy0 = y0;
        cy1 = y1;
    }
    else
    {
        cy0 = (mby + sqy)/ay;
        cy1 = (mby - sqy)/ay;
    }

    double sAng = atan2(y0-cy0, x0-cx0);
    if (sAng < 0.0)
        sAng += 2.0*M_PI;
    double eAng = atan2(y1-cy0, x1-cx0);
    if (eAng < 0.0)
        eAng += 2.0*M_PI;

    // TODO: scale radii until properly specified instead of failing
    if (!_finite(cx0) || _isnan(cx0) ||
        !_finite(cx1) || _isnan(cx1) ||
        !_finite(cy0) || _isnan(cy0) ||
        !_finite(cy1) || _isnan(cy1))
        return false;

    double cx = cx0, cy = cy0;
    bool small = SMALL_ANGLE(sAng, eAng);
    if ((small && !def.clockwise && def.largeArc) || (small && def.clockwise && !def.largeArc)) // reject
    {
        cx = cx1, cy = cy1;
        sAng = atan2(y0-cy0, x0-cx1);
        if (sAng < 0.0)
            sAng += 2.0*M_PI;
        eAng = atan2(y1-cy1, x1-cx1);
        if (eAng < 0.0)
            eAng += 2.0*M_PI;
    }

    if (def.clockwise)
    {
        double temp = sAng;
        sAng = eAng;
        eAng = temp;
    }

    if (eAng < sAng)
        eAng += 2.0*M_PI;

    if (def.rotation != 0.0)
    {
        // rerotate the center
        double tcx = cx, tcy = cy;
        double sine = sin(rotrad);
        double cosine = cos(rotrad);
        cx = tcx*cosine + tcy*sine;
        cy = tcy*cosine - tcx*sine;
    }

    data.cx = cx; data.cy = cy;
    data.endAng = eAng; data.startAng = sAng;
    return true;
}


void SE_StyleVisitor::VisitPath(Path& path)
{
    if (m_primitive)
    {
        delete m_primitive;
        m_primitive = NULL;
    }

    SE_Color fillColor;
    ParseColorExpression(path.GetFillColor(), fillColor, 0);

    if (fillColor.empty())
    {
        SE_Polyline* primitive = new SE_Polyline();
        m_primitive = primitive;
        primitive->geometry = m_bp->NewLineBuffer(4);
        ParseGeometry(path.GetGeometry(), *primitive->geometry);
        ParseDoubleExpression(path.GetLineWeight(), primitive->weight, 0.0);
        ParseColorExpression(path.GetLineColor(), primitive->color, 0);
        ParseBooleanExpression(path.GetLineWeightScalable(), primitive->weightScalable, true);
        ParseStringExpression(path.GetLineCap(), primitive->cap, L"Round");
        ParseStringExpression(path.GetLineJoin(), primitive->join, L"Round");
        ParseDoubleExpression(path.GetLineMiterLimit(), primitive->miterLimit, 5.0);

        // if the color is transparent there's no point in drawing this
        // path, so change it to black
        if (primitive->color.value.argb == 0)
            primitive->color.value.comps.a = 255;

        primitive->cacheable = !(primitive->weight.expression
                              || primitive->color.expression
                              || primitive->weightScalable.expression
                              || primitive->cap.expression
                              || primitive->join.expression
                              || primitive->miterLimit.expression);
    }
    else
    {
        SE_Polygon* primitive = new SE_Polygon();
        m_primitive = primitive;
        primitive->fill = fillColor;
        primitive->geometry = m_bp->NewLineBuffer(4);
        ParseGeometry(path.GetGeometry(), *primitive->geometry);
        ParseDoubleExpression(path.GetLineWeight(), primitive->weight, 0.0);
        ParseColorExpression(path.GetLineColor(), primitive->color, 0);
        ParseBooleanExpression(path.GetLineWeightScalable(), primitive->weightScalable, true);
        ParseStringExpression(path.GetLineCap(), primitive->cap, L"Round");
        ParseStringExpression(path.GetLineJoin(), primitive->join, L"Round");
        ParseDoubleExpression(path.GetLineMiterLimit(), primitive->miterLimit, 5.0);

        primitive->cacheable = !(primitive->weight.expression
                              || primitive->color.expression
                              || primitive->fill.expression
                              || primitive->weightScalable.expression
                              || primitive->cap.expression
                              || primitive->join.expression
                              || primitive->miterLimit.expression);
    }
}


void SE_StyleVisitor::VisitImage(Image& image)
{
    if (m_primitive)
    {
        delete m_primitive;
        m_primitive = NULL;
    }

    SE_Raster* primitive = new SE_Raster();
    m_primitive = primitive;

    // remember any parent symbol's res ID so that we can use it later for
    // getting the raster data in case the image name is an expression
    primitive->resId = (m_resIdStack.size() > 0)? m_resIdStack.back() : NULL;

    if (image.GetContent().size())
    {
        const MdfModel::MdfString& src_u = image.GetContent();

        if (m_resources)
            m_resources->GetImageData(src_u.c_str(), (int)src_u.size(), primitive->imageData);
    }
    else
    {
        // We need to pass in the actual strings as default values.  A user might forget
        // to wrap the resource ID and library item name in single quotes, and therefore
        // the string gets treated as an expression.  But in this case the expression won't
        // evaluate, and therefore the default value provides the original intended value.
        // In the case where we have actual expressions, these will evaluate correctly and
        // replace the default values.
        ParseStringExpression(image.GetResourceId(), primitive->pngResourceId, image.GetResourceId().c_str());
        ParseStringExpression(image.GetLibraryItemName(), primitive->pngResourceName, image.GetLibraryItemName().c_str());

        if (primitive->pngResourceId.expression == NULL && primitive->pngResourceName.expression == NULL) // constant path
        {
            // if we have non-empty resource ID then use it, otherwise
            // use the ID of any parent symbol definition
            const wchar_t* resourceId = primitive->pngResourceId.value;
            if (wcslen(resourceId) == 0)
                resourceId = primitive->resId;

            if (m_resources)
                m_resources->GetImageData(resourceId, primitive->pngResourceName.value, primitive->imageData);
        }
        else
        {
            // the image data for the non-constant case gets obtained later
            primitive->imageData.data = NULL;
        }
    }

    ParseDoubleExpression(image.GetPositionX(), primitive->position[0], 0.0);
    ParseDoubleExpression(image.GetPositionY(), primitive->position[1], 0.0);
    ParseDoubleExpression(image.GetSizeX(), primitive->extent[0], 5.0);
    ParseDoubleExpression(image.GetSizeY(), primitive->extent[1], 5.0);
    ParseDoubleExpression(image.GetAngle(), primitive->angleDeg, 0.0);
    ParseBooleanExpression(image.GetSizeScalable(), primitive->extentScalable, true);

    primitive->cacheable = !(primitive->position[0].expression
                          || primitive->position[1].expression
                          || primitive->extent[0].expression
                          || primitive->extent[1].expression
                          || primitive->angleDeg.expression
                          || primitive->extentScalable.expression)
                          && primitive->imageData.data;
}


void SE_StyleVisitor::VisitText(Text& text)
{
    if (m_primitive)
    {
        delete m_primitive;
        m_primitive = NULL;
    }

    SE_Text* primitive = new SE_Text();
    m_primitive = primitive;

    ParseStringExpression(text.GetContent(), primitive->content, L"");
    ParseStringExpression(text.GetFontName(), primitive->fontName, L"Arial");
    ParseDoubleExpression(text.GetHeight(), primitive->height, 4.0);
    ParseDoubleExpression(text.GetAngle(), primitive->angleDeg, 0.0);
    ParseDoubleExpression(text.GetPositionX(), primitive->position[0], 0.0);
    ParseDoubleExpression(text.GetPositionY(), primitive->position[1], 0.0);
    ParseDoubleExpression(text.GetLineSpacing(), primitive->lineSpacing, 1.05);
    ParseBooleanExpression(text.GetHeightScalable(), primitive->heightScalable, true);
    ParseBooleanExpression(text.GetBold(), primitive->bold, false);
    ParseBooleanExpression(text.GetItalic(), primitive->italic, false);
    ParseBooleanExpression(text.GetUnderlined(), primitive->underlined, false);
    ParseStringExpression(text.GetHorizontalAlignment(), primitive->hAlignment, L"Center");
    ParseStringExpression(text.GetVerticalAlignment(), primitive->vAlignment, L"Halfline");
    ParseStringExpression(text.GetJustification(), primitive->justification, L"FromAlignment");
    ParseColorExpression(text.GetTextColor(), primitive->textColor, 0xff000000);
    ParseColorExpression(text.GetGhostColor(), primitive->ghostColor, 0);

    TextFrame* frame = text.GetFrame();
    if (frame)
    {
        ParseColorExpression(frame->GetLineColor(), primitive->frameLineColor, 0);
        ParseColorExpression(frame->GetFillColor(), primitive->frameFillColor, 0);
        ParseDoubleExpression(frame->GetOffsetX(), primitive->frameOffset[0], 0.0);
        ParseDoubleExpression(frame->GetOffsetY(), primitive->frameOffset[1], 0.0);
    }

    primitive->cacheable = !(primitive->content.expression
                          || primitive->fontName.expression
                          || primitive->height.expression
                          || primitive->angleDeg.expression
                          || primitive->position[0].expression
                          || primitive->position[1].expression
                          || primitive->lineSpacing.expression
                          || primitive->heightScalable.expression
                          || primitive->bold.expression
                          || primitive->italic.expression
                          || primitive->underlined.expression
                          || primitive->hAlignment.expression
                          || primitive->vAlignment.expression
                          || primitive->justification.expression
                          || primitive->textColor.expression
                          || primitive->ghostColor.expression
                          || primitive->frameLineColor.expression
                          || primitive->frameFillColor.expression
                          || primitive->frameOffset[0].expression
                          || primitive->frameOffset[1].expression);
}


void SE_StyleVisitor::VisitSimpleSymbolDefinition(MdfModel::SimpleSymbolDefinition& simpleSymbol)
{
    SetDefaultValues(&simpleSymbol);

    // TODO - We need a hint that says what feature geometry type we're
    //        working with, so that we can get the relevant usage.  For
    //        now just keep the first non-NULL usage we find.
    PointUsage* pointUsage = simpleSymbol.GetPointUsage();
    LineUsage* lineUsage = simpleSymbol.GetLineUsage();
    AreaUsage* areaUsage = simpleSymbol.GetAreaUsage();

    m_style = NULL;
    if (pointUsage != NULL)
        m_style = this->ProcessPointUsage(*pointUsage);
    else if (lineUsage != NULL)
        m_style = this->ProcessLineUsage(*lineUsage);
    else if (areaUsage != NULL)
        m_style = this->ProcessAreaUsage(*areaUsage);

    // must have a style in order to render something
    if (m_style == NULL)
        return;

    GraphicElementCollection* graphics = simpleSymbol.GetGraphics();

    int count = graphics->GetCount();
    for (int i = 0; i < count; i++)
    {
        GraphicElement* elem = graphics->GetAt(i);
        elem->AcceptVisitor(*this);

        if (m_primitive)
        {
            ParseStringExpression(elem->GetResizeControl(), m_primitive->resizeControl, L"ResizeNone");
            m_style->symbol.push_back(m_primitive);

            // update the style's cacheable flag to take into account
            // the primitive's flag
            m_style->cacheable &= m_primitive->cacheable;
        }

        m_primitive = NULL;
    }

    ResizeBox* box = simpleSymbol.GetResizeBox();
    m_style->useBox = (box != NULL);
    if (m_style->useBox)
    {
        ParseDoubleExpression(box->GetSizeX(), m_style->resizeSize[0], 1.0);
        ParseDoubleExpression(box->GetSizeY(), m_style->resizeSize[1], 1.0);
        ParseDoubleExpression(box->GetPositionX(), m_style->resizePosition[0], 0.0);
        ParseDoubleExpression(box->GetPositionY(), m_style->resizePosition[1], 0.0);
        ParseStringExpression(box->GetGrowControl(), m_style->growControl, L"GrowInXYMaintainAspect");

        m_style->cacheable &= !(m_style->resizeSize[0].expression
                             || m_style->resizeSize[1].expression
                             || m_style->resizePosition[0].expression
                             || m_style->resizePosition[1].expression);
    }

    m_symbolization->styles.push_back(m_style);
}


void SE_StyleVisitor::VisitCompoundSymbolDefinition(MdfModel::CompoundSymbolDefinition& compoundSymbol)
{
    SimpleSymbolCollection* symbols = compoundSymbol.GetSymbols();
    int len = symbols->GetCount();
    for (int i = 0; i < len; i++)
    {
        SimpleSymbol* sym = symbols->GetAt(i);

        bool isRef = false;

        // get the symbol definition, either inlined or by reference
        SimpleSymbolDefinition* def = sym->GetSymbolDefinition();
        if (def == NULL)
        {
            if (m_resources == NULL)
                return;

            const MdfString& ref = sym->GetResourceId(); // symbol reference
            def = dynamic_cast<SimpleSymbolDefinition*>(m_resources->GetSymbolDefinition(ref.c_str()));
            if (def == NULL)
                return;

            // remember the current symbol resource id in case it references
            // an attached png image resource
            isRef = true;
            m_resIdStack.push_back(ref.c_str());
        }

        VisitSimpleSymbolDefinition(*def);

        if (m_style)
            ParseIntegerExpression(sym->GetRenderingPass(), m_style->renderPass, 0);

        if (isRef)
            m_resIdStack.pop_back();
    }
}


void SE_StyleVisitor::Convert(std::vector<SE_Symbolization*>& result, MdfModel::CompositeSymbolization* symbolization)
{
    if (symbolization == NULL)
        return;

    m_resIdStack.clear();

    SymbolInstanceCollection* symbols = symbolization->GetSymbolCollection();
    int nSymbols = symbols->GetCount();

    for (int i = 0; i < nSymbols; i++)
    {
        SymbolInstance* instance = symbols->GetAt(i);

        SetParameterValues(instance->GetParameterOverrides());

        bool isRef = false;

        // get the symbol definition, either inlined or by reference
        SymbolDefinition* def = instance->GetSymbolDefinition();
        if (def == NULL)
        {
            if (m_resources == NULL)
                continue;

            const MdfString& ref = instance->GetResourceId(); // symbol reference
            def = m_resources->GetSymbolDefinition(ref.c_str());
            if (def == NULL)
                continue;

            // remember the current symbol resource id in case it references
            // an attached png image resource
            isRef = true;
            m_resIdStack.push_back(ref.c_str());
        }

        m_symbolization = new SE_Symbolization();

        m_symbolization->context = instance->GetSizeContext();

        ParseStringExpression(instance->GetPositioningAlgorithm(), m_symbolization->positioningAlgorithm, L"");

        ParseBooleanExpression(instance->GetDrawLast(), m_symbolization->drawLast, false);
        ParseBooleanExpression(instance->GetAddToExclusionRegion(), m_symbolization->addToExclusionRegions, false);
        ParseBooleanExpression(instance->GetCheckExclusionRegion(), m_symbolization->checkExclusionRegions, false);

        ParseDoubleExpression(instance->GetScaleX(), m_symbolization->scale[0], 1.0);
        ParseDoubleExpression(instance->GetScaleY(), m_symbolization->scale[1], 1.0);
        ParseDoubleExpression(instance->GetInsertionOffsetX(), m_symbolization->absOffset[0], 0.0);
        ParseDoubleExpression(instance->GetInsertionOffsetY(), m_symbolization->absOffset[1], 0.0);

        def->AcceptVisitor(*this);

        result.push_back(m_symbolization);

        if (isRef)
            m_resIdStack.pop_back();
    }
}
