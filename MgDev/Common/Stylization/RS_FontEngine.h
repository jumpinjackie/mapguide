//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef RS_FONTENGINE_H
#define RS_FONTENGINE_H

struct RS_Font;
class Renderer;
class SE_Renderer;
struct SE_Matrix;


struct CharPos
{
    double x;
    double y;
    double anglerad;
};


struct LinePos
{
    RS_F_Point ext[4];
    double hOffset;
    double vOffset;
};


class RS_TextMetrics
{
public:
    RS_TextMetrics()
        : font(NULL),
          font_height(0),
          text_width(0),
          text_height(0)
    {
    }

    const RS_Font* font;
    double font_height;
    double text_width;
    double text_height;
    RS_String text;

    //for path text -- character advances and positions
    std::vector<float> char_advances;
    std::vector<CharPos> char_pos;

    //for block text -- line positions
    std::vector<LinePos> line_pos;
    std::vector<RS_String> line_breaks;
};


class RS_FontEngine
{
public:
    RS_FontEngine();
    virtual ~RS_FontEngine();

    virtual void InitFontEngine(Renderer* renderer, SE_Renderer* serenderer);

    virtual void DrawString(const RS_String& s,
                            int              x,
                            int              y,
                            double           height,
                            const RS_Font*   font,
                            const RS_Color&  color,
                            double           anglerad) = 0;

    virtual void MeasureString(const RS_String& s,
                               double           height,
                               const RS_Font*   font,
                               double           anglerad,
                               RS_F_Point*      res,
                               float*           offsets) = 0;

    const RS_Font* FindFont(RS_FontDef& def);

    size_t SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks);

    bool GetTextMetrics(const RS_String& s, RS_TextDef& tdef, RS_TextMetrics& ret, bool bPathText);

    bool LayoutPathText(RS_TextMetrics& tm, const RS_F_Point* pts, int npts, double* seglens,
                        double param_position, RS_VAlignment valign, int layout_option);

    void DrawPathText(RS_TextMetrics& tm, RS_TextDef& tdef);

    void DrawBlockText(RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy);

    double GetVerticalAlignmentOffset(RS_VAlignment vAlign, const RS_Font* font,
                                      double actual_height, double line_height,
                                      size_t numLines);

    double GetHorizontalAlignmentOffset(RS_HAlignment hAlign, RS_F_Point* extent);

    //----------------------------------------------------------------
    //TODO: Those really belong as utility functions on the renderer itself
    //      -- they are unrelated to fonts
    //----------------------------------------------------------------
    double MeterToMapSize(RS_Units unit, double number);
    double MetersToPixels(RS_Units unit, double number);

    bool _Yup()
    {
        return m_bYup;
    }

public:
    Renderer* m_renderer;
    SE_Renderer* m_serenderer;
    bool m_bYup;
};

#endif  // RS_FONTENGINE_H
