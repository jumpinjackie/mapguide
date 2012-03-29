#ifndef DESKTOP_SQL_READER_H
#define DESKTOP_SQL_READER_H

#include "MgDesktop.h"

class FdoISQLDataReader;
class MgFeatureConnection;

class MG_DESKTOP_API MgdSqlDataReader : public MgSqlDataReader
{
INTERNAL_API:
	MgdSqlDataReader(MgFeatureConnection* conn, FdoISQLDataReader* reader);

EXTERNAL_API:
    virtual ~MgdSqlDataReader();

PUBLISHED_API:
    virtual bool ReadNext();

    virtual INT32 GetPropertyCount();

    virtual STRING GetPropertyName(INT32 index);

    virtual INT32 GetPropertyType(CREFSTRING propertyName);

    virtual INT32 GetPropertyType(INT32 index);

    virtual bool IsNull(CREFSTRING propertyName);

    virtual bool GetBoolean(CREFSTRING propertyName);

    virtual BYTE GetByte(CREFSTRING propertyName);

    virtual MgDateTime* GetDateTime(CREFSTRING propertyName);

    virtual float GetSingle(CREFSTRING propertyName);

    virtual double GetDouble(CREFSTRING propertyName);

    virtual INT16 GetInt16(CREFSTRING propertyName);

    virtual INT32 GetInt32(CREFSTRING propertyName);

    virtual INT64 GetInt64(CREFSTRING propertyName);

    virtual STRING GetString(CREFSTRING propertyName);

    virtual MgByteReader* GetBLOB(CREFSTRING propertyName);

    virtual MgByteReader* GetCLOB(CREFSTRING propertyName);

    virtual MgByteReader* GetGeometry(CREFSTRING propertyName);

    virtual MgRaster* GetRaster(CREFSTRING propertyName);

    virtual INT32 GetPropertyIndex(CREFSTRING propertyName);

    virtual bool IsNull(INT32 index);

    virtual bool GetBoolean(INT32 index);

    virtual BYTE GetByte(INT32 index);

    virtual MgDateTime* GetDateTime(INT32 index);

    virtual float GetSingle(INT32 index);

    virtual double GetDouble(INT32 index);

    virtual INT16 GetInt16(INT32 index);

    virtual INT32 GetInt32(INT32 index);

    virtual INT64 GetInt64(INT32 index);

    virtual STRING GetString(INT32 index);

    virtual MgByteReader* GetBLOB(INT32 index);

    virtual MgByteReader* GetCLOB(INT32 index);

    virtual MgByteReader* GetGeometry(INT32 index);

    virtual MgRaster* GetRaster(INT32 index);

    virtual void Close();

    virtual INT32 GetReaderType();

EXTERNAL_API:

    virtual MgByteReader* ToXml();

INTERNAL_API:

    MgByteReader* GetLOB(CREFSTRING propertyName);

    virtual const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

    virtual void Serialize(MgStream* stream);

    virtual void Deserialize(MgStream* stream);

protected:
	virtual void Dispose() { delete this; }

private:
	FdoISQLDataReader* m_reader;
    Ptr<MgFeatureConnection> m_connection;
};

#endif