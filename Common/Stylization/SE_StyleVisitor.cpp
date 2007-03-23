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

SE_StyleVisitor::SE_StyleVisitor(SE_SymbolManager* resources, SE_LineBufferPool* lbp)
{
    m_resources = resources;
    m_lbp = lbp;
    m_primitive = NULL;
    m_symbolization = NULL;
    m_style = NULL;
}

SE_PointStyle* SE_StyleVisitor::ProcessPointUsage(PointUsage& pointUsage)
{
    SE_PointStyle* style = new SE_PointStyle();
    ParseDoubleExpression(pointUsage.GetAngle(), style->angle);
    ParseDoubleExpression(pointUsage.GetOriginOffsetX(), style->originOffset[0]);
    ParseDoubleExpression(pointUsage.GetOriginOffsetY(), style->originOffset[1]);
    ParseStringExpression(pointUsage.GetAngleControl(), style->angleControl);

    //set flag if all properties are constant
    style->cacheable = ! (style->angle.expression
                        || style->angleControl.expression
                        || style->originOffset[0].expression
                        || style->originOffset[1].expression);

    return style;
}

SE_LineStyle* SE_StyleVisitor::ProcessLineUsage(LineUsage& lineUsage)
{
    SE_LineStyle* style = new SE_LineStyle();
    ParseStringExpression(lineUsage.GetAngleControl(), style->angleControl);
    ParseStringExpression(lineUsage.GetUnitsControl(), style->unitsControl);
    ParseStringExpression(lineUsage.GetVertexControl(), style->vertexControl);
    ParseDoubleExpression(lineUsage.GetAngle(), style->angle);
    ParseDoubleExpression(lineUsage.GetStartOffset(), style->startOffset);
    ParseDoubleExpression(lineUsage.GetEndOffset(), style->endOffset);
    ParseDoubleExpression(lineUsage.GetRepeat(), style->repeat);
    ParseDoubleExpression(lineUsage.GetVertexAngleLimit(), style->vertexAngleLimit);
    ParseStringExpression(lineUsage.GetVertexJoin(), style->vertexJoin);

    //set flag if all properties are constant
    style->cacheable = ! (  style->angle.expression
        || style->angleControl.expression
        || style->unitsControl.expression
        || style->vertexControl.expression
        || style->angle.expression
        || style->startOffset.expression
        || style->endOffset.expression
        || style->repeat.expression
        || style->vertexAngleLimit.expression);

    return style;
}

SE_AreaStyle* SE_StyleVisitor::ProcessAreaUsage(AreaUsage& areaUsage)
{
    SE_AreaStyle* style = new SE_AreaStyle();
    ParseStringExpression(areaUsage.GetAngleControl(), style->angleControl);
    ParseStringExpression(areaUsage.GetOriginControl(), style->originControl);
    ParseStringExpression(areaUsage.GetClippingControl(), style->clippingControl);
    ParseDoubleExpression(areaUsage.GetAngle(), style->angle);
    ParseDoubleExpression(areaUsage.GetOriginX(), style->origin[0]);
    ParseDoubleExpression(areaUsage.GetOriginY(), style->origin[1]);
    ParseDoubleExpression(areaUsage.GetRepeatX(), style->repeat[0]);
    ParseDoubleExpression(areaUsage.GetRepeatY(), style->repeat[1]);
    ParseDoubleExpression(areaUsage.GetBufferWidth(), style->bufferWidth);

    //set flag if all properties are constant
    style->cacheable = !(  style->angle.expression
                        || style->angleControl.expression
                        || style->originControl.expression
                        || style->clippingControl.expression
                        || style->angle.expression
                        || style->origin[0].expression
                        || style->origin[1].expression
                        || style->repeat[0].expression
                        || style->repeat[1].expression
                        || style->bufferWidth.expression);

    return style;
}

bool SE_StyleVisitor::ParseDouble(const wchar_t*& str, double& val)
{
    size_t length;
    swscanf(str, L" %lf %n", &val, &length);
    str += length;
    return length > 0;
}

