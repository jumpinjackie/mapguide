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

#include "RS_Font.h"

//-----------------------------------------------------------------------------
//  Constrctors and Destructors
//-----------------------------------------------------------------------------

//  default constructor
RS_Font::RS_Font(void) :
    m_bold(false),
    m_italic(false),
    m_underlined(false)
{
}

//  copy constructor
RS_Font::RS_Font(RS_Font & copy)
{
    m_bold = copy.GetBold();
    m_italic = copy.GetItalic();
    m_underlined = copy.GetUnderlined();

    m_index = copy.GetIndex();
    m_fullname = copy.GetFullName();

    m_fullname = copy.GetFullName();
    m_familyname = copy.GetFamilyName();
    m_filename = copy.GetFileName();
}

//  destructor
RS_Font::~RS_Font(void)
{
}

//-----------------------------------------------------------------------------
//  Other Methods
//-----------------------------------------------------------------------------

//
bool RS_Font::GetBold()
{
    return m_bold;
}

//
void RS_Font::SetBold( bool isBold )
{
    m_bold = isBold;
}

//
bool RS_Font::GetItalic()
{
    return m_italic;
}

//
void RS_Font::SetItalic( bool isItalic )
{
    m_italic = isItalic;
}

//
bool RS_Font::GetUnderlined()
{
    return m_underlined;
}

//
void RS_Font::SetUnderlined( bool isUnderlined )
{
    m_underlined = isUnderlined;
}

//
wchar_t const * RS_Font::GetFullName()
{
    return m_fullname.c_str();
}

//
void RS_Font::SetFullName( wchar_t const * pFullName )
{
    m_fullname = pFullName;
}

//
wchar_t const * RS_Font::GetFamilyName()
{
    return m_familyname.c_str();
}

//
void RS_Font::SetFamilyName( wchar_t const * pFamilyName )
{
    m_familyname = pFamilyName;
}

long RS_Font::GetIndex()
{
    return m_index;
}

//
void RS_Font::SetIndex( long index )
{
    m_index = index;
}

//
wchar_t const * RS_Font::GetFileName()
{
    return m_filename.c_str();
}

//
void RS_Font::SetFileName( wchar_t const * pFileName )
{
    m_filename = pFileName;
}
