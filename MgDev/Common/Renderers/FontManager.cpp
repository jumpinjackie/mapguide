//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "FontManager.h"
#include "UnicodeString.h"

#ifdef _WIN32
#include "shlobj.h"
#else
#include <cstddef>
#include <cstdlib>
#include <clocale>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#endif

//  local functions
void make_lower_case(char* s)
{
    while (*s)
    {
        *s = (char)tolower((int)*s);
        s++;
    }
}

//  static initializations
FontManager FontManager::m_manager;


//-------------------------------------------------------------------------
//  Constructor/Desctructor
//-------------------------------------------------------------------------


FontManager::FontManager()
{
    FT_Init_FreeType(&m_library);  //  TODO:  check error code

    m_manager.init_font_list();
}


FontManager::~FontManager()
{
    FaceMapIterator it;
    FaceMapEntryType* pEntry = NULL;
    char* pData = NULL;

    it = m_facemap.begin();

    //  clean up entries
    while (it != m_facemap.end())
    {
        pEntry = (FaceMapEntryType*)(*it).second;
        pData = pEntry->pData;
        free(pData);
        delete pEntry;
        ++it;
    }

    // free up font map entries
    for (FontMapIterator fmi = m_fontAliases.begin(); fmi != m_fontAliases.end(); fmi++)
    {
        delete (*fmi).first;
        delete (*fmi).second;
    }

    m_fontAliases.clear();

    //  clear map
    m_facemap.clear();

    //  clean up fontlist
    FontListIterator it_font;
    RS_Font* font = NULL;

    it_font = m_fontlist.begin();

    while (it_font != m_fontlist.end())
    {
        font = (RS_Font*)(*it_font);
        delete font;
        ++it_font;
    }

    //  clean fontlist
    m_fontlist.clear();

    // clean up the library
    FT_Done_FreeType(m_library);
}


//-------------------------------------------------------------------------
//  Methods
//-------------------------------------------------------------------------


FontManager* FontManager::Instance()
{
    return &m_manager;
}


int FontManager::get_face(const char* filename, FT_Long index, FT_Face* face)
{
    int ret = 0;                      //  our return error code
    FaceMapIterator it;               //  an interator
    FaceMapEntryType* pEntry = NULL;  //  pointer to loaded font data

    //  look for face in map
    it = m_facemap.find(filename);

    if (it != m_facemap.end())
    {
        //  found an entry
        pEntry = (FaceMapEntryType*)(*it).second;

        //  create a new face
        ret = FT_New_Memory_Face(m_library, (FT_Byte*)pEntry->pData, (FT_Long)pEntry->length, index, face);
    }
    else
    {
        //  ok, we have to load it
//      ret = FT_New_Face(m_library, filename, index, face);

        pEntry = load_file(filename);

        if (pEntry)
        {
            //  insert the entry into the map
            m_facemap.insert(FaceMapPair(filename, pEntry));

            //  create a new face
            ret = FT_New_Memory_Face(m_library, (FT_Byte*)pEntry->pData, (FT_Long)pEntry->length, index, face);
        }
        else
        {
            face = NULL;
            ret = -1;
        }
    }

    return ret;
}


FaceMapEntryType* FontManager::load_file(const char* filename)
{
    int errcode = 0;                  //  an error code
    FaceMapEntryType* pEntry = NULL;  //  pointer to return value
    char* pData = NULL;               //  pointer to loaded font file
    size_t length = 0;                //  length of buffer
    size_t count = 0;                 //  length of file (obviously should be equal to above)

    FILE* pFile = fopen(filename, "rb");

    if (pFile)
    {
        errcode = fseek(pFile, 0L, SEEK_END);
        if (!errcode)
        {
            length = (unsigned long)ftell(pFile);

            errcode = fseek(pFile, 0L, SEEK_SET);
            if (!errcode)
            {
                pData = (char*)malloc(sizeof(char) * length);
                if (pData)
                {
                    count = fread(pData, sizeof(char), length, pFile);

                    if (ferror(pFile) == 0 && count == length)
                        errcode = fclose(pFile);
                }
            }
        }
    }

    if (pData)
    {
        pEntry = new FaceMapEntryType();
        pEntry->pData = pData;
        pEntry->length = length;
    }

    return pEntry;
}


