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
    m_usageContext = SymbolInstance::ucUnspecified;
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
    ParseDoubleExpression(lineUsage.GetStartOffset(), style->startOffset, -1.0);
    ParseDoubleExpression(lineUsage.GetEndOffset(), style->endOffset, -1.0);
    ParseDoubleExpression(lineUsage.GetRepeat(), style->repeat, 0.0);
    ParseDoubleExpression(lineUsage.GetVertexAngleLimit(), style->vertexAngleLimit, 0.0);
    ParseStringExpression(lineUsage.GetVertexJoin(), style->vertexJoin, L"Round");
    ParseDoubleExpression(lineUsage.GetVertexMiterLimit(), style->vertexMiterLimit, 5.0);

    Path* defaultPath = lineUsage.GetDefaultPath();
    if (defaultPath != NULL)
    {
        ParseDoubleExpression(defaultPath->GetLineWeight(), style->dpWeight, 0.0);
        ParseColorExpression(defaultPath->GetLineColor(), style->dpColor, 0);
        ParseBooleanExpression(defaultPath->GetLineWeightScalable(), style->dpWeightScalable, true);
        ParseStringExpression(defaultPath->GetLineCap(), style->dpCap, L"Round");
        ParseStringExpression(defaultPath->GetLineJoin(), style->dpJoin, L"Round");
        ParseDoubleExpression(defaultPath->GetLineMiterLimit(), style->dpMiterLimit, 5.0);

        // if the color is transparent there's no point in drawing this
        // path, so change it to black
        if (style->dpColor.value.argb == 0)
            style->dpColor.value.comps.a = 255;
    }

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
                      || style->vertexMiterLimit.expression
                      || style->dpWeight.expression
                      || style->dpColor.expression
                      || style->dpWeightScalable.expression
                      || style->dpCap.expression
                      || style->dpJoin.expression
                      || style->dpMiterLimit.expression);

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
            arcdef.clockwise = (cw != 0.0);
            arcdef.largeArc = (large != 0.0);
            arcdef.rotation = rot * M_PI180;
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
    // don't allow degenerate arcs
    if (def.rx == 0.0 || def.ry == 0.0)
        return false;

    // step 1: compute a center of rotation which minimizes R/O error
    double ctrX = 0.5*(def.x0 + def.x1);
    double ctrY = 0.5*(def.y0 + def.y1);

    // step 2: derotate arc points by the arc angle about this point
    //         => the arc is now oriented with the coordinates axes
    double x0, y0, x1, y1;
    if (def.rotation != 0.0)
    {
        double cs = cos(-def.rotation);
        double sn = sin(-def.rotation);
        x0 = (def.x0 - ctrX)*cs - (def.y0 - ctrY)*sn;
        y0 = (def.x0 - ctrX)*sn + (def.y0 - ctrY)*cs;
        x1 = (def.x1 - ctrX)*cs - (def.y1 - ctrY)*sn;
        y1 = (def.x1 - ctrX)*sn + (def.y1 - ctrY)*cs;
    }
    else
    {
        x0 = def.x0 - ctrX;
        y0 = def.y0 - ctrY;
        x1 = def.x1 - ctrX;
        y1 = def.y1 - ctrY;
    }

    // step 3: scale the rotated points by rx/ry in the vertical direction
    //         => the elliptical arc is now transformed into a circle of
    //            radius rx
    double vscale = def.rx / def.ry;
    y0 *= vscale;
    y1 *= vscale;

    // step 4: compute the circle centers
    //
    // We have two points (pt0 and pt1) on the circle and its radius (rx).  To find the
    // two possible centers draw a circle of radius rx about each point, and then compute
    // the intersections of those two circles:
    //
    //           * center A
    //          /|\
    //         / | \
    //      rx/  |  \rx
    //       /   |   \
    //      /    |    \
    // pt0 *-----------* pt1
    //      \    |    /
    //       \   |   /
    //      rx\  |  /rx
    //         \ | /
    //          \|/
    //           * center B
    //
    // The diagram shows the case where pt0 and pt1 are aligned horizontally, but in
    // general they can be anywhere.  In that case just rotate the entire diagram.
    //
    // Inspection of the diagram shows that the centers are easily computed.  We have
    // the vector T going from pt0 to pt1, and we can compute its normal N.  To compute
    // the centers go halfway along T, and then go either up or down along N, by the
    // distance obtained from the right triangle.

    // vector T and its length squared
    double tx  = x1 - x0;
    double ty  = y1 - y0;
    double td2 = tx*tx + ty*ty;
    if (td2 == 0.0)
        return false;   // degenerate

    // vector N and its length
    double nd2 = def.rx * def.rx - 0.25*td2;
    if (nd2 < 0.0)
        return false;   // degenerate

    double td = sqrt(td2);
    double nd = sqrt(nd2);
    double nx = -ty * nd / td;
    double ny =  tx * nd / td;

    // compute the centers - these are still scaled vertically
    double midx = 0.5*(x0 + x1);
    double midy = 0.5*(y0 + y1);

    double cx0 = midx + nx;
    double cy0 = midy + ny;

    double cx1 = midx - nx;
    double cy1 = midy - ny;

    // try first center
    double cx = cx0, cy = cy0;
    double sAng = atan2(y0-cy0, x0-cx0);
    double eAng = atan2(y1-cy0, x1-cx0);

    // need end angle > start angle for CCW rotation, and reverse for CW rotation
    if (!def.clockwise)
    {
        if (eAng < sAng)
            eAng += 2.0*M_PI;
    }
    else
    {
        if (sAng < eAng)
            sAng += 2.0*M_PI;
    }

    // determine if the separation is large
    bool large = fabs(eAng - sAng) > M_PI;

    // if we get a large arc but we wanted a small one (or vice-versa) then use the second center
    if (def.largeArc != large)
    {
        // try second center
        cx = cx1, cy = cy1;
        sAng = atan2(y0-cy1, x0-cx1);
        eAng = atan2(y1-cy1, x1-cx1);

        // need end angle > start angle for CCW rotation, and reverse for CW rotation
        if (!def.clockwise)
        {
            if (eAng < sAng)
                eAng += 2.0*M_PI;
        }
        else
        {
            if (sAng < eAng)
                sAng += 2.0*M_PI;
        }
    }

    // step 5: undo vertical scaling for center
    cy /= vscale;

    // rerotate and recenter the center
    if (def.rotation != 0.0)
    {
        double cs = cos(def.rotation);
        double sn = sin(def.rotation);
        data.cx = ctrX + cx*cs - cy*sn;
        data.cy = ctrY + cx*sn + cy*cs;
    }
    else
    {
        data.cx = ctrX + cx;
        data.cy = ctrY + cy;
    }

    data.startAng = sAng;
    data.endAng = eAng;

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
        primitive->geometry = m_bp->NewSELineBuffer(4);
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
        primitive->geometry = m_bp->NewSELineBuffer(4);
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

    // get the usage to work with
    PointUsage* pointUsage = simpleSymbol.GetPointUsage();
    LineUsage* lineUsage = simpleSymbol.GetLineUsage();
    AreaUsage* areaUsage = simpleSymbol.GetAreaUsage();

    m_style = NULL;
    switch (m_usageContext)
    {
        case SymbolInstance::ucPoint:
            if (pointUsage != NULL)
                m_style = ProcessPointUsage(*pointUsage);
            break;

        case SymbolInstance::ucLine:
            if (lineUsage != NULL)
                m_style = ProcessLineUsage(*lineUsage);
            break;

        case SymbolInstance::ucArea:
            if (areaUsage != NULL)
                m_style = ProcessAreaUsage(*areaUsage);
            break;

        case SymbolInstance::ucUnspecified:
            // default behavior is first one found, in order of point, line, area
            if (pointUsage != NULL)
                m_style = this->ProcessPointUsage(*pointUsage);
            else if (lineUsage != NULL)
                m_style = this->ProcessLineUsage(*lineUsage);
            else if (areaUsage != NULL)
                m_style = this->ProcessAreaUsage(*areaUsage);
    }

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

        m_symbolization->sizeContext = instance->GetSizeContext();
        m_symbolization->geomContext = instance->GetGeometryContext();

        m_usageContext = instance->GetUsageContext();

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
