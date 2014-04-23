#include "FeatureReader.h"
#include "Services/Feature/FeatureDefs.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/RasterHelper.h"
#include "Services/Feature/FdoConnectionUtil.h"
#include "Fdo.h"

MgdFeatureReader::MgdFeatureReader(MgdFeatureConnection* conn, FdoIFeatureReader* reader, MgStringCollection* forceIdProps)
{
	m_reader = FDO_SAFE_ADDREF(reader);
    m_connection = SAFE_ADDREF(conn);
    m_forceIdProps = SAFE_ADDREF(forceIdProps); //This is only passed in for select queries that involve the FDO Join APIs

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();
}

MgdFeatureReader::~MgdFeatureReader() 
{
	FDO_SAFE_RELEASE(m_reader);
    m_connection = NULL;
} 

bool MgdFeatureReader::ReadNext() 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->ReadNext();
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::ReadNext")
	return ret;
}

bool MgdFeatureReader::IsNull(CREFSTRING propertyName) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	ret = m_reader->IsNull(propertyName.c_str());
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::IsNull")
	return ret;
}

bool MgdFeatureReader::GetBoolean(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetBoolean",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetBoolean")
	return ret;
}

BYTE MgdFeatureReader::GetByte(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetByte",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetByte")
	return ret;
}

MgDateTime* MgdFeatureReader::GetDateTime(CREFSTRING propertyName) 
{ 
	Ptr<MgDateTime> ret;
	MG_FEATURE_SERVICE_TRY()
	try
    {
        FdoDateTime val = m_reader->GetDateTime(propertyName.c_str());
        ret = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetDateTime",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetDateTime")
	return ret.Detach();
}

float MgdFeatureReader::GetSingle(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetSingle",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetSingle")
	return ret;
}

double MgdFeatureReader::GetDouble(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetDouble",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetDouble")
	return ret;
}

INT16 MgdFeatureReader::GetInt16(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetInt16",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt16")
	return ret;
}

INT32 MgdFeatureReader::GetInt32(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetInt32",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt32")
	return ret;
}

INT64 MgdFeatureReader::GetInt64(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetInt64",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt64")
	return ret;
}

STRING MgdFeatureReader::GetString(CREFSTRING propertyName) 
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetString",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetString")
	return ret;
}

MgByteReader* MgdFeatureReader::GetBLOB(CREFSTRING propertyName) 
{ 
	Ptr<MgByteReader> byteReader;
	MG_FEATURE_SERVICE_TRY()
    try
    {
        FdoPtr<FdoLOBValue> fdoVal = m_reader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetBLOB",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetBLOB")
	return byteReader.Detach();
}

MgByteReader* MgdFeatureReader::GetCLOB(CREFSTRING propertyName)
{ 
	Ptr<MgByteReader> byteReader;
	MG_FEATURE_SERVICE_TRY()
    try
    {
        FdoPtr<FdoLOBValue> fdoVal = m_reader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
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

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetCLOB",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetCLOB")
	return byteReader.Detach();
}

MgByteReader* MgdFeatureReader::GetGeometry(CREFSTRING propertyName) 
{ 
	Ptr<MgByteReader> retVal;
	MG_FEATURE_SERVICE_TRY()
    try
    {
        FdoInt32 len = 0;
        const FdoByte* data = m_reader->GetGeometry(propertyName.c_str(), &len);

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            retVal = bSource->GetReader();
        }
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetGeometry")
	return retVal.Detach();
}

