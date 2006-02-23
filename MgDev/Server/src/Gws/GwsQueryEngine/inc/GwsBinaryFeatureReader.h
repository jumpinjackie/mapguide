// Copyright (C) 2004-2006  Autodesk, Inc.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef GWS_BINARYFEATUREWRITER_H
#define GWS_BINARYFEATUREWRITER_H

#include "GwsBinaryReader.h"

class GwsBinaryFeatureReader
{
public:
    GwsBinaryFeatureReader(unsigned char* pBuf, int len, int totalProps);
    ~GwsBinaryFeatureReader() {}

    //write property value
    GisString* QualifiedClassName();
    int SetPosition(int propertyIndex);

    // Getter by the property index
    virtual bool            IsNull      (GisInt32 iProp);
    virtual GisString   *   GetString   (GisInt32 iProp);
    virtual bool            GetBoolean  (GisInt32 iProp);
    virtual GisByte         GetByte     (GisInt32 iProp);
    virtual GisDateTime     GetDateTime (GisInt32 iProp);
    virtual double          GetDouble   (GisInt32 iProp);
    virtual GisInt16        GetInt16    (GisInt32 iProp);
    virtual GisInt32        GetInt32    (GisInt32 iProp);
    virtual GisInt64        GetInt64    (GisInt32 iProp);
    virtual float           GetSingle   (GisInt32 iProp);
    virtual const GisByte * GetGeometry (GisInt32 iProp, GisInt32 * count);
    virtual GisByteArray*   GetGeometry (GisInt32 iProp);
    //virtual FdoIFeatureReader* GetFeatureObject (GisInt32 iProp);

private:
    BinaryReader m_rdr;
    std::wstring m_className;
    int m_fcNameLen;
    int m_totalProps;
};


#endif //GWS_BINARYFEATUREWRITER_H

