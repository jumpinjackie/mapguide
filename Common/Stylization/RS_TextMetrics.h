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

#ifndef RS_TEXTMETRICS_H_
#define RS_TEXTMETRICS_H_

#include "StylizationAPI.h"
#include "RendererStyles.h"

namespace RichText { namespace ATOM { class Particle; } }
struct RS_Font;

//////////////////////////////////////////////////////////////////////////////
struct CharPos
{
    double x;
    double y;
    double anglerad;
};


//////////////////////////////////////////////////////////////////////////////
struct LinePos
{
    RS_F_Point ext[4];
    double hOffset;
    double vOffset;
};


//////////////////////////////////////////////////////////////////////////////
class RS_TextMetrics
{
public:
    STYLIZATION_API RS_TextMetrics();
    STYLIZATION_API ~RS_TextMetrics();

    // note that this value is NULL if RS_TextMetrics is uninitialized or invalid
    const RS_Font* font;

    double font_height;
    double text_width;
    double text_height;
    RS_String text;

    // for path text - character advances and positions
    std::vector<float> char_advances;
    std::vector<CharPos> char_pos;

    // for block text - line positions
    std::vector<LinePos> line_pos;
    std::vector<RS_String> line_breaks;

    // for formatted text - format changes
    std::vector<const RichText::ATOM::Particle*> format_changes;
};

#endif
