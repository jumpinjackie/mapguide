//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef RS_FONTENGINE_H_
#define RS_FONTENGINE_H_

#include "Stylization.h"
#include "SE_RenderProxies.h"
#include "RS_Font.h"
#include "RS_TextMetrics.h"

class SE_Renderer;

// the maximum number of path segments allowed when labeling a path
const int MAX_PATH_SEGMENTS = 16384;

// the distance, in inches, between repeating path labels
const double PATH_LABEL_SEPARATION_INCHES = 2.5;



//////////////////////////////////////////////////////////////////////////////
class RS_FontEngine
{
public:
    STYLIZATION_API RS_FontEngine();
    STYLIZATION_API virtual ~RS_FontEngine();

    STYLIZATION_API virtual void InitFontEngine(SE_Renderer* pSERenderer);

    STYLIZATION_API virtual void MeasureString(const RS_String& s,
                                               double           height,
                                               const RS_Font*   font,
                                               double           angleRad,
                                               RS_F_Point*      res,
                                               float*           offsets) = 0;

    STYLIZATION_API virtual void DrawString(const RS_String& s,
                                            double           x,
                                            double           y,
                                            double           width,
                                            double           height,
                                            const RS_Font*   font,
                                            RS_Color&        color,
                                            double           angleRad) = 0;

    STYLIZATION_API virtual const RS_Font* FindFont(RS_FontDef& def) = 0;

    STYLIZATION_API virtual bool ScreenVectorPointsUp(double x, double y);

    STYLIZATION_API bool GetTextMetrics(const RS_String& s, RS_TextDef& tdef, RS_TextMetrics& ret, bool bPathText);

    STYLIZATION_API bool LayoutPathText(RS_TextMetrics& tm, const RS_F_Point* pts, int npts, double* segpos,
                                        double param_position, RS_VAlignment valign, double scaleLimit);

    STYLIZATION_API void DrawPathText(RS_TextMetrics& tm, RS_TextDef& tdef);

    STYLIZATION_API void DrawBlockText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy);

    STYLIZATION_API double MetersToScreenUnits(RS_Units unit, double number);

    STYLIZATION_API virtual const RS_Font* GetRenderingFont(RS_TextDef& tdef);

public:
    size_t SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks);

private:
    double GetVerticalAlignmentOffset(RS_VAlignment vAlign, const RS_Font* font,
                                      double actual_height, double line_height,
                                      size_t numLines);

    double GetJustificationOffset(RS_Justify justify, double textWidth, RS_F_Point* ext);

    double GetHorizontalAlignmentOffset(RS_HAlignment hAlign, double lineWidth);

public:
    SE_Renderer* m_pSERenderer;
    SE_LineStroke m_frameStroke;
    SE_LineStroke m_lineStroke;
};

#endif
