#ifndef DESKTOP_FEATURE_READER_H
#define DESKTOP_FEATURE_READER_H

#include "MgDesktop.h"

class FdoIFeatureReader;
class MgCoordinateSystemTransform;
class MgAgfReaderWriter;
class MgFeatureConnection;

class MG_DESKTOP_API MgdFeatureReader : public MgFeatureReader
{
INTERNAL_API:
	MgdFeatureReader(MgFeatureConnection* conn, FdoIFeatureReader* reader);

EXTERNAL_API:
	virtual ~MgdFeatureReader();

	//MgReader
PUBLISHED_API:
    virtual bool ReadNext();

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

    virtual const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

    virtual void Serialize(MgStream* stream);

    virtual void Deserialize(MgStream* stream);

	//MgFeatureReader
PUBLISHED_API:
    virtual MgClassDefinition* GetClassDefinition();

    virtual MgFeatureReader* GetFeatureObject(CREFSTRING propertyName);

    virtual MgFeatureReader* GetFeatureObject(INT32 index);

INTERNAL_API:

    virtual BYTE_ARRAY_OUT GetGeometry(CREFSTRING propertyName, INT32& length);

    virtual BYTE_ARRAY_OUT GetGeometry(INT32 index, INT32& length);

INTERNAL_API:

    virtual MgClassDefinition* GetClassDefinitionNoXml();

    virtual MgFeatureSet* GetFeatures(INT32 count);

    MgByteReader* GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize);

protected:
	virtual void Dispose() { delete this; }

INTERNAL_API:
    FdoIFeatureReader* GetInternalReader();

protected:
	FdoIFeatureReader* m_reader;
    Ptr<MgClassDefinition> m_classDef;
    Ptr<MgFeatureConnection> m_connection;
};

#endif