//Didn't want to implement, but needed by the RS_FeatureReader
BYTE_ARRAY_OUT MgdFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length) 
{ 
	CHECKNULL(m_reader, L"MgdFeatureReader::GetGeometry");

    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    try
    {
        FdoInt32 len = 0;
        data = m_reader->GetGeometry(propertyName.c_str(), &length);
        length = len;
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetGeometry");

    return (BYTE_ARRAY_OUT)data;
}

//Didn't want to implement, but needed by the RS_FeatureReader
BYTE_ARRAY_OUT MgdFeatureReader::GetGeometry(INT32 index, INT32& length) 
{ 
	BYTE_ARRAY_OUT ret = NULL;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetGeometry(propertyName, length);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetGeometry")
	return ret;
}

MgRaster* MgdFeatureReader::GetRaster(CREFSTRING propertyName) 
{ 
	Ptr<MgRaster> ret;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIRaster> raster = m_reader->GetRaster(propertyName.c_str());
    CHECKNULL((FdoIRaster*)raster, L"MgdFeatureReader::GetRaster");

    ret = MgdFeatureUtil::GetMgRaster(raster, propertyName);
    CHECKNULL((MgRaster*)ret, L"MgdFeatureReader::GetRaster");

    //This is a clunky way to do what is effectively calling the overloaded GetRaster(propName, xSize, ySize)
    //method, but MgRaster demands this
    Ptr<MgFeatureService> rasterHelp = new MgdRasterHelper(this);
    ret->SetMgService(rasterHelp);
    //MgRaster demands a handle
    STRING handle;
    MgUtil::GenerateUuid(handle);
    ret->SetHandle(handle);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetRaster")

	return ret.Detach();
}

MgByteReader* MgdFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // If there is no raster property, GetRaster should not be called
    if (!m_classDef->HasRasterProperty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgdFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgdFeatureReader::GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // There can be more than one Raster property
    if (rasterPropName.empty())
    {
        rasterPropName = m_classDef->GetRasterPropertyName();
    }

    // If this property is requested then we fetch the raster data
    byteReader = MgdFeatureUtil::GetRaster(m_reader, rasterPropName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetRaster")

    return byteReader.Detach();
}

bool MgdFeatureReader::IsNull(INT32 index) 
{ 
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = IsNull(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::IsNull")
	return ret;
}

bool MgdFeatureReader::GetBoolean(INT32 index) 
{
	bool ret = false;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBoolean(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetBoolean")
	return ret;
}

BYTE MgdFeatureReader::GetByte(INT32 index) 
{ 
	BYTE ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetByte(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetByte")
	return ret;
}

MgDateTime* MgdFeatureReader::GetDateTime(INT32 index) 
{ 
	Ptr<MgDateTime> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDateTime(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetDateTime")
	return ret.Detach();
}

float MgdFeatureReader::GetSingle(INT32 index) 
{ 
	float ret = 0.0f;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetSingle(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetSingle")
	return ret;
}

double MgdFeatureReader::GetDouble(INT32 index) 
{ 
	double ret = 0.0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetDouble(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetDouble")
	return ret;
}

INT16 MgdFeatureReader::GetInt16(INT32 index) 
{ 
	INT16 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt16(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt16")
	return ret;
}

INT32 MgdFeatureReader::GetInt32(INT32 index)
{ 
	INT32 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt32(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt32")
	return ret;
}

INT64 MgdFeatureReader::GetInt64(INT32 index) 
{ 
	INT64 ret = 0;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetInt64(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetInt64")
	return ret;
}

STRING MgdFeatureReader::GetString(INT32 index) 
{ 
	STRING ret = L"";
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetString(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetString")
	return ret;
}

MgByteReader* MgdFeatureReader::GetBLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetBLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetBLOB")
	return ret.Detach();
}

MgByteReader* MgdFeatureReader::GetCLOB(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetCLOB(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetCLOB")
	return ret.Detach();
}

MgByteReader* MgdFeatureReader::GetGeometry(INT32 index) 
{ 
	Ptr<MgByteReader> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetGeometry(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetGeometry")
	return ret.Detach();
}

MgRaster* MgdFeatureReader::GetRaster(INT32 index) 
{ 
	Ptr<MgRaster> ret;
	MG_FEATURE_SERVICE_TRY()
	STRING propertyName = GetPropertyName(index);
    ret = GetRaster(propertyName);
	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetRaster")
	return ret.Detach();
}

void MgdFeatureReader::Close() 
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::Close");
}

INT32 MgdFeatureReader::GetReaderType() 
{ 
	return MgReaderType::FeatureReader;
}

//EXTERNAL_API:

MgByteReader* MgdFeatureReader::ToXml() 
{ 
	throw new MgNotImplementedException(L"MgdFeatureReader::ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}

//INTERNAL_API:

string MgdFeatureReader::GetResponseElementName() { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

string MgdFeatureReader::GetBodyElementName() { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::ResponseStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::ResponseEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::BodyStartUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::BodyEndUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::HeaderToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

void MgdFeatureReader::CurrentToStringUtf8(string& str) { NOT_IMPLEMENTED(L"MgdFeatureReader::GetResponseElementName"); }

const wchar_t* MgdFeatureReader::GetString(CREFSTRING propertyName, INT32& length) 
{ 
	CHECKNULL(m_reader, L"MgdFeatureReader::GetString");

    FdoString* retVal = NULL;

    MG_FEATURE_SERVICE_TRY()

    if(m_reader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdFeatureReader::GetString",
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetString");

    return ((const wchar_t*)retVal);
}

void MgdFeatureReader::Serialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdFeatureReader::Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgdFeatureReader::Deserialize(MgStream* stream) 
{ 
	throw new MgNotImplementedException(L"MgdFeatureReader::Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgFeatureReader
//PUBLISHED_API:
MgClassDefinition* MgdFeatureReader::GetClassDefinition() 
{ 
    CHECKNULL(m_reader, L"MgdFeatureReader::GetClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    // Check to see if the class definition cached contains the XML information
    bool bGetClassDefinition = true;
    if (NULL != (MgClassDefinition*)m_classDef)
    {
        if(m_classDef->HasSerializedXml())
        {
            // We have the full object so don't update it
            bGetClassDefinition = false;
        }
    }

    if (bGetClassDefinition)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_reader->GetClassDefinition();

        // Convert FdoClassDefinition to MgClassDefinition
        m_classDef = MgdFeatureUtil::GetMgClassDefinition(fdoClassDefinition, true);
        CHECKNULL(m_classDef.p, L"MgdFeatureReader::GetClassDefinition");

        // The class definition presented by a FDO reader from a join select query will probably
        // not contain any identity properties, thereby violating our golden rule of selection (no id props = unselectable)
        // so for such queries executed by MapGuide, we pass this list of identity properties on to this reader (sourced
        // with the identity properties from the "primary" class definition) so that we can "rewrite" the converted MgClassDefinition
        if (NULL != m_forceIdProps && m_forceIdProps->GetCount() > 0)
        {
            Ptr<MgPropertyDefinitionCollection> clsProps = m_classDef->GetProperties();
            Ptr<MgPropertyDefinitionCollection> clsIdProps = m_classDef->GetIdentityProperties();

            // This is most likely empty, but we're overwriting this anyway, so clear it
            clsIdProps->Clear();

            // Copy across any properties from the explicity identity property name list
            for (INT32 i = 0; i < m_forceIdProps->GetCount(); i++)
            {
                STRING propName = m_forceIdProps->GetItem(i);
                INT32 pidx = clsProps->IndexOf(propName);
                if (pidx >= 0)
                {
                    Ptr<MgPropertyDefinition> p = clsProps->GetItem(pidx);
                    clsIdProps->Add(p);
                } //else should we care about correctness?
            }
        }

#ifdef DEBUG_FDO_JOIN
        Ptr<MgPropertyDefinitionCollection> idProps = m_classDef->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> props = m_classDef->GetProperties();
        INT32 idCount = idProps->GetCount();
        INT32 propCount = props->GetCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) [MgFeatureReader::GetClassDefinition] %d props, %d identity props"), propCount, idCount));
#endif
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}

//This is internal, but MgdMappingUtil needs it. So we have to implement it
MgClassDefinition* MgdFeatureReader::GetClassDefinitionNoXml() 
{ 
	CHECKNULL(m_reader, L"MgdFeatureReader::GetClassDefinitionNoXml");

    Ptr<MgClassDefinition> classDef;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_reader->GetClassDefinition();

        // Convert FdoClassDefinition to MgClassDefinition
        m_classDef = MgdFeatureUtil::GetMgClassDefinition(fdoClassDefinition, false);
        CHECKNULL(m_classDef.p, L"MgdFeatureReader::GetClassDefinitionNoXml");

        // The class definition presented by a FDO reader from a join select query will probably
        // not contain any identity properties, thereby violating our golden rule of selection (no id props = unselectable)
        // so for such queries executed by MapGuide, we pass this list of identity properties on to this reader (sourced
        // with the identity properties from the "primary" class definition) so that we can "rewrite" the converted MgClassDefinition
        if (NULL != m_forceIdProps && m_forceIdProps->GetCount() > 0)
        {
            Ptr<MgPropertyDefinitionCollection> clsProps = m_classDef->GetProperties();
            Ptr<MgPropertyDefinitionCollection> clsIdProps = m_classDef->GetIdentityProperties();

            // This is most likely empty, but we're overwriting this anyway, so clear it
            clsIdProps->Clear();

            // Copy across any properties from the explicity identity property name list
            for (INT32 i = 0; i < m_forceIdProps->GetCount(); i++)
            {
                STRING propName = m_forceIdProps->GetItem(i);
                INT32 pidx = clsProps->IndexOf(propName);
                if (pidx >= 0)
                {
                    Ptr<MgPropertyDefinition> p = clsProps->GetItem(pidx);
                    clsIdProps->Add(p);
                } //else should we care about correctness?
            }
        }

#ifdef DEBUG_FDO_JOIN
        Ptr<MgPropertyDefinitionCollection> idProps = m_classDef->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> props = m_classDef->GetProperties();
        INT32 idCount = idProps->GetCount();
        INT32 propCount = props->GetCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) [MgFeatureReader::GetClassDefinition] %d props, %d identity props"), propCount, idCount));
#endif
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetClassDefinitionNoXml")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}

MgFeatureReader* MgdFeatureReader::GetFeatureObject(CREFSTRING propertyName) 
{ 
	Ptr<MgFeatureReader> reader;

	MG_FEATURE_SERVICE_TRY()

    try
    {
        FdoPtr<FdoIFeatureReader> featureObjectReader = m_reader->GetFeatureObject(propertyName.c_str());

        if (featureObjectReader != NULL)
        {
            // Create a feature reader identifier
            reader = new MgdFeatureReader(m_connection, featureObjectReader);
        }
    }
    catch(...)
    {
        if(m_reader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgdFeatureReader::GetFeatureObject",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetFeatureObject")

	return reader.Detach();
}

MgFeatureReader* MgdFeatureReader::GetFeatureObject(INT32 index) 
{ 
	Ptr<MgFeatureReader> ret;

	MG_FEATURE_SERVICE_TRY()	

	STRING propertyName = GetPropertyName(index);
    ret = GetFeatureObject(propertyName);

	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureReader::GetFeatureObject")

	return ret.Detach();
}

//INTERNAL_API:

MgFeatureSet* MgdFeatureReader::GetFeatures(INT32 count) 
{ 
	throw new MgNotImplementedException(L"MgdFeatureReader::GetFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoIFeatureReader* MgdFeatureReader::GetInternalReader()
{
    return FDO_SAFE_ADDREF(m_reader);
}