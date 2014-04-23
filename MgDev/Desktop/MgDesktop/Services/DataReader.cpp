#include "DataReader.h"
#include "Services/Feature/FeatureDefs.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/RasterHelper.h"
#include "Services/Feature/FdoConnectionUtil.h"
#include "Fdo.h"

MgdDataReader::MgdDataReader(MgdFeatureConnection* conn, FdoIDataReader* reader)
{
	m_reader = FDO_SAFE_ADDREF(reader);
    m_connection = SAFE_ADDREF(conn);

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();
}

MgdDataReader::~MgdDataReader() 
{
	FDO_SAFE_RELEASE(m_reader);
    m_connection = NULL;
} 

bool MgdDataReader::ReadNext() 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->ReadNext(); 
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::ReadNext")
	return ret;
}

INT32 MgdDataReader::GetPropertyCount() 
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetPropertyCount(); 
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetPropertyCount")
	return ret;
}

STRING MgdDataReader::GetPropertyName(INT32 index) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	FdoString* name = m_reader->GetPropertyName(index);
	ret = STRING(name);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetPropertyName")
	return ret;
}

INT32 MgdDataReader::GetPropertyType(CREFSTRING propertyName) 
{ 
	INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoPropertyType propType = m_reader->GetPropertyType(propertyName.c_str());

    switch (propType)
    {
        // If geometric property, return geometric data type.
        case FdoPropertyType_GeometricProperty:
        {
            type = MgPropertyType::Geometry;
            break;
        }
        case FdoPropertyType_DataProperty:
        {
            FdoDataType dataType = m_reader->GetDataType(propertyName.c_str());
            if (FdoDataType(-1) == dataType)
            {
                throw new MgInvalidPropertyTypeException(L"MgdDataReader.GetPropertyType",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            type = MgdFeatureUtil::GetMgPropertyType(dataType);
            break;
        }
        case FdoPropertyType_RasterProperty:
        {
            type = MgPropertyType::Raster;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgdDataReader.GetPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader.GetPropertyType")

    return type;
}

INT32 MgdDataReader::GetPropertyType(INT32 index) 
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetPropertyType(index);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetPropertyType")
	return ret;
}

bool MgdDataReader::IsNull(CREFSTRING propertyName) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(propertyName.c_str());
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::IsNull")
	return ret;
}

bool MgdDataReader::GetBoolean(CREFSTRING propertyName) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = m_reader->GetBoolean(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetBoolean",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetBoolean")
	return ret;
}

BYTE MgdDataReader::GetByte(CREFSTRING propertyName) 
{ 
	BYTE ret = 0;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = (BYTE)m_reader->GetByte(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetByte",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetByte")
	return ret;
}

MgDateTime* MgdDataReader::GetDateTime(CREFSTRING propertyName) 
{ 
	Ptr<MgDateTime> mdt;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        FdoDateTime val = m_reader->GetDateTime(propertyName.c_str());
        mdt = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                            (INT8)val.hour, (INT8)val.minute, val.seconds);
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetDateTime",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetByte")
	return mdt.Detach();
}

float MgdDataReader::GetSingle(CREFSTRING propertyName) 
{ 
	float ret = 0.0f;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = m_reader->GetSingle(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetSingle",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetSingle")
	return ret;
}

double MgdDataReader::GetDouble(CREFSTRING propertyName) 
{ 
	double ret = 0.0;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = m_reader->GetDouble(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetDouble",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetDouble")
	return ret;
}

INT16 MgdDataReader::GetInt16(CREFSTRING propertyName) 
{ 
	INT16 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = (INT16)m_reader->GetInt16(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetInt16",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt16")
	return ret;
}

INT32 MgdDataReader::GetInt32(CREFSTRING propertyName) 
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = (INT32)m_reader->GetInt32(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetInt32",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt32")
	return ret;
}

INT64 MgdDataReader::GetInt64(CREFSTRING propertyName) 
{ 
	INT64 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        ret = (INT64)m_reader->GetInt64(propertyName.c_str());
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetInt64",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt64")
	return ret;
}

STRING MgdDataReader::GetString(CREFSTRING propertyName) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	try
    {
        INT32 length = 0;
        const wchar_t* str = this->GetString(propertyName.c_str(), length);
        if (str != NULL)
        {
            ret = str;
        }
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetString",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetString")
	return ret;
}

