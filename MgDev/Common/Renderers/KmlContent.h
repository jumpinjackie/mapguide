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

#ifndef KMLCONTENT_H
#define KMLCONTENT_H

#include "Renderers.h"
#include "RendererStyles.h"
#include "KmlLineStyle.h"
#include "KmlPolyStyle.h"
#include "KmlIconStyle.h"
#include <sstream>

class KmlContent
{
public:
    RENDERERS_API KmlContent();
    RENDERERS_API ~KmlContent();

    RENDERERS_API void StartDocument();
    RENDERERS_API void EndDocument();

    RENDERERS_API void WriteString(const char* szString, bool lineBreak = true);
    RENDERERS_API void WriteString(const wchar_t* wszString, bool lineBreak = true);
    RENDERERS_API void WriteString(const std::string& mbString, bool lineBreak = true);
    RENDERERS_API void WriteString(const std::wstring& wString, bool lineBreak = true);

    RENDERERS_API std::string GetString();

private:
    void WriteString(const char* szString, std::streamsize length, bool lineBreak);
    std::stringbuf m_content;
};

#endif //KMLCONTENT_H
