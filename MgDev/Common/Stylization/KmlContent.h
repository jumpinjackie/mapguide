//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "Stylization.h"
#include "RendererStyles.h"
#include "KmlLineStyle.h"
#include "KmlPolyStyle.h"
#include <sstream>

class KmlContent
{
public:
    STYLIZATION_API KmlContent();
    STYLIZATION_API ~KmlContent();

    STYLIZATION_API void StartDocument();
    STYLIZATION_API void EndDocument();

    STYLIZATION_API void WriteString(const char* mbString);
    STYLIZATION_API void WriteString(const wchar_t* wString);
    STYLIZATION_API void WriteString(const std::string& mbString);
    STYLIZATION_API void WriteString(const std::wstring& wString);
    
    STYLIZATION_API std::string GetString();
private:

    std::stringbuf m_content;
};

#endif //KMLCONTENT_H