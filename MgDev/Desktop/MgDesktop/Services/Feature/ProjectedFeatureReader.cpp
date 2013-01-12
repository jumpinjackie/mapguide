#include "MgDesktop.h"

MgdProjectedFeatureReader::MgdProjectedFeatureReader(MgFeatureReader* reader, MgCoordinateSystemTransform* trans)
{
    m_reader = SAFE_ADDREF(reader);
    m_trans = SAFE_ADDREF(trans);
    m_agfRw = new MgAgfReaderWriter();
}

MgdProjectedFeatureReader::~MgdProjectedFeatureReader()
{
    m_reader = NULL;
    m_trans = NULL;
    m_agfRw = NULL;
}

MgByteReader* MgdProjectedFeatureReader::GetGeometry(INT32 index)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_reader)->GetGeometry(index);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_trans);
    ret = m_agfRw->Write(geom);
    
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetGeometry")

    return ret.Detach();
}

MgByteReader* MgdProjectedFeatureReader::GetGeometry(CREFSTRING name)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_reader)->GetGeometry(name);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_trans);
    ret = m_agfRw->Write(geom);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetGeometry")

    return ret.Detach();
}

void MgdProjectedFeatureReader::Close() 
{
    MG_FEATURE_SERVICE_TRY()
    m_reader->Close();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::Close")
}

bool MgdProjectedFeatureReader::ReadNext() 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_reader->ReadNext(); 
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::ReadNext")
    return ret;
}

bool MgdProjectedFeatureReader::IsNull(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::IsNull")
    return ret;
}

bool MgdProjectedFeatureReader::GetBoolean(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBoolean(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetBoolean")
    return ret;
}

BYTE MgdProjectedFeatureReader::GetByte(CREFSTRING propertyName) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetByte(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetByte")
    return ret;
}

MgDateTime* MgdProjectedFeatureReader::GetDateTime(CREFSTRING propertyName) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDateTime(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetDateTime")
    return ret.Detach();
}

float MgdProjectedFeatureReader::GetSingle(CREFSTRING propertyName) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetSingle(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetSingle")
    return ret;
}

double MgdProjectedFeatureReader::GetDouble(CREFSTRING propertyName) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDouble(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetDouble")
    return ret;
}

INT16 MgdProjectedFeatureReader::GetInt16(CREFSTRING propertyName) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt16(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt16")
    return ret;
}

INT32 MgdProjectedFeatureReader::GetInt32(CREFSTRING propertyName) 
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt32(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt32")
    return ret;
}

INT64 MgdProjectedFeatureReader::GetInt64(CREFSTRING propertyName) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt64(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt32")
    return ret;
}

STRING MgdProjectedFeatureReader::GetString(CREFSTRING propertyName) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetString(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetString")
    return ret;
}

MgByteReader* MgdProjectedFeatureReader::GetBLOB(CREFSTRING propertyName) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetBLOB")
    return ret.Detach();
}

MgByteReader* MgdProjectedFeatureReader::GetCLOB(CREFSTRING propertyName)
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetCLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetCLOB")
    return ret.Detach();
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgdProjectedFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length) 
{ 
	NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetGeometry");
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgdProjectedFeatureReader::GetGeometry(INT32 index, INT32& length) 
{ 
    NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetGeometry");
}

MgRaster* MgdProjectedFeatureReader::GetRaster(CREFSTRING propertyName) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetRaster(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetRaster")
    return ret.Detach();
}

//MgByteReader* MgdProjectedFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
//{
//    return m_reader->GetRaster(rasterPropName, xSize, ySize);
//}

bool MgdProjectedFeatureReader::IsNull(INT32 index) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::IsNull")
    return ret;
}

bool MgdProjectedFeatureReader::GetBoolean(INT32 index) 
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBoolean(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetBoolean")
    return ret;
}

BYTE MgdProjectedFeatureReader::GetByte(INT32 index) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetByte(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetByte")
    return ret;
}

MgDateTime* MgdProjectedFeatureReader::GetDateTime(INT32 index) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDateTime(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetDateTime")
    return ret.Detach();
}

float MgdProjectedFeatureReader::GetSingle(INT32 index) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetSingle(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetSingle")
    return ret;
}

double MgdProjectedFeatureReader::GetDouble(INT32 index) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetDouble(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetDouble")
    return ret;
}

INT16 MgdProjectedFeatureReader::GetInt16(INT32 index) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt16(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt16")
    return ret;
}

INT32 MgdProjectedFeatureReader::GetInt32(INT32 index)
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt32(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt32")
    return ret;
}

INT64 MgdProjectedFeatureReader::GetInt64(INT32 index) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetInt64(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetInt64")
    return ret;
}

STRING MgdProjectedFeatureReader::GetString(INT32 index) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetString(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetString")
    return ret;
}

MgByteReader* MgdProjectedFeatureReader::GetBLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetBLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetBLOB")
    return ret.Detach();
}

MgByteReader* MgdProjectedFeatureReader::GetCLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetCLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetCLOB")
    return ret.Detach();
}

MgRaster* MgdProjectedFeatureReader::GetRaster(INT32 index) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetRaster(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetRaster")
    return ret.Detach();
}

INT32 MgdProjectedFeatureReader::GetReaderType() 
{ 
	return m_reader->GetReaderType();
}

//EXTERNAL_API:

MgByteReader* MgdProjectedFeatureReader::ToXml() 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->ToXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::ToXml")
    return ret.Detach();
}

//INTERNAL_API:

string MgdProjectedFeatureReader::GetResponseElementName() { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

string MgdProjectedFeatureReader::GetBodyElementName() { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

void MgdProjectedFeatureReader::ResponseStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

void MgdProjectedFeatureReader::ResponseEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

void MgdProjectedFeatureReader::BodyStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

void MgdProjectedFeatureReader::BodyEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetResponseElementName"); }

void MgdProjectedFeatureReader::CurrentToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::CurrentToStringUtf8"); }

void MgdProjectedFeatureReader::HeaderToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::HeaderToStringUtf8"); }

const wchar_t* MgdProjectedFeatureReader::GetString(CREFSTRING propertyName, INT32& length) 
{
	return m_reader->GetString(propertyName, length);
}

void MgdProjectedFeatureReader::Serialize(MgStream* stream) 
{ 
    MG_FEATURE_SERVICE_TRY()
	m_reader->Serialize(stream);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::Serialize")
}

void MgdProjectedFeatureReader::Deserialize(MgStream* stream) 
{ 
    MG_FEATURE_SERVICE_TRY()
	m_reader->Deserialize(stream);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::Deserialize")
}

//MgFeatureReader
//PUBLISHED_API:
MgClassDefinition* MgdProjectedFeatureReader::GetClassDefinition() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_reader->GetClassDefinition();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetClassDefinition")
    return ret.Detach();
}

//This is internal, but MgdMappingUtil needs it. So we have to implement it
MgClassDefinition* MgdProjectedFeatureReader::GetClassDefinitionNoXml() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetClassDefinitionNoXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetClassDefinitionNoXml")
    return ret.Detach();
}

MgFeatureReader* MgdProjectedFeatureReader::GetFeatureObject(CREFSTRING propertyName) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatureObject(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetFeatureObject")
    return ret.Detach();
}

MgFeatureReader* MgdProjectedFeatureReader::GetFeatureObject(INT32 index) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatureObject(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetFeatureObject")
    return ret.Detach();
}

//INTERNAL_API:

MgFeatureSet* MgdProjectedFeatureReader::GetFeatures(INT32 count) 
{ 
    Ptr<MgFeatureSet> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetFeatures(count);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdProjectedFeatureReader::GetFeatures")
    return ret.Detach();
}

//FdoIFeatureReader* MgdProjectedFeatureReader::GetInternalReader()
//{
//    NOT_IMPLEMENTED(L"MgdProjectedFeatureReader::GetInternalReader");
//}

