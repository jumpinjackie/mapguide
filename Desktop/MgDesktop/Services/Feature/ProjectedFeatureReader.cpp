#include "MgDesktop.h"

MgProjectedFeatureReader::MgProjectedFeatureReader(MgFeatureReader* reader, MgCoordinateSystemTransform* trans)
{
    m_reader = SAFE_ADDREF(reader);
    m_trans = SAFE_ADDREF(trans);
    m_agfRw = new MgAgfReaderWriter();
}

MgProjectedFeatureReader::~MgProjectedFeatureReader()
{
    m_reader = NULL;
    m_trans = NULL;
    m_agfRw = NULL;
}

MgByteReader* MgProjectedFeatureReader::GetGeometry(INT32 index)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_reader)->GetGeometry(index);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_trans);
    ret = m_agfRw->Write(geom);
    
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetGeometry")

    return ret.Detach();
}

MgByteReader* MgProjectedFeatureReader::GetGeometry(CREFSTRING name)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_reader)->GetGeometry(name);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_trans);
    ret = m_agfRw->Write(geom);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetGeometry")

    return ret.Detach();
}

void MgProjectedFeatureReader::Close() 
{
    MG_FEATURE_SERVICE_TRY()
    m_reader->Close();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::Close")
}

bool MgProjectedFeatureReader::ReadNext() 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_reader->ReadNext(); 
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::ReadNext")
    return ret;
}

bool MgProjectedFeatureReader::IsNull(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::IsNull")
    return ret;
}

bool MgProjectedFeatureReader::GetBoolean(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBoolean(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetBoolean")
    return ret;
}

BYTE MgProjectedFeatureReader::GetByte(CREFSTRING propertyName) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetByte(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetByte")
    return ret;
}

MgDateTime* MgProjectedFeatureReader::GetDateTime(CREFSTRING propertyName) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDateTime(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetDateTime")
    return ret.Detach();
}

float MgProjectedFeatureReader::GetSingle(CREFSTRING propertyName) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetSingle(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetSingle")
    return ret;
}

double MgProjectedFeatureReader::GetDouble(CREFSTRING propertyName) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDouble(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetDouble")
    return ret;
}

INT16 MgProjectedFeatureReader::GetInt16(CREFSTRING propertyName) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt16(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt16")
    return ret;
}

INT32 MgProjectedFeatureReader::GetInt32(CREFSTRING propertyName) 
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt32(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt32")
    return ret;
}

INT64 MgProjectedFeatureReader::GetInt64(CREFSTRING propertyName) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt64(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt32")
    return ret;
}

STRING MgProjectedFeatureReader::GetString(CREFSTRING propertyName) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetString(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetString")
    return ret;
}

MgByteReader* MgProjectedFeatureReader::GetBLOB(CREFSTRING propertyName) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetBLOB")
    return ret.Detach();
}

MgByteReader* MgProjectedFeatureReader::GetCLOB(CREFSTRING propertyName)
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetCLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetCLOB")
    return ret.Detach();
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgProjectedFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length) 
{ 
	NOT_IMPLEMENTED(L"MgProjectedFeatureReader::GetGeometry");
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgProjectedFeatureReader::GetGeometry(INT32 index, INT32& length) 
{ 
    NOT_IMPLEMENTED(L"MgProjectedFeatureReader::GetGeometry");
}

MgRaster* MgProjectedFeatureReader::GetRaster(CREFSTRING propertyName) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetRaster(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetRaster")
    return ret.Detach();
}

//MgByteReader* MgProjectedFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
//{
//    return m_reader->GetRaster(rasterPropName, xSize, ySize);
//}

bool MgProjectedFeatureReader::IsNull(INT32 index) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::IsNull")
    return ret;
}

bool MgProjectedFeatureReader::GetBoolean(INT32 index) 
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBoolean(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetBoolean")
    return ret;
}

BYTE MgProjectedFeatureReader::GetByte(INT32 index) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetByte(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetByte")
    return ret;
}

MgDateTime* MgProjectedFeatureReader::GetDateTime(INT32 index) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDateTime(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetDateTime")
    return ret.Detach();
}

float MgProjectedFeatureReader::GetSingle(INT32 index) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetSingle(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetSingle")
    return ret;
}

double MgProjectedFeatureReader::GetDouble(INT32 index) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDouble(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetDouble")
    return ret;
}

INT16 MgProjectedFeatureReader::GetInt16(INT32 index) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt16(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt16")
    return ret;
}

INT32 MgProjectedFeatureReader::GetInt32(INT32 index)
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt32(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt32")
    return ret;
}

INT64 MgProjectedFeatureReader::GetInt64(INT32 index) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt64(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetInt64")
    return ret;
}

STRING MgProjectedFeatureReader::GetString(INT32 index) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetString(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetString")
    return ret;
}

MgByteReader* MgProjectedFeatureReader::GetBLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetBLOB")
    return ret.Detach();
}

MgByteReader* MgProjectedFeatureReader::GetCLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetCLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetCLOB")
    return ret.Detach();
}

MgRaster* MgProjectedFeatureReader::GetRaster(INT32 index) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetRaster(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetRaster")
    return ret.Detach();
}

INT32 MgProjectedFeatureReader::GetReaderType() 
{ 
	return m_reader->GetReaderType();
}

//EXTERNAL_API:

MgByteReader* MgProjectedFeatureReader::ToXml() 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->ToXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::ToXml")
    return ret.Detach();
}

//INTERNAL_API:

const wchar_t* MgProjectedFeatureReader::GetString(CREFSTRING propertyName, INT32& length) 
{
	return m_reader->GetString(propertyName, length);
}

void MgProjectedFeatureReader::Serialize(MgStream* stream) 
{ 
    MG_FEATURE_SERVICE_TRY()
	m_reader->Serialize(stream);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::Serialize")
}

void MgProjectedFeatureReader::Deserialize(MgStream* stream) 
{ 
    MG_FEATURE_SERVICE_TRY()
	m_reader->Deserialize(stream);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::Deserialize")
}

//MgFeatureReader
//PUBLISHED_API:
MgClassDefinition* MgProjectedFeatureReader::GetClassDefinition() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_reader->GetClassDefinition();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetClassDefinition")
    return ret.Detach();
}

//This is internal, but MgMappingUtil needs it. So we have to implement it
MgClassDefinition* MgProjectedFeatureReader::GetClassDefinitionNoXml() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetClassDefinitionNoXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetClassDefinitionNoXml")
    return ret.Detach();
}

MgFeatureReader* MgProjectedFeatureReader::GetFeatureObject(CREFSTRING propertyName) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatureObject(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetFeatureObject")
    return ret.Detach();
}

MgFeatureReader* MgProjectedFeatureReader::GetFeatureObject(INT32 index) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatureObject(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetFeatureObject")
    return ret.Detach();
}

//INTERNAL_API:

MgFeatureSet* MgProjectedFeatureReader::GetFeatures(INT32 count) 
{ 
    Ptr<MgFeatureSet> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatures(count);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgProjectedFeatureReader::GetFeatures")
    return ret.Detach();
}

//FdoIFeatureReader* MgProjectedFeatureReader::GetInternalReader()
//{
//    NOT_IMPLEMENTED(L"MgProjectedFeatureReader::GetInternalReader");
//}

