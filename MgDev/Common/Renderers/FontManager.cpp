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

#include "stdafx.h"
#include "FontManager.h"
#include "UnicodeString.h"
#include <algorithm>

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
#include <wctype.h>
#endif


//  local functions
wstring ToLower(const wstring& source)
{
    wstring lower = source;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    return lower;
}


//  static initializations
CustomThreadMutex FontManager::sm_mutex;
FontManager FontManager::sm_manager;


//-------------------------------------------------------------------------
//  Constructor/Destructor
//-------------------------------------------------------------------------


FontManager::FontManager()
{
    AutoMutexLocker autoLocker(sm_mutex);

    FT_Init_FreeType(&m_library);  //  TODO:  check error code

    sm_manager.init_font_list();
}


FontManager::~FontManager()
{
    AutoMutexLocker autoLocker(sm_mutex);

    // free up font map entries
    for (FontMapIterator fmi = m_fontAliases.begin(); fmi != m_fontAliases.end(); ++fmi)
    {
        delete (*fmi).first;
        delete (*fmi).second;
    }

    m_fontAliases.clear();

    // clean up fontlist
    RS_Font* font = NULL;
    FontListIterator it_font = m_fontlist.begin();
    while (it_font != m_fontlist.end())
    {
        font = (RS_Font*)(*it_font);
        delete font;
        ++it_font;
    }

    // clean fontlist
    m_fontlist.clear();

    // clean up the library
    FT_Done_FreeType(m_library);
}


//-------------------------------------------------------------------------
//  Methods
//-------------------------------------------------------------------------


FontManager* FontManager::Instance()
{
    return &sm_manager;
}


FaceMapEntryType* FontManager::load_file(const char* filename)
{
    AutoMutexLocker autoLocker(sm_mutex);

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
// initialize the font list
void FontManager::init_font_list()
{
    AutoMutexLocker autoLocker(sm_mutex);

    int error = 0;

    // look for the windows font directory
    wchar_t fontpath[MAX_PATH];
    HRESULT hres = SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, fontpath);

    wstring fontdir;
    if (S_OK == hres)
        fontdir = fontpath;

    // enum fonts
    if (!fontdir.empty())
    {
        wstring dir(fontdir);
        dir += L"\\*";

        WIN32_FIND_DATA FindFileData;
        HANDLE hFile = FindFirstFile(dir.c_str(), &FindFileData);

        BOOL bOK = TRUE;
        while (bOK && hFile != INVALID_HANDLE_VALUE)
        {
            // do we have a file?
            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // skip special Windows files
                if (wcscmp(FindFileData.cFileName, L"desktop.ini") != 0)
                {
                    wstring entryName(fontdir);
                    entryName += L"\\";
                    entryName += FindFileData.cFileName;

                    // ok, load up the face information
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
                            // init num_faces if necessary
                            if (!num_faces)
                                num_faces = face->num_faces;
                        }

                        create_font(face, index, entryName.c_str());

                        // dispose of face
                        FT_Done_Face(face);

                        // increment our face index
                        index++;
                    }
                    while (!error && index < num_faces);
                }
            }

            bOK = FindNextFile(hFile, &FindFileData);
        }

        FindClose(hFile);
    }

    // The font matching code iterates over the font list, and the first font in the
    // list always becomes the one to beat.  In the case where none of the fonts match
    // (they all have the same score), this first font ends up winning.  So make sure
    // a reasonably good font, like Arial, is the first one in the list.
    wstring lowerName = L"arial"; // use lower-case
    RS_Font* font = (RS_Font*)FindFont(lowerName, false, false);
    if (font && font->m_fullname == lowerName)
    {
        // we found Arial - move it to the front
        m_fontlist.remove(font);
        m_fontlist.push_front(font);
    }
}
#else
int FontManager::AddLinuxFontDirectory(const char* fontDir)
{
    AutoMutexLocker autoLocker(sm_mutex);
    int error = 0;
    int added = 0;

    std::string sFontDir;
    sFontDir.append(fontDir);
    DIR* pCurrent = opendir(fontDir);

    while (pCurrent)
    {
        errno = 0;
        dirent* pDirent = readdir(pCurrent);

        if (pDirent)
        {
            string entryName = sFontDir;
            entryName += "/";
            entryName += pDirent->d_name;

            struct stat st;
            if (stat(entryName.c_str(), &st) == 0)
            {
                // ok, load up the face information
                FT_Face face = NULL;
                FT_Long index = 0;
                FT_Long num_faces = 0;

                do
                {
                    // TODO:  revisit using the font manager here
                    error = FT_New_Face(m_library, entryName.c_str(), index, &face);
                    if (!error)
                    {
                        // init num_faces if necessary
                        if (!num_faces)
                            num_faces = face->num_faces;
                    }

                    wstring en;
                    UnicodeString::MultiByteToWideChar(entryName.c_str(), en);
                    create_font(face, index, en.c_str());
                    added++;

                    // dispose of face
                    FT_Done_Face(face);

                    // increment our face index
                    index++;
                }
                while (!error && index < num_faces);
            }
        }
        else
        {
            // either we got an error or there are no more entries
            closedir(pCurrent);
            pCurrent = NULL;    //  break out of while loop
        }
    }
    return added;
}
// initialize the font list
void FontManager::init_font_list()
{
    int added = AddLinuxFontDirectory(".");
#ifdef _DEBUG
    printf("Added %d fonts from default location\n", added);
#endif
}
#endif  //  _WIN32


