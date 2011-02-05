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

#ifndef _XmlElementEmitter_h
#define _XmlElementEmitter_h

#include "ResponseStream.h"

////////////////////////////////////////////////////////////////////////////////
// Handy little tool to make sure <x> is always matched with </x> when writing
class MgXmlElementEmitter
{
public:
    MgXmlElementEmitter(CStream& StreamOut,CPSZ pszElement,CPSZ pszAttributes = NULL)
    : m_StreamOut(StreamOut)
    , m_pszElement(pszElement)
    {
        Write(_("<")); // NOXLATE
        Write(m_pszElement);
        if (pszAttributes != NULL && pszAttributes[0] != '\0') {
            Write(_(" "));// NOXLATE
            Write(pszAttributes);
        }
        Write(_(">"));// NOXLATE
    }
    ~MgXmlElementEmitter()
    {
        Write(_("</"));// NOXLATE
        Write(m_pszElement);
        Write(_(">\r\n"));// NOXLATE
    }

    bool Write(CPSZ pszOut,size_t iCount)
    {
        size_t uWritten;
        iCount *= sizeof(wchar_t);
        m_StreamOut.Write(pszOut,iCount,&uWritten);
        return uWritten == iCount;
    }

    bool Write(CPSZ pszOut)
    {
        return Write(pszOut,szlen(pszOut));
    }

private:
    CStream& m_StreamOut;
    CPSZ     m_pszElement;
};

#endif//_XmlElementEmitter_h
