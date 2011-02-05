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

#include "Foundation.h"
#include "StreamHelper.h"

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

MgStreamHelper::MgStreamHelper()
{
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for a MgStreamHelper object.
//  </summary>
MgStreamHelper::~MgStreamHelper()
{
};

void MgStreamHelper::Dispose()
{
    delete this;
}

bool MgStreamHelper::IsConnected()
{
    return true;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Standard protocol handling for strings, via the UTF-16 standard.
/// Uses WriteBytes internally.
///
MgStreamHelper::MgStreamStatus MgStreamHelper::WriteString(CREFSTRING str)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    UINT8 charSize = sizeof(wchar_t);
    stat = WriteUINT8(charSize);

    UINT32 len = (UINT32) str.length()+1;
    if (mssDone ==  stat) stat = WriteUINT32(len);
    if (mssDone ==  stat) stat = WriteBytes((unsigned char*) str.c_str(), len*charSize);

    return stat;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Standard protocol handling for strings, via the UTF-16 standard.
/// Uses GetData internally.
///
MgStreamHelper::MgStreamStatus MgStreamHelper::GetString(REFSTRING wcStr)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;
    UINT8 charSize = 0;
    stat = GetUINT8(charSize);

    UINT32 len = 0;
    if (mssDone ==  stat) stat = GetUINT32(len);

    if (mssDone != stat) return stat;

    // Check for 0 length string
    if(len == 0)
    {
        wcStr = L"";
        return stat;
    }

    UINT8 currSize = sizeof(wchar_t);
    if (currSize == charSize)
    {
        wcStr.reserve(len);
        wcStr.resize(len-1);
        stat = GetData( (void*)wcStr.c_str(), len*charSize , true, false );
    }
    else if (2 == currSize && 4 == charSize)
    {
        // Do UTF32 to UTF16 conversion
        LCh* wideStr = new LCh[len];
        stat = GetData( wideStr, len*charSize, true, false );
        if (mssDone == stat)
        {
            xstring xStr;
            UnicodeString::UTF32toUTF16(wideStr, xStr);
            wcStr = (wchar_t*) xStr.c_str();
        }
        delete [] wideStr;
    }
    else if (4 == currSize && 2 == charSize)
    {
        // Do UTF16 to UTF32 conversion
        XMLCh* wideStr = new XMLCh[len];
        stat = GetData( wideStr, len*charSize, true, false );
        if (mssDone == stat)
        {
            lstring lStr;
            UnicodeString::UTF16toUTF32(wideStr, lStr);
            wcStr = (wchar_t*) lStr.c_str();
        }
        delete [] wideStr;
    }
    else
    {
        throw new MgStreamIoException(L"MgStreamHelper.GetString", __LINE__, __WFILE__, NULL, L"MgInvalidTCPProtocol", NULL);
        stat = MgStreamHelper::mssError;
    }

    return stat;
}