bool SE_StyleVisitor::ParseDoublePair(const wchar_t*& str, double& x, double& y)
{
    size_t length;
    swscanf(str, L" %lf , %lf %n", &x, &y, &length);
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
        switch(tag)
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
            lb->LineTo(lx,ly);
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
    double sq = sqrt(rxy2*dx2*(-ry4*dx2*dx2 + 2*rxy2*dx2*(2*ry2 - dy2) - rx2*rx2*dy2*(-4*ry2 + dy2)));
    double sqy = sq;
    double sqx = (y1-y0)*sq;
    double mby = rxy2*sy*dx2 + rx4*dy2*sy;
    double mbx = rxy2*x012*dy2 + ry4*dx2*x012;

    double cx0, cx1, cy0, cy1;

    if (ax == 0.0)
    {
        //x0 equal to x1 -- vertical chord
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
        //this means that y0 and y1 were equal so chord is horizontal
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

    /* TODO: scale radii until properly specified instead of failing */
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
        /* Rerotate the center */
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
    SE_Color color;
    const MdfString& geometry = path.GetGeometry();

    ParseColorExpression(path.GetFillColor(), color);

    if (m_primitive)
        delete m_primitive;

    if (color.empty())
    {
        SE_Polyline* line = new SE_Polyline();
        m_primitive = line;
        line->geometry = m_lbp->NewLineBuffer(4);
        ParseGeometry(geometry, *line->geometry);
        ParseDoubleExpression(path.GetLineWeight(), line->weight);
        ParseColorExpression(path.GetLineColor(), line->color);
        ParseBooleanExpression(path.GetLineWeightScalable(), line->weightScalable);

        /* If the color is transparent, there is no point in drawing this path,
         * so we will change it to black. */
        if (line->color.value.argb == 0)
            line->color.value.comps.a = 255;
        line->cacheable = !(line->weight.expression ||
                            line->color.expression ||
                            line->weightScalable.expression);
    }
    else
    {
        SE_Polygon* polygon = new SE_Polygon();
        m_primitive = polygon;
        polygon->geometry = m_lbp->NewLineBuffer(4);
        ParseGeometry(geometry, *polygon->geometry);
        ParseDoubleExpression(path.GetLineWeight(), polygon->weight);
        polygon->fill = color;
        ParseColorExpression(path.GetLineColor(), polygon->color);
        ParseBooleanExpression(path.GetLineWeightScalable(), polygon->weightScalable);
        polygon->cacheable = !(polygon->weight.expression ||
                            polygon->color.expression ||
                            polygon->fill.expression ||
                            polygon->weightScalable.expression);
    }
}

void SE_StyleVisitor::VisitImage(Image& image)
{
    SE_Raster* primitive = new SE_Raster();
    m_primitive = primitive;

    //remember the parent symbol's res ID so that we can use it later for getting the raster data
    //in case the image name is an expression
    primitive->resId = m_resIdStack.back();

    if (image.GetContent().size())
    {
        const MdfModel::MdfString& src_u = image.GetContent();
        
        size_t srclen = src_u.size();
        char* src_ascii = new char[srclen];

        char* ptr = src_ascii;
        for (size_t i=0; i<srclen; i++)
            *ptr++ = (char)src_u[i];
                
        size_t dstlen = Base64::GetDecodedLength(image.GetContent().size());
        primitive->pngPtr = new unsigned char[dstlen];
        primitive->ownPtr = true;

        Base64::Decode((unsigned char*)primitive->pngPtr, src_ascii, srclen);
                
        delete [] src_ascii;
    }
    else
    {
        ParseStringExpression(image.GetReference(), primitive->pngPath);

        if (primitive->pngPath.expression == NULL) // constant path
            primitive->pngPtr = (unsigned char*)(m_resources? m_resources->GetImageData(primitive->resId, primitive->pngPath.value, primitive->pngSize) : NULL);
        else
            primitive->pngPtr = NULL;
    }

    ParseDoubleExpression(image.GetPositionX(), primitive->position[0]);
    ParseDoubleExpression(image.GetPositionY(), primitive->position[1]);
    ParseDoubleExpression(image.GetSizeX(), primitive->extent[0]);
    ParseDoubleExpression(image.GetSizeY(), primitive->extent[1]);
    ParseDoubleExpression(image.GetAngle(), primitive->angle);

    //TODO: SizeScaleable
    //ParseBooleanExpression(image.GetExtentScaleable(), primitive->extentScaleable);

    primitive->cacheable = !(primitive->position[0].expression ||
                             primitive->position[1].expression ||
                             primitive->extent[0].expression ||
                             primitive->extent[1].expression ||
                             primitive->angle.expression ||
                             primitive->extentScaleable.expression
                             ) && primitive->pngPtr;
}

void SE_StyleVisitor::VisitText(Text& text)
{
    SE_Text* primitive = new SE_Text();
    m_primitive = primitive;

    ParseStringExpression(text.GetString(), primitive->textExpr);
    ParseStringExpression(text.GetFontName(), primitive->fontExpr);
    ParseDoubleExpression(text.GetHeight(), primitive->size);
    ParseDoubleExpression(text.GetAngle(), primitive->angle);
    ParseDoubleExpression(text.GetPositionX(), primitive->position[0]);
    ParseDoubleExpression(text.GetPositionY(), primitive->position[1]);
    ParseDoubleExpression(text.GetLineSpacing(), primitive->lineSpacing);
    ParseBooleanExpression(text.GetUnderlined(), primitive->underlined);
    ParseBooleanExpression(text.GetBold(), primitive->bold);
    ParseBooleanExpression(text.GetItalic(), primitive->italic);
    ParseColorExpression(text.GetTextColor(), primitive->textColor);
    ParseColorExpression(text.GetGhostColor(), primitive->ghostColor);
    ParseStringExpression(text.GetHorizontalAlignment(), primitive->hAlignment);
    ParseStringExpression(text.GetVerticalAlignment(), primitive->vAlignment);
    ParseStringExpression(text.GetJustification(), primitive->justification);
    
    //TODO: SizeScaleable
    //ParseBooleanExpression(text.GetSizeScaleable(), primitive->sizeScaleable);

    primitive->cacheable = !(primitive->textExpr.expression ||
                             primitive->fontExpr.expression ||
                             primitive->size.expression ||
                             primitive->angle.expression ||
                             primitive->position[0].expression ||
                             primitive->position[1].expression ||
                             primitive->lineSpacing.expression ||
                             primitive->underlined.expression ||
                             primitive->bold.expression ||
                             primitive->italic.expression ||
                             primitive->textColor.expression ||
                             primitive->ghostColor.expression ||
                             primitive->hAlignment.expression ||
                             primitive->vAlignment.expression ||
                             primitive->justification.expression ||
                             primitive->sizeScaleable.expression
                             );
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
            ParseStringExpression(elem->GetResizeControl(), m_primitive->resizeControl);
            m_style->symbol.push_back(m_primitive);

            //also update the style's cacheable flag to take into account the primitive's flag
            m_style->cacheable = m_style->cacheable && m_primitive->cacheable;
        }

        m_primitive = NULL;
    }

    ResizeBox* box = simpleSymbol.GetResizeBox();
    m_style->useBox = box != NULL;
    if (m_style->useBox)
    {
        ParseDoubleExpression(box->GetSizeX(), m_style->resizeSize[0]);
        ParseDoubleExpression(box->GetSizeY(), m_style->resizeSize[1]);
        ParseDoubleExpression(box->GetPositionX(), m_style->resizePosition[0]);
        ParseDoubleExpression(box->GetPositionY(), m_style->resizePosition[1]);
        ParseStringExpression(box->GetGrowControl(), m_style->growControl);

        m_style->cacheable = m_style->cacheable &&
                             ! (  m_style->resizeSize[0].expression
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

            const MdfString& ref = sym->GetSymbolReference();
            def = dynamic_cast<SimpleSymbolDefinition*>(m_resources->GetSymbolDefinition(ref.c_str()));
            if (def == NULL)
                return;

            //remember the current symbol resource id, in case it references an 
            //attached png image resource
            isRef = true;
            m_resIdStack.push_back(ref.c_str());
        }

        VisitSimpleSymbolDefinition(*def);

        if (m_style)
            ParseIntegerExpression(sym->GetRenderingPass(), m_style->renderPass);

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

            const MdfString& ref = instance->GetSymbolReference();
            def = m_resources->GetSymbolDefinition(ref.c_str());
            if (def == NULL)
                continue;

            //remember the current symbol resource id, in case it references an 
            //attached png image resource
            isRef = true;
            m_resIdStack.push_back(ref.c_str());
        }

        m_symbolization = new SE_Symbolization();

        m_symbolization->context = instance->GetSizeContext();

        ParseStringExpression(instance->GetPositioningAlgorithm(), m_symbolization->positioningAlgorithm);

        ParseBooleanExpression(instance->GetDrawLast(), m_symbolization->drawLast);
        ParseBooleanExpression(instance->GetAddToExclusionRegion(), m_symbolization->addToExclusionRegions);
        ParseBooleanExpression(instance->GetCheckExclusionRegion(), m_symbolization->checkExclusionRegions);

        ParseDoubleExpression(instance->GetScaleX(), m_symbolization->scale[0]);
        ParseDoubleExpression(instance->GetScaleY(), m_symbolization->scale[1]);
        ParseDoubleExpression(instance->GetInsertionOffsetX(), m_symbolization->absOffset[0]);
        ParseDoubleExpression(instance->GetInsertionOffsetY(), m_symbolization->absOffset[1]);

        def->AcceptVisitor(*this);
        
        result.push_back(m_symbolization);

        if (isRef)
            m_resIdStack.pop_back();
    }
}