// create a font with the given face information
void FontManager::create_font(FT_Face face, FT_Long index, wchar_t const* filename)
{
    // do we have a valid face?
    if (!face)
        return;

    // create a font
    RS_Font* font = new RS_Font();

    // process family name
    wstring familyname;
    UnicodeString::MultiByteToWideChar(face->family_name, familyname);
    familyname = ToLower(familyname);

    // process style name
    wstring stylename;
    UnicodeString::MultiByteToWideChar(face->style_name, stylename);
    stylename = ToLower(stylename);

    // The style name often includes the strings "regular" / "bold" / "italic",
    // and these correspond to the bold / italic attributes for the font.  We
    // don't want these strings to be part of the font's full name, however, as
    // this will affect font matching.  The font matching checks for bold / italic
    // attributes happen separately through the style flags.
    size_t indStr;
    wstring repStr;
    const wchar_t* empty = L"";

    // remove occurrences of "regular"
    repStr = L"regular";// NOXLATE
    while ((indStr = stylename.find(repStr)) != wstring::npos)
        stylename.replace(indStr, repStr.length(), empty, 0);

    // remove occurrences of "bold"
    repStr = L"bold";   // NOXLATE
    while ((indStr = stylename.find(repStr)) != wstring::npos)
        stylename.replace(indStr, repStr.length(), empty, 0);

    // remove occurrences of "italic"
    repStr = L"italic"; // NOXLATE
    while ((indStr = stylename.find(repStr)) != wstring::npos)
        stylename.replace(indStr, repStr.length(), empty, 0);

    // remove any remaining leading / trailing whitespace
    repStr = L" ";
    if (!stylename.empty())
    {
        // remove the trailing whitespace first
        indStr = stylename.find_last_of(repStr);
        if (indStr == stylename.length()-1)
            stylename.replace(indStr, repStr.length(), empty, 0);

        indStr = stylename.find(repStr);
        if (indStr == 0)
            stylename.replace(indStr, repStr.length(), empty, 0);
    }

    // process full name
    wstring fullname;
    fullname += familyname;
    if (!stylename.empty())
    {
        fullname += L" ";
        fullname += stylename;
    }

    // remember information
    font->m_familyname = familyname;
    font->m_fullname = fullname;
    font->m_filename = filename;
    font->m_index = index;

    // remember font style
    if (face->style_flags & FT_STYLE_FLAG_ITALIC)
        font->m_italic = true;

    if (face->style_flags & FT_STYLE_FLAG_BOLD)
        font->m_bold = true;

    // remember font metrics
    font->m_ascender = face->ascender;
    font->m_descender = face->descender;
    font->m_units_per_EM = face->units_per_EM;
    font->m_height = face->height;
    font->m_underline_position = face->underline_position;
    font->m_underline_thickness = face->underline_thickness;

    AutoMutexLocker autoLocker(sm_mutex);

    // add the font
    m_fontlist.push_front(font);
}


// Add an alias for a font.  Aliases are useful for specifying non-ascii names
// for a font. Or if the user wants, to remap a font name to a completely
// different font.
void FontManager::AddFontAlias(const wchar_t* alias, const wchar_t* asciiName)
{
    if (alias != NULL && asciiName != NULL)
    {
        AutoMutexLocker autoLocker(sm_mutex);

        m_fontAliases.insert(FontMapPair(new wstring(alias), new wstring(asciiName)));
    }
}


//
FontList* FontManager::GetFontList()
{
    return &m_fontlist;
}


//
const RS_Font* FontManager::FindFont(const wstring& sfontname, bool bold, bool italic)
{
    AutoMutexLocker autoLocker(sm_mutex);

    // first see if we cached the font match so we don't have to iterate
    NameCacheEntry entry = m_matchedCache[sfontname];
    int index = (int)bold | ((int)italic << 1);
    if (entry.stylearray[index])
        return entry.stylearray[index];

    const wchar_t* fontname = sfontname.c_str();

    // if there is an alias for the font use that instead
    for (FontMapIterator fmi = m_fontAliases.begin(); fmi != m_fontAliases.end(); ++fmi)
    {
        const wchar_t* alias = (*fmi).first->c_str();
        if (wcscmp(alias, fontname) == 0)
        {
            fontname = ((*fmi).second)->c_str();
            break;
        }
    }

    wstring lowername = ToLower(fontname);
    const wchar_t* buf = lowername.c_str();

    int score = 0;
    RS_Font* current = NULL;
    RS_Font* best = NULL;

    FontListIterator it = m_fontlist.begin();
    while (it != m_fontlist.end())
    {
        int tempscore = 0;
        current = (RS_Font*)(*it);

        if (NULL == best)
            best = current;

        // check full name (really the family here)
        if (wcsstr(current->m_fullname.c_str(), buf) != NULL)
        {
            tempscore += 2;

            // See if the specified font name is exactly equal to the font
            // family.  This cuts down on wrong matches for variations like
            // Arial vs. Arial Unicode MS.  No need for a case-insensitive
            // compare since all names are already converted to lower case.
            if (wcscmp(current->m_familyname.c_str(), buf) == 0)
                tempscore++;

            // Within a family we still need to do some checks.  For example,
            // the Arial family has both Arial Regular and Arial Black, and
            // with only the above checks both of these return the same score
            // if we just ask for "Arial".  Bump the score if the specified
            // font name exactly matches the full name.
            if (wcscmp(current->m_fullname.c_str(), buf) == 0)
                tempscore++;
        }

        // check bold
        if (bold == current->m_bold)
            tempscore++;

        // check italic
        if (italic == current->m_italic)
            tempscore++;

        // is the current font a better match?
        if (tempscore > score)
        {
            best = current;
            score = tempscore;
        }

        ++it;
    }

    // add to font cache map
    entry.stylearray[index] = best;
    m_matchedCache[sfontname] = entry;

    // return matched font
    return best;
}
