#include "SqlReader.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FdoConnectionUtil.h"
#include "Fdo.h"

MgdSqlDataReader::MgdSqlDataReader(MgdFeatureConnection* conn, FdoISQLDataReader* reader)
{
	m_reader = FDO_SAFE_ADDREF(reader);
    m_connection = SAFE_ADDREF(conn);

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();
}

MgdSqlDataReader::~MgdSqlDataReader() 
{
	FDO_SAFE_RELEASE(m_reader);
    m_connection = NULL;
} 


bool MgdSqlDataReader::ReadNext() 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->ReadNext(); 
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::ReadNext")
	return ret;
}

INT32 MgdSqlDataReader::GetPropertyCount() 
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetColumnCount(); 
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetPropertyCount")
	return ret;
}

STRING MgdSqlDataReader::GetPropertyName(INT32 index) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	FdoString* name = m_reader->GetColumnName(index);
	ret = STRING(name);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetPropertyName")
	return ret;
}

INT32 MgdSqlDataReader::GetPropertyType(CREFSTRING propertyName) 
{ 
	INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoDataType dataType = m_reader->GetColumnType(propertyName.c_str());
    if (FdoDataType(-1) == dataType)
    {
        throw new MgInvalidPropertyTypeException(L"MgdDataReader.GetPropertyType",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    type = MgdFeatureUtil::GetMgPropertyType(dataType);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdDataReader.GetPropertyType")

    return type;
}

INT32 MgdSqlDataReader::GetPropertyType(INT32 index) 
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->GetPropertyType(index);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetPropertyType")
	return ret;
}

bool MgdSqlDataReader::IsNull(CREFSTRING propertyName) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(propertyName.c_str());
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::IsNull")
	return ret;
}

bool MgdSqlDataReader::GetBoolean(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetBoolean",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetBoolean")
	return ret;
}

BYTE MgdSqlDataReader::GetByte(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetByte",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetByte")
	return ret;
}

MgDateTime* MgdSqlDataReader::GetDateTime(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetDateTime",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetByte")
	return mdt.Detach();
}

float MgdSqlDataReader::GetSingle(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetSingle",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetSingle")
	return ret;
}

double MgdSqlDataReader::GetDouble(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetDouble",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetDouble")
	return ret;
}

INT16 MgdSqlDataReader::GetInt16(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetInt16",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt16")
	return ret;
}

INT32 MgdSqlDataReader::GetInt32(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetInt32",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt32")
	return ret;
}

INT64 MgdSqlDataReader::GetInt64(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetInt64",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt64")
	return ret;
}

STRING MgdSqlDataReader::GetString(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetString",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetString")
	return ret;
}

MgByteReader* MgdSqlDataReader::GetBLOB(CREFSTRING propertyName) 
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
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetBLOB")
	return blob.Detach();
}

MgByteReader* MgdSqlDataReader::GetCLOB(CREFSTRING propertyName)
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
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetCLOB")
	return clob.Detach();
}

MgByteReader* MgdSqlDataReader::GetGeometry(CREFSTRING propertyName) 
{ 
	Ptr<MgByteReader> geom;
	MG_FEATURE_SERVICE_TRY()
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
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetGeometry")
	return geom.Detach();
}

INT32 MgdSqlDataReader::GetPropertyIndex(CREFSTRING propertyName) 
{ 
	INT32 ret = -1;
	MG_FEATURE_SERVICE_TRY()
    ret = m_reader->GetColumnIndex(propertyName.c_str());
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetPropertyIndex")
	return ret;
}

bool MgdSqlDataReader::IsNull(INT32 index) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = IsNull(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::IsNull")
	return ret;
}

bool MgdSqlDataReader::GetBoolean(INT32 index) 
{
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBoolean(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetBoolean")
	return ret;
}

BYTE MgdSqlDataReader::GetByte(INT32 index) 
{ 
	BYTE ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetByte(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetByte")
	return ret;
}

MgDateTime* MgdSqlDataReader::GetDateTime(INT32 index) 
{ 
	Ptr<MgDateTime> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDateTime(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetDateTime")
	return ret;
}

float MgdSqlDataReader::GetSingle(INT32 index) 
{
	float ret = 0.0f;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetSingle(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetSingle")
	return ret;
}

double MgdSqlDataReader::GetDouble(INT32 index) 
{ 
	double ret = 0.0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDouble(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetDouble")
	return ret;
}

INT16 MgdSqlDataReader::GetInt16(INT32 index) 
{ 
	INT16 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt16(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt16")
	return ret;
}

INT32 MgdSqlDataReader::GetInt32(INT32 index)
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt32(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt32")
	return ret;
}

INT64 MgdSqlDataReader::GetInt64(INT32 index) 
{ 
	INT64 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt64(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetInt64")
	return ret;
}

STRING MgdSqlDataReader::GetString(INT32 index) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetString(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetString")
	return ret;
}

MgByteReader* MgdSqlDataReader::GetBLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetBLOB")
	return ret;
}

MgByteReader* MgdSqlDataReader::GetCLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetCLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetCLOB")
	return ret;
}

MgByteReader* MgdSqlDataReader::GetGeometry(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetGeometry(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetGeometry")
	return ret;
}

MgRaster* MgdSqlDataReader::GetRaster(INT32 index) 
{ 
	throw new MgInvalidPropertyTypeException(L"MgdSqlDataReader::GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

MgRaster* MgdSqlDataReader::GetRaster(CREFSTRING propertyName) 
{ 
    throw new MgInvalidPropertyTypeException(L"MgdSqlDataReader::GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgdSqlDataReader::Close() 
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

INT32 MgdSqlDataReader::GetReaderType() 
{ 
	return MgReaderType::FeatureReader;
}

//EXTERNAL_API:

MgByteReader* MgdSqlDataReader::ToXml() 
{ 
	throw new MgNotImplementedException(L"MgdSqlDataReader::ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}

//INTERNAL_API:

string MgdSqlDataReader::GetResponseElementName() { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

string MgdSqlDataReader::GetBodyElementName() { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

void MgdSqlDataReader::ResponseStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

void MgdSqlDataReader::ResponseEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

void MgdSqlDataReader::BodyStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

void MgdSqlDataReader::BodyEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::GetResponseElementName"); }

void MgdSqlDataReader::CurrentToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::CurrentToStringUtf8"); }

void MgdSqlDataReader::HeaderToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdSqlDataReader::HeaderToStringUtf8"); }

MgByteReader* MgdSqlDataReader::GetLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    try
    {
        // TODO: We need to switch to FdoIStreamReader when we have streaming capability in MgByteReader
        FdoPtr<FdoLOBValue> fdoVal = m_reader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                // TODO: We need to differentiate between CLOB and BLOB
                // TODO: How do we fine the MimeType of data for CLOB
                byteSource->SetMimeType(MgMimeType::Binary);
                byteReader = byteSource->GetReader();
            }
        }
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetLOB",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
    return byteReader.Detach();
}

const wchar_t* MgdSqlDataReader::GetString(CREFSTRING propertyName, INT32& length) 
{ 
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_reader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdSqlDataReader::GetString",
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSqlDataReader::GetString");

    return ((const wchar_t*)retVal);
}

void MgdSqlDataReader::Serialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdSqlDataReader::Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgdSqlDataReader::Deserialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdSqlDataReader::Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}