#ifdef _WIN32
//  initialize the font list
void FontManager::init_font_list()
{
    int error = 0;

    // look for the windows font directory
    wchar_t fontpath[MAX_PATH];
    HRESULT hres = SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, fontpath);

    wstring fontdir;
    if (S_OK == hres)
        fontdir = fontpath;

    //  enum fonts
    if (!fontdir.empty())
    {
        wstring dir(fontdir);
        dir += L"\\*";

        WIN32_FIND_DATA FindFileData;
        HANDLE hFile = FindFirstFile(dir.c_str(), &FindFileData);

        BOOL bOK = TRUE;
        while (bOK && hFile != INVALID_HANDLE_VALUE)
        {
            //  do we have a file?
            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                //  ok, load up the face information
                wstring entryName(fontdir);
                entryName += L"\\";
                entryName += FindFileData.cFileName;

                FT_Face face = NULL;
                FT_Long index = 0;
                FT_Long num_faces = 0;

                do
                {
                    string en;
                    UnicodeString::WideCharToMultiByte(entryName.c_str(), en);
                    error = FT_New_Face(m_library, en.c_str(), index, &face);

                    if (!error)
                    {
                        //  init num_faces if necessary
                        if (!num_faces)
                            num_faces = face->num_faces;
                    }

                    create_font(face, index, entryName.c_str());

                    //  dispose of face
                    FT_Done_Face(face);

                    //  increment our face index
                    index++;
                }
                while (!error && index < num_faces);
            }

            bOK = FindNextFile(hFile, &FindFileData);
        }

        FindClose(hFile);
    }
}
#else
//  initialize the font list
void FontManager::init_font_list()
{
    int error = 0;

    string dirname(".");
    DIR* pCurrent = opendir(dirname.c_str());

    while (pCurrent)
    {
        errno = 0;
        dirent* pDirent = readdir(pCurrent);

        if (pDirent)
        {
            string entryName = dirname;
            entryName += "/";
            entryName += pDirent->d_name;

            struct stat st;
            if (stat(entryName.c_str(), &st) == 0)
            {
                //  ok, load up the face information
                FT_Face face = NULL;
                FT_Long index = 0;
                FT_Long num_faces = 0;

                do
                {
                    error = FT_New_Face(m_library, entryName.c_str(), index, &face);

                    //  TODO:  revisit using the font manager here
                    //error = m_FM->get_face(entryName.c_str(), index, &face);

                    if (!error)
                    {
                        //  init num_faces if necessary
                        if (!num_faces)
                            num_faces = face->num_faces;
                    }

                    wstring en;
                    UnicodeString::MultiByteToWideChar(entryName.c_str(), en);
                    create_font(face, index, en.c_str());

                    //  dispose of face
                    FT_Done_Face(face);

                    //  increment our face index
                    index++;
                }
                while (!error && index < num_faces);
            }
        }
        else
        {
            //  either we got an error or there are no more entries
            closedir(pCurrent);
            pCurrent = NULL;    //  break out of while loop
        }
    }
}
#endif  //  _WIN32


//  create a font with the given face information
void FontManager::create_font(FT_Face face, FT_Long index, wchar_t const* filename)
{
    //  do we have a valid face?
    if (!face)
        return;

    //  create a font
    RS_Font* font = new RS_Font();

    //  set font attributes
    wstring familyname;
    UnicodeString::MultiByteToWideChar(face->family_name, familyname);

#ifdef _WIN32
    wchar_t* buf = (wchar_t*)alloca(sizeof(wchar_t) * (familyname.length()+1));
    wcscpy(buf, familyname.c_str());
    wcslwr(buf);
    buf[familyname.length()] = 0;
    familyname = buf;
#else
    string tempstr;
    UnicodeString::WideCharToMultiByte(familyname.c_str(), tempstr);
    make_lower_case((char*)tempstr.c_str());
    UnicodeString::MultiByteToWideChar(tempstr.c_str(), familyname);
#endif

    font->m_familyname = familyname;

    wstring fullname;
    fullname += familyname;
    fullname += L" ";

    wstring stylename;
    UnicodeString::MultiByteToWideChar(face->style_name, stylename);
    fullname += stylename;

#ifdef _WIN32
    buf = (wchar_t*)alloca(sizeof(wchar_t) * (fullname.length()+1));
    wcscpy(buf, fullname.c_str());
    wcslwr(buf);
    buf[fullname.length()] = 0;
    fullname = buf;
#else
    tempstr.clear();
    UnicodeString::WideCharToMultiByte(fullname.c_str(), tempstr);
    make_lower_case((char*)tempstr.c_str());
    UnicodeString::MultiByteToWideChar(tempstr.c_str(), fullname);
#endif

    font->m_fullname = fullname;
    font->m_filename = filename;
    font->m_index = index;

    //remember font style
    if (face->style_flags & FT_STYLE_FLAG_ITALIC)
        font->m_italic = true;

    if (face->style_flags & FT_STYLE_FLAG_BOLD)
        font->m_bold = true;

    //remember font metrics
    font->m_ascender = face->ascender;
    font->m_descender = face->descender;
    font->m_units_per_EM = face->units_per_EM;
    font->m_height = face->height;
    font->m_underline_position = face->underline_position;
    font->m_underline_thickness = face->underline_thickness;

    //  add the font
    m_fontlist.push_front(font);
}


