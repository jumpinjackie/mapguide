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

#ifndef _MgOgcException_h
#define _MgOgcException_h

// Forward declaration;
class CStream;

class MgOgcException
{
public:
    MgOgcException(CPSZ pszElementName,CPSZ pszType,CPSZ pszMessage)
    : m_pszElementName(pszElementName)
    , m_pszType(pszType)
    , m_pszMessage(pszMessage)
    {
    }

    virtual void WriteTo(CStream& Response) const = 0;

public:
    virtual CPSZ ElementName() const  { return m_pszElementName; }
    virtual CPSZ Type()        const  { return m_pszType;        }
    virtual CPSZ Message()     const  { return m_pszMessage;     }

    static CPSZ kpszInternalError;

private:
    CPSZ m_pszElementName;
    CPSZ m_pszType;
    CPSZ m_pszMessage;
};

#endif//_MgOgcException_h
