//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "Renderers.h"
#include "RS_Font.h"    // for FontList
#include "CriticalSection.h"


#include <map>

typedef struct FaceMapEntry {
    char*  pData;
    size_t length;
} FaceMapEntryType;

struct NameCacheEntry
{
public:

    NameCacheEntry()
    {
        memset(stylearray, 0, sizeof(stylearray));
    }

    RS_Font* stylearray[4];
};

typedef map<string, FaceMapEntryType*> FaceMap;
typedef FaceMap::value_type FaceMapPair;
typedef FaceMap::iterator FaceMapIterator;
typedef map<wstring*, wstring*> FontMap;
typedef FontMap::value_type FontMapPair;
typedef FontMap::iterator FontMapIterator;

class FontManager
{
    private:
        FontManager();
        ~FontManager();

    public:

        RENDERERS_API void init_font_list(const wchar_t* specificName = NULL);

        void create_font(void* face, long index, wchar_t const* filename);

        RENDERERS_API void AddFontAlias(const wchar_t* alias, const wchar_t* asciiName);

        FontList* GetFontList();

        const RS_Font* FindFont(const wstring& fontname, bool bold, bool italic);

        // singleton access method
        RENDERERS_API static FontManager* Instance();

    private:
        FaceMapEntryType* load_file(const char* filename);
        bool CacheFont(wchar_t const* fileName);

    private:
        void*  m_library;
        FontList    m_fontlist;
        FontMap     m_fontAliases;

        map<wstring, NameCacheEntry> m_matchedCache;

        static CustomThreadMutex sm_mutex;
        static FontManager sm_manager;
        static bool sm_init;
};

#endif