// Add an alias for a font.  Aliases are useful for specifying non-ascii names
// for a font. Or if the user wants, to remap a font name to a completely
// different font.
void FontManager::AddFontAlias(const wchar_t* alias, const wchar_t* asciiName)
{
    if (alias != NULL && asciiName != NULL)
        m_fontAliases.insert(FontMapPair(new wstring(alias), new wstring(asciiName)));
}


//
FontList* FontManager::GetFontList()
{
    return &m_fontlist;
}


//
const RS_Font* FontManager::FindFont(const wstring& sfontname, bool bold, bool italic)
{
    //first see if we cached the font match so we don't have to iterate
    NameCacheEntry entry = m_matchedCache[sfontname];
    int index = (int)bold | ((int)italic << 1);
    if (entry.stylearray[index])
        return entry.stylearray[index];

    const wchar_t* fontname = sfontname.c_str();

    // If there is an alias for the font use that instead
    for (FontMapIterator fmi = m_fontAliases.begin(); fmi != m_fontAliases.end(); fmi++)
    {
        const wchar_t* alias = (*fmi).first->c_str();
        if (wcscmp(alias, fontname) == 0)
        {
            fontname = ((*fmi).second)->c_str();
            break;
        }
    }

    FontListIterator it = m_fontlist.begin();

    size_t len = wcslen(fontname);
    wchar_t* buf = (wchar_t*)alloca(sizeof(wchar_t) * (len+1));

#ifdef _WIN32
    wcscpy(buf, fontname);
    wcslwr(buf);
#else
    //I think ::towlower can be used on Linux without
    //having to go through multibyte. TODO
    string tempstr;
    UnicodeString::WideCharToMultiByte(fontname, tempstr);
    make_lower_case((char*)tempstr.c_str());
    wstring fn;
    UnicodeString::MultiByteToWideChar(tempstr.c_str(), fn);
    wcscpy(buf, fn.c_str());
#endif
    buf[len] = 0;

    int score = 0;
    RS_Font* current = NULL;
    RS_Font* best = NULL;

    while (it != m_fontlist.end())
    {
        int tempscore = 0;
        current = (RS_Font*)(*it);

        if (NULL == best)
            best = current;

        //  check full name (really the family here)
        if (wcsstr(current->m_fullname.c_str(), buf) != NULL)
        {
            tempscore += 2;

            //see if specified font name is exactly equal to font family
            //This will cut down on wrong match for family variations like
            //Arial Black vs. Arial Narrow vs. Arial
            //no need for a case-insensiteive compare since all names
            //are already converted to lower case
            if (wcscmp(current->m_familyname.c_str(), buf) == 0)
                tempscore ++;
        }

        //  check bold
        if (bold == current->m_bold)
            tempscore++;

        //  check italic
        if (italic == current->m_italic)
            tempscore++;

        //  is the current font a better match?
        if (tempscore > score)
        {
            best = current;
            score = tempscore;
        }

        ++it;
    }

    //add to font cache map 
    entry.stylearray[index] = best;
    m_matchedCache[sfontname] = entry;

    //  return matched font
    return best;
}
