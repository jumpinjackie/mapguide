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

#ifndef RS_Font_H_
#define RS_Font_H_

#include <string>
#include <list>

using namespace std;

struct RS_Font
{
    public :

    public :
        RS_Font(void) :
            m_bold(false),
            m_italic(false),
            m_units_per_EM(0),
            m_ascender(0),
            m_descender(0),
            m_height(0),
            m_max_advance_width(0),
            m_max_advance_height(0),
            m_underline_position(0),
            m_underline_thickness(0),
            m_fullname(L""),
            m_familyname(L""),
            m_index(0),
            m_filename(L""),
            m_capheight(0)
        {
        }

    public:

        //font style
        bool m_bold;
        bool m_italic;

        //font metrics
        unsigned short m_units_per_EM;
        short          m_ascender;
        short          m_descender;
        short          m_height;
        short          m_capheight;

        short          m_max_advance_width;
        short          m_max_advance_height;

        short          m_underline_position;
        short          m_underline_thickness;

        //font names
        wstring  m_fullname;
        wstring  m_familyname;
        long  m_index;
        wstring  m_filename;
};

typedef list<RS_Font*> FontList;
typedef list<RS_Font*>::iterator FontListIterator;

#endif  //  RS_Font_H_
