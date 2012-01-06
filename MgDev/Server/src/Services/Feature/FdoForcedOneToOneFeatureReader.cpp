#include "FdoForcedOneToOneFeatureReader.h"

// constructs a MgFdoForcedOneToOneFeatureReader
MgFdoForcedOneToOneFeatureReader::MgFdoForcedOneToOneFeatureReader() { }

MgFdoForcedOneToOneFeatureReader::MgFdoForcedOneToOneFeatureReader(FdoIFeatureReader* reader, FdoStringCollection* idPropNames)
{
    m_cachedClsDef = NULL;
    m_reader = FDO_SAFE_ADDREF(reader);
    m_idPropNames = FDO_SAFE_ADDREF(idPropNames);
}

// default destructor
MgFdoForcedOneToOneFeatureReader::~MgFdoForcedOneToOneFeatureReader() 
{ 
    m_idValues.clear();
    FDO_SAFE_RELEASE(m_reader);
    FDO_SAFE_RELEASE(m_idPropNames);
    FDO_SAFE_RELEASE(m_cachedClsDef);
}

void MgFdoForcedOneToOneFeatureReader::Dispose() { delete this; }

FdoClassDefinition* MgFdoForcedOneToOneFeatureReader::GetClassDefinition()
{
    return m_reader->GetClassDefinition();
}

int MgFdoForcedOneToOneFeatureReader::GetDepth()
{
    return m_reader->GetDepth();
}

bool MgFdoForcedOneToOneFeatureReader::GetBoolean(const wchar_t *propertyName)
{
    return m_reader->GetBoolean(propertyName);
}

FdoByte MgFdoForcedOneToOneFeatureReader::GetByte(const wchar_t *propertyName)
{
    return m_reader->GetByte(propertyName);
}

double MgFdoForcedOneToOneFeatureReader::GetDouble(const wchar_t* propertyName)
{
    return m_reader->GetDouble(propertyName);
}

short MgFdoForcedOneToOneFeatureReader::GetInt16(const wchar_t *propertyName)
{
    return m_reader->GetInt16(propertyName);
}

int MgFdoForcedOneToOneFeatureReader::GetInt32(const wchar_t *propertyName)
{
    return m_reader->GetInt32(propertyName);
}

FdoInt64 MgFdoForcedOneToOneFeatureReader::GetInt64(const wchar_t *propertyName)
{
    return m_reader->GetInt64(propertyName);
}

float MgFdoForcedOneToOneFeatureReader::GetSingle(const wchar_t *propertyName)
{
    return m_reader->GetSingle(propertyName);
}

const wchar_t* MgFdoForcedOneToOneFeatureReader::GetString(const wchar_t *propertyName)
{
    return m_reader->GetString(propertyName);
}

FdoLOBValue* MgFdoForcedOneToOneFeatureReader::GetLOB(const wchar_t* propertyName)
{
    return m_reader->GetLOB(propertyName);
}

FdoIStreamReader* MgFdoForcedOneToOneFeatureReader::GetLOBStreamReader(const wchar_t* propertyName)
{
    return m_reader->GetLOBStreamReader(propertyName);
}

bool MgFdoForcedOneToOneFeatureReader::IsNull(const wchar_t *propertyName)
{
    return m_reader->IsNull(propertyName);
}

FdoIFeatureReader* MgFdoForcedOneToOneFeatureReader::GetFeatureObject(const wchar_t* propertyName)
{
    return m_reader->GetFeatureObject(propertyName);
}

FdoByteArray* MgFdoForcedOneToOneFeatureReader::GetGeometry(const wchar_t* propertyName)
{
    return m_reader->GetGeometry(propertyName);
}

const FdoByte * MgFdoForcedOneToOneFeatureReader::GetGeometry(const wchar_t* propertyName, FdoInt32 * count)
{
    return m_reader->GetGeometry(propertyName, count);
}

FdoIRaster* MgFdoForcedOneToOneFeatureReader::GetRaster(const wchar_t* propertyName)
{
    return m_reader->GetRaster(propertyName);
}

