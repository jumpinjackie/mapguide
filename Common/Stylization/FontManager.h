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

#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

//  FreeType includes
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_CACHE_MANAGER_H

#include <map>
#include <string>

#ifdef WIN32
#include <tchar.h>
#include "shlobj.h"
#endif  //  WIN32

#include "UnicodeString.h"

#include "Stylization.h"
#include "RS_Font.h"

typedef struct FaceMapEntry {
    char *  pData;
    size_t  length;
} FaceMapEntryType;

typedef map<string, FaceMapEntryType*> FaceMap;
typedef FaceMap::value_type FaceMapPair;
typedef FaceMap::iterator FaceMapIterator;

class FontManager
{
    private:
        FontManager ();
        ~FontManager (void);

    public:
        int get_face (const char * filename, FT_Long index, FT_Face * face);

        void init_font_list();
        void create_font (FT_Face face, FT_Long index, wchar_t const * filename);

        FontList*   GetFontList();

        STYLIZATION_API const wchar_t* FindFont( const wchar_t* fontname, bool bold, bool italic, bool underline );

        //  singleton access method
        STYLIZATION_API static FontManager* Instance();

    private:
        FaceMapEntryType * load_file (const char * filename);

    private:
        FT_Library  m_library;
        FaceMap     m_facemap;
        FontList    m_fontlist;

        static FontManager    m_manager;
};

#endif  //  FONTMANAGER_H_
