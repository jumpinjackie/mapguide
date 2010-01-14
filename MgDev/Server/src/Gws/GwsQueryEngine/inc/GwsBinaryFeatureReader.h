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

#ifndef GWS_BINARYFEATUREWRITER_H
#define GWS_BINARYFEATUREWRITER_H

#include "GwsBinaryReader.h"

///////////////////////////////////////////////////////////////////////////////
class GwsBinaryFeatureReader
{
public:
    GwsBinaryFeatureReader(unsigned char* pBuf, int len, int totalProps);
    ~GwsBinaryFeatureReader() {}

    //write property value
    FdoString* QualifiedClassName();
    int SetPosition(int propertyIndex);

    // Getter by the property index
    virtual bool            IsNull      (FdoInt32 iProp);
    virtual FdoString   *   GetString   (FdoInt32 iProp);
    virtual bool            GetBoolean  (FdoInt32 iProp);
    virtual FdoByte         GetByte     (FdoInt32 iProp);
    virtual FdoDateTime     GetDateTime (FdoInt32 iProp);
    virtual double          GetDouble   (FdoInt32 iProp);
    virtual FdoInt16        GetInt16    (FdoInt32 iProp);
    virtual FdoInt32        GetInt32    (FdoInt32 iProp);
    virtual FdoInt64        GetInt64    (FdoInt32 iProp);
    virtual float           GetSingle   (FdoInt32 iProp);
    virtual const FdoByte * GetGeometry (FdoInt32 iProp, FdoInt32 * count);
    virtual FdoByteArray*   GetGeometry (FdoInt32 iProp);
    //virtual FdoIFeatureReader* GetFeatureObject (FdoInt32 iProp);

private:
    BinaryReader m_rdr;
    std::wstring m_className;
    int m_fcNameLen;
    int m_totalProps;
};

#endif //GWS_BINARYFEATUREWRITER_H
