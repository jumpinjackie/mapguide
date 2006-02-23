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

#ifndef RS_Font_H_
#define RS_Font_H_

#include <string>
#include <list>

using namespace std;

class RS_Font
{
    public :

        //enum FontFlag
        //{
        //    ffNone                = 0x0000,
        //    //Typeface            = 0x0001,
        //    ffBold                = 0x0002,
        //    ffItalic              = 0x0004,
        //    //Charset             = 0x0008,
        //    //Pitch_And_Family    = 0x0010,
        //    //Font_Type           = 0x0020,
        //    //Panose              = 0x0040,
        //    //Height              = 0x0080,
        //    ffUnderlined          = 0x0100,
        //    ffAll                 = 0x01FF,

        //    ffUndefined = -1
        //};

    public :
        RS_Font(void);
        RS_Font(RS_Font & copy);
        virtual ~RS_Font(void);

    public :
        bool GetBold();
        void SetBold( bool isBold );
        bool GetItalic();
        void SetItalic( bool isItalic );
        bool GetUnderlined();
        void SetUnderlined( bool isUnderlined );

        wchar_t const * GetFullName();
        void SetFullName( wchar_t const * pFullName );
        wchar_t const * GetFamilyName();
        void SetFamilyName( wchar_t const * pFamilyName );
        long GetIndex();
        void SetIndex( long index) ;
        wchar_t const * GetFileName();
        void SetFileName( wchar_t const * pFileName );

    private :
        bool m_bold;
        bool m_italic;
        bool m_underlined;

        wstring  m_fullname;
        wstring  m_familyname;
        long  m_index;
        wstring  m_filename;
};

typedef list<RS_Font*> FontList;
typedef list<RS_Font*>::iterator FontListIterator;

#endif  //  RS_Font_H_
