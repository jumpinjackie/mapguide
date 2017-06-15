//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef _UTFGRIDCONTENT_H_
#define _UTFGRIDCONTENT_H_

#include "Renderers.h"
#include "RendererStyles.h"
#include "RS_FeatureReader.h"
#include "KeyEncode.h"
#include <set>
#include <sstream>

typedef std::map<unsigned int, unsigned int> ColorReMap;
typedef std::map<unsigned int, std::string> ColorKeyMap;
typedef std::map<std::string, std::wstring> KeyFeatureMap;
typedef std::set<unsigned int> ColorSet;

class UTFGridContent
{
public:
    RENDERERS_API UTFGridContent();
    RENDERERS_API ~UTFGridContent();

    RENDERERS_API std::string GetString();

    bool AddFeature(RS_LayerUIInfo* layer, unsigned int color, RS_FeatureReader* feature, const RS_String * tooltip, const RS_String * url);

    void StartGrid();
    void StartGridRow(bool bFirst);
    void BeginTracking();
    void TrackPixel(unsigned int pixel);
    void AppendRowPixel(unsigned int pixel);
    void EndGridRow();
    void EndGrid();

    void WriteKeys();
    void WriteData();

    static unsigned int EncodeChar(unsigned int toEncode);
    static unsigned int DecodeChar(unsigned int toDecode);

private:
    static void EscapeJsonString(const std::wstring& input, std::wstring& output);
    unsigned int m_currentRemappedColor;
    std::wstringstream m_content;
    ColorSet m_trackedEncodedColors;
    ColorReMap m_remappedEncodedColors;
    ColorReMap m_remappedEncodedColorsInv;

    KeyFeatureMap m_features;
    ColorKeyMap m_pixels;
    KeyEncode m_keyEncode;
};

#endif