MgByteReader* MgdDataReader::GetBLOB(CREFSTRING propertyName) 
{ 
	Ptr<MgByteReader> blob;
	MG_FEATURE_SERVICE_TRY()
    FdoPtr<FdoLOBValue> fdoVal = m_reader->GetLOB(propertyName.c_str());
    if (fdoVal != NULL)
    {
        FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
        if (byteArray != NULL)
        {
            FdoByte* bytes = byteArray->GetData();
            FdoInt32 len = byteArray->GetCount();
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
            blob = byteSource->GetReader();
        }
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetBLOB")
	return blob.Detach();
}

MgByteReader* MgdDataReader::GetCLOB(CREFSTRING propertyName)
{ 
	Ptr<MgByteReader> clob;
	MG_FEATURE_SERVICE_TRY()
    FdoPtr<FdoLOBValue> fdoVal = m_reader->GetLOB(propertyName.c_str());
    if (fdoVal != NULL)
    {
        FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
        if (byteArray != NULL)
        {
            FdoByte* bytes = byteArray->GetData();
            FdoInt32 len = byteArray->GetCount();
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
            clob = byteSource->GetReader();
        }
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetCLOB")
	return clob.Detach();
}

MgByteReader* MgdDataReader::GetGeometry(CREFSTRING propertyName) 
{ 
	Ptr<MgByteReader> geom;
	MG_FEATURE_SERVICE_TRY()
    try
    {
        FdoPtr<FdoByteArray> byteArray = m_reader->GetGeometry(propertyName.c_str());
        INT32 len = (INT32)byteArray->GetCount();
        const FdoByte* data = byteArray->GetData();

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            geom = bSource->GetReader();
        }
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdDataReader::GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetGeometry")
	return geom.Detach();
}

MgRaster* MgdDataReader::GetRaster(CREFSTRING propertyName) 
{ 
    Ptr<MgRaster> retVal;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIRaster> raster = m_reader->GetRaster(propertyName.c_str());
    CHECKNULL((FdoIRaster*)raster, L"MgdDataReader::GetRaster");

    retVal = MgdFeatureUtil::GetMgRaster(raster, propertyName);
    CHECKNULL((MgRaster*)retVal, L"MgdDataReader::GetRaster");

    //This is a clunky way to do what is effectively calling the overloaded GetRaster(propName, xSize, ySize)
    //method, but MgRaster demands this
    Ptr<MgFeatureService> rasterHelp = new MgdRasterHelper(this);
    retVal->SetMgService(rasterHelp);
    //MgRaster demands a handle
    STRING handle;
    MgUtil::GenerateUuid(handle);
    retVal->SetHandle(handle);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetRaster")

	return retVal.Detach();
}

MgByteReader* MgdDataReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
{
    CHECKNULL(m_reader, L"MgdDataReader::GetRaster");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if (rasterPropName.empty())
        rasterPropName = GetRasterPropertyName();

    // If there is no raster property, GetRaster should not be called
    if (rasterPropName.empty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgdFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgdDataReader::GetRaster",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    byteReader = MgdFeatureUtil::GetRaster(m_reader, rasterPropName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetRaster")

    return byteReader.Detach();
}

STRING MgdDataReader::GetRasterPropertyName()
{
    STRING name = L"";

    INT32 cnt = GetPropertyCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        name = GetPropertyName(i);

        INT32 type = GetPropertyType(name);
        if (type == MgPropertyType::Raster)
        {
            break;
        }
    }

    return name;
}

INT32 MgdDataReader::GetPropertyIndex(CREFSTRING propertyName) 
{ 
	INT32 ret = -1;
	MG_FEATURE_SERVICE_TRY()
    ret = m_reader->GetPropertyIndex(propertyName.c_str());
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetPropertyIndex")
	return ret;
}

bool MgdDataReader::IsNull(INT32 index) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = IsNull(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::IsNull")
	return ret;
}

bool MgdDataReader::GetBoolean(INT32 index) 
{
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBoolean(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetBoolean")
	return ret;
}

BYTE MgdDataReader::GetByte(INT32 index) 
{ 
	BYTE ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetByte(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetByte")
	return ret;
}

MgDateTime* MgdDataReader::GetDateTime(INT32 index) 
{ 
	Ptr<MgDateTime> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDateTime(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetDateTime")
	return ret;
}

float MgdDataReader::GetSingle(INT32 index) 
{
	float ret = 0.0f;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetSingle(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetSingle")
	return ret;
}

double MgdDataReader::GetDouble(INT32 index) 
{ 
	double ret = 0.0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDouble(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetDouble")
	return ret;
}

INT16 MgdDataReader::GetInt16(INT32 index) 
{ 
	INT16 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt16(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt16")
	return ret;
}

INT32 MgdDataReader::GetInt32(INT32 index)
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt32(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt32")
	return ret;
}

INT64 MgdDataReader::GetInt64(INT32 index) 
{ 
	INT64 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt64(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetInt64")
	return ret;
}

STRING MgdDataReader::GetString(INT32 index) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetString(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetString")
	return ret;
}

MgByteReader* MgdDataReader::GetBLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetBLOB")
	return ret;
}

MgByteReader* MgdDataReader::GetCLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetCLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetCLOB")
	return ret;
}

MgByteReader* MgdDataReader::GetGeometry(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetGeometry(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetGeometry")
	return ret;
}

MgRaster* MgdDataReader::GetRaster(INT32 index) 
{ 
	Ptr<MgRaster> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetRaster(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetRaster")
	return ret;
}

void MgdDataReader::Close() 
{ 
    MG_FEATURE_SERVICE_TRY()

    m_reader->Close(); 

    // Get the FDO connection
    FdoPtr<FdoIConnection> fdoConnection = m_connection->GetConnection();

    // Release the connection.
    //m_connection = NULL;
    MgdFdoConnectionPool::ReturnConnection(m_connection);
    m_connection = NULL;
    //MgdFdoConnectionUtil::CloseConnection(fdoConnection);

	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::Close");
}

INT32 MgdDataReader::GetReaderType() 
{ 
	return MgReaderType::FeatureReader;
}

//EXTERNAL_API:

MgByteReader* MgdDataReader::ToXml() 
{ 
	throw new MgNotImplementedException(L"MgdDataReader::ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}

//INTERNAL_API:

string MgdDataReader::GetResponseElementName() { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

string MgdDataReader::GetBodyElementName() { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

void MgdDataReader::ResponseStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

void MgdDataReader::ResponseEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

void MgdDataReader::BodyStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

void MgdDataReader::BodyEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::GetResponseElementName"); }

void MgdDataReader::CurrentToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::CurrentToStringUtf8"); }

void MgdDataReader::HeaderToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdDataReader::HeaderToStringUtf8"); }

const wchar_t* MgdDataReader::GetString(CREFSTRING propertyName, INT32& length) 
{ 
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_reader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdDataReader::GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_reader->GetString(propertyName.c_str());
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader::GetString");

    return ((const wchar_t*)retVal);

}

void MgdDataReader::Serialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdDataReader::Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgdDataReader::Deserialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdDataReader::Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}