bool MgFdoForcedOneToOneFeatureReader::IsNull(FdoInt32 index)
{
    return m_reader->IsNull(index);
}

const wchar_t* MgFdoForcedOneToOneFeatureReader::GetString(FdoInt32 index)
{
    return m_reader->GetString(index);
}

bool MgFdoForcedOneToOneFeatureReader::GetBoolean(FdoInt32 index)
{
    return m_reader->GetBoolean(index);
}

FdoByte MgFdoForcedOneToOneFeatureReader::GetByte(FdoInt32 index)
{
    return m_reader->GetByte(index);
}

FdoDateTime MgFdoForcedOneToOneFeatureReader::GetDateTime(FdoInt32 index)
{
    return m_reader->GetDateTime(index);
}

double MgFdoForcedOneToOneFeatureReader::GetDouble(FdoInt32 index)
{
    return m_reader->GetDouble(index);
}

short MgFdoForcedOneToOneFeatureReader::GetInt16(FdoInt32 index)
{
    return m_reader->GetInt16(index);
}

int MgFdoForcedOneToOneFeatureReader::GetInt32(FdoInt32 index)
{
    return m_reader->GetInt32(index);
}

FdoInt64 MgFdoForcedOneToOneFeatureReader::GetInt64(FdoInt32 index)
{
    return m_reader->GetInt64(index);
}

float MgFdoForcedOneToOneFeatureReader::GetSingle(FdoInt32 index)
{
    return m_reader->GetSingle(index);
}

FdoLOBValue* MgFdoForcedOneToOneFeatureReader::GetLOB(FdoInt32 index)
{
    return m_reader->GetLOB(index);
}

FdoIStreamReader* MgFdoForcedOneToOneFeatureReader::GetLOBStreamReader(FdoInt32 index)
{
    return m_reader->GetLOBStreamReader(index);
}

FdoIRaster* MgFdoForcedOneToOneFeatureReader::GetRaster(FdoInt32 index)
{
    return m_reader->GetRaster(index);
}

const FdoByte* MgFdoForcedOneToOneFeatureReader::GetGeometry(FdoInt32 index, FdoInt32 * count)
{
    return m_reader->GetGeometry(index, count);
}

FdoByteArray* MgFdoForcedOneToOneFeatureReader::GetGeometry(FdoInt32 index)
{
    return m_reader->GetGeometry(index);
}

FdoIFeatureReader* MgFdoForcedOneToOneFeatureReader::GetFeatureObject (FdoInt32 index)
{
    return m_reader->GetFeatureObject(index);
}

FdoString* MgFdoForcedOneToOneFeatureReader::GetPropertyName(FdoInt32 index)
{
    return m_reader->GetPropertyName(index);
}

FdoInt32 MgFdoForcedOneToOneFeatureReader::GetPropertyIndex(FdoString* propertyName)
{
    return m_reader->GetPropertyIndex(propertyName);
}

bool MgFdoForcedOneToOneFeatureReader::ReadNext()
{
    bool ret = false;

    ret = m_reader->ReadNext();
    if (!ret) //End-of-reader
        return ret;

    //So we're forcing one-to-one, this means we need to keep track of 
    //identity property values. We hash each one and store in a set, if 
    //our generated hash already exists, we skip it.
    STRING hash = GetIdentityHash();
    while (m_idValues.find(hash) != m_idValues.end()) 
    {
        ret = m_reader->ReadNext(); //Read next feature
        if (!ret) //End-of-reader
            return ret;
        hash = GetIdentityHash();
    }
    //Add this hash
    m_idValues.insert(hash);

    return ret;
}

void MgFdoForcedOneToOneFeatureReader::Close()
{
    m_reader->Close();
}


FdoDateTime MgFdoForcedOneToOneFeatureReader::GetDateTime(const wchar_t *propertyName )
{
    return m_reader->GetDateTime(propertyName);
}

STRING MgFdoForcedOneToOneFeatureReader::GetIdentityHash()
{
    STRING ret;

    if (NULL == m_cachedClsDef)
    {
        m_cachedClsDef = m_reader->GetClassDefinition();
    }

    //return the current identity values as: <identity 1>|<identity 2>|...|<identity n>
    const wchar_t* SEPARATOR = L"|";

    FdoPtr<FdoPropertyDefinitionCollection> clsProps = m_cachedClsDef->GetProperties();
    for (INT32 i = 0; i < m_idPropNames->GetCount(); i++) 
    {
        bool append = false;
        wchar_t tbuff[256];
        //Don't bother with IsNull() checks. These are identity property values, how can they be null?
        FdoStringElement* el = m_idPropNames->GetItem(i);
        FdoStringP str = el->GetString();
        FdoString* name = (FdoString*)str;
        
        FdoPtr<FdoPropertyDefinition> propDef = clsProps->GetItem(name);
        if (propDef->GetPropertyType() == FdoPropertyType_DataProperty)
        {
            FdoDataPropertyDefinition* dataProp = static_cast<FdoDataPropertyDefinition*>(propDef.p);
            FdoDataType dtype = dataProp->GetDataType();
            //I don't like to assume, but this is a safe assumption to make. There is no way in hell
            //these types could ever be identity property types:
            // - BLOB
            // - CLOB
            // - Feature
            // - Geometry
            // - Raster
            //So they can be ignored here.
            switch(dtype)
            {
            case FdoDataType_Boolean:
                {
                    bool val = m_reader->GetBoolean(name);
                    swprintf (tbuff, 256, L"%s", val ? L"TRUE" : L"FALSE");
                    append = true;
                }
                break;
            case FdoDataType_Byte:
                {
                    BYTE val = m_reader->GetByte(name);
                    swprintf (tbuff, 256, L"%d", val);
                    append = true;
                }
                break;
            case FdoDataType_DateTime:
                {
                    FdoDateTime dt = m_reader->GetDateTime(name);
                    swprintf (tbuff, 256, L"%d-%d-%d %2d:%2d:%2.4f",
                                          dt.year,
                                          dt.month, 
                                          dt.day, 
                                          dt.hour, 
                                          dt.minute, 
                                          dt.seconds);
                    append = true;
                }
                break;
            case FdoDataType_Decimal:
            case FdoDataType_Double:
                {
                    double val = m_reader->GetDouble(name);
                    swprintf (tbuff, 256, L"%f", val);
                    append = true;
                }
                break;
            case FdoDataType_Int16:
                {
                    INT16 val = m_reader->GetInt16(name);
                    swprintf (tbuff, 256, L"%d", val);
                    append = true;
                }
                break;
            case FdoDataType_Int32:
                {
                    INT32 val = m_reader->GetInt32(name);
                    swprintf (tbuff, 256, L"%ld", val);
                    append = true;
                }
                break;
            case FdoDataType_Int64:
                {
                    INT64 val = m_reader->GetInt64(name);
                    #ifdef _WIN32
                    _i64tow (val, tbuff, 10);
                    #else
                    swprintf(tbuff, 256, L"%lli", val);
                    #endif
                    append = true;
                }
                break;
            case FdoDataType_Single:
                {
                    float val = m_reader->GetSingle(name);
                    swprintf (tbuff, 256, L"%f", val);
                    append = true;
                }
                break;
            case FdoDataType_String:
                {
                    STRING val = m_reader->GetString(name);
                    if (ret.empty())
                    {
                        ret = val;
                    }
                    else
                    {
                        ret += SEPARATOR;
                        ret += val;
                    }   
                    append = false; //already appended
                }
                break;
            }

            if (append)
            {
                if (ret.empty())
                {
                    ret = tbuff;
                }
                else
                {
                    ret += SEPARATOR;
                    ret += tbuff;
                }   
            }
        }
        else
        {
            
        }
    }

    return ret;
}
