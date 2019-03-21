//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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

#ifndef LIMITING_FEATURE_READER_H
#define LIMITING_FEATURE_READER_H

#include "PlatformBase.h"

class MgLimitingReader : public MgReader
{
public:
    MgLimitingReader(MgReader* inner, INT32 limit)
    {
        m_inner = SAFE_ADDREF(inner);
        m_read = 0;
        m_limit = limit;
    }
    virtual ~MgLimitingReader()
    {
        m_inner = NULL;
    }

    // Inherited via MgReader
    virtual void Dispose()
    {
        delete this;
    }

    virtual bool ReadNext()
    {
        bool bRet = m_inner->ReadNext();
        if (bRet)
        {
            m_read++;
            if (m_read > m_limit) //Reached limit
                return false;
        }
        return bRet;
    }

    virtual INT32 GetPropertyCount() { return m_inner->GetPropertyCount(); }

    virtual STRING GetPropertyName(INT32 index) { return m_inner->GetPropertyName(index); }

    virtual INT32 GetPropertyType(CREFSTRING propertyName) { return m_inner->GetPropertyType(propertyName); }

    virtual INT32 GetPropertyType(INT32 index) { return m_inner->GetPropertyType(index); }

    virtual bool IsNull(CREFSTRING propertyName) { return m_inner->IsNull(propertyName); }

    virtual bool GetBoolean(CREFSTRING propertyName) { return m_inner->GetBoolean(propertyName); }

    virtual BYTE GetByte(CREFSTRING propertyName) { return m_inner->GetByte(propertyName); }

    virtual MgDateTime * GetDateTime(CREFSTRING propertyName) { return m_inner->GetDateTime(propertyName); }

    virtual float GetSingle(CREFSTRING propertyName) { return m_inner->GetSingle(propertyName); }

    virtual double GetDouble(CREFSTRING propertyName) { return m_inner->GetDouble(propertyName); }

    virtual INT16 GetInt16(CREFSTRING propertyName) { return m_inner->GetInt16(propertyName); }

    virtual INT32 GetInt32(CREFSTRING propertyName) { return m_inner->GetInt32(propertyName); }

    virtual INT64 GetInt64(CREFSTRING propertyName) { return m_inner->GetInt64(propertyName); }

    virtual STRING GetString(CREFSTRING propertyName) { return m_inner->GetString(propertyName); }

    virtual MgByteReader * GetBLOB(CREFSTRING propertyName) { return m_inner->GetBLOB(propertyName); }

    virtual MgByteReader * GetCLOB(CREFSTRING propertyName) { return m_inner->GetCLOB(propertyName); }

    virtual MgByteReader * GetGeometry(CREFSTRING propertyName) { return m_inner->GetGeometry(propertyName); }

    virtual MgRaster * GetRaster(CREFSTRING propertyName) { return m_inner->GetRaster(propertyName); }

    virtual INT32 GetPropertyIndex(CREFSTRING propertyName) { return m_inner->GetPropertyIndex(propertyName); }

    virtual bool IsNull(INT32 index) { return m_inner->IsNull(index); }

    virtual bool GetBoolean(INT32 index) { return m_inner->GetBoolean(index); }

    virtual BYTE GetByte(INT32 index) { return m_inner->GetByte(index); }

    virtual MgDateTime * GetDateTime(INT32 index) { return m_inner->GetDateTime(index); }

    virtual float GetSingle(INT32 index) { return m_inner->GetSingle(index); }

    virtual double GetDouble(INT32 index) { return m_inner->GetDouble(index); }

    virtual INT16 GetInt16(INT32 index) { return m_inner->GetInt16(index); }

    virtual INT32 GetInt32(INT32 index) { return m_inner->GetInt32(index); }

    virtual INT64 GetInt64(INT32 index) { return m_inner->GetInt64(index); }

    virtual STRING GetString(INT32 index) { return m_inner->GetString(index); }

    virtual MgByteReader * GetBLOB(INT32 index) { return m_inner->GetBLOB(index); }

    virtual MgByteReader * GetCLOB(INT32 index) { return m_inner->GetCLOB(index); }

    virtual MgByteReader * GetGeometry(INT32 index) { return m_inner->GetGeometry(index); }

    virtual MgRaster * GetRaster(INT32 index) { return m_inner->GetRaster(index); }

    virtual void Close() { m_inner->Close(); }

    virtual INT32 GetReaderType() { return m_inner->GetReaderType(); };

    virtual string GetBodyElementName() { return m_inner->GetBodyElementName(); }

    virtual void ResponseStartUtf8(string & str) { return m_inner->ResponseStartUtf8(str); }

    virtual void ResponseEndUtf8(string & str) { return m_inner->ResponseEndUtf8(str); }

    virtual void BodyStartUtf8(string & str) { return m_inner->BodyStartUtf8(str); }

    virtual void BodyEndUtf8(string & str) { return m_inner->BodyEndUtf8(str); }

    virtual void HeaderToStringUtf8(string & str) { return m_inner->HeaderToStringUtf8(str); }

    virtual void CurrentToStringUtf8(string & str) { return m_inner->CurrentToStringUtf8(str); }

    virtual const wchar_t * GetString(CREFSTRING propertyName, INT32 & length) { return m_inner->GetString(propertyName, length); }

    virtual void Serialize(MgStream * stream) { m_inner->Serialize(stream); }

    virtual void Deserialize(MgStream * stream) { m_inner->Deserialize(stream); }

    virtual MgByteReader * ToXml() { return m_inner->ToXml(); }

    virtual std::string GetResponseElementName() { return m_inner->GetResponseElementName(); }

private:
    Ptr<MgReader> m_inner;
    INT32 m_limit;
    INT32 m_read;
};

#endif