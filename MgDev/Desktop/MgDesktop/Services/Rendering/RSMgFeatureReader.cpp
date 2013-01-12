#include "RSMgFeatureReader.h"
#include "RSMgRaster.h"
#include "RSMgInputStream.h"
#include "LineBuffer.h"
#include "Services/FeatureReader.h"
//#include "ServerGwsFeatureReader.h"

//we want to catch the MgException and rethrow a FdoException which
//the Stylizer knows how to catch and release. It does not know about
//MgExceptions so it can't properly release them
#define RSFR_TRY() try {
#define RSFR_CATCH()   }                                                                    \
                       catch (MgException* ex)                                              \
                       {                                                                    \
                           STRING message;                                                  \
                           message = ex->GetExceptionMessage();                             \
                           ex->Release();                                                   \
                           throw FdoException::Create(message.c_str());                     \
                       }


RSMgdFeatureReader::RSMgdFeatureReader(MgFeatureReader* reader, MgFeatureService* svcFeature, MgResourceIdentifier* featResId, MgFeatureQueryOptions* options, const STRING& geomPropName)
{
    _ASSERT(NULL != reader);
    m_reader = SAFE_ADDREF(reader);

    //stuff needed for resetting the reader
    m_svcFeature = SAFE_ADDREF(svcFeature);
    m_resId = SAFE_ADDREF(featResId);
    m_options = SAFE_ADDREF(options);

    //use geometry property given by user
    m_geomPropName = geomPropName;
    m_rasterPropName = L"";
    m_cachePropValue = L"";

    //keep track of the class for the lifetime of this reader
    //since we point to strings inside it.
    m_class = reader->GetClassDefinitionNoXml();

    //class properties, including inherited base class properties
    Ptr<MgPropertyDefinitionCollection> pdc = m_class->GetPropertiesIncludingBase();

    //
    // Set up the cache of propery data types and indices
    //

    m_numProps = pdc->GetCount();
    m_vProps = new PropertyStub[m_numProps];

    m_propNames = new wchar_t*[m_numProps];

    int index = 0;

    //class properties
    for (int i=0; i<pdc->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> pd = pdc->GetItem(i);

        MgDataPropertyDefinition* dpd = dynamic_cast<MgDataPropertyDefinition*>(pd.p);

        PropertyStub* ps = &m_vProps[index];
        ps->m_name = pd->GetName();

        ps->m_name_cstr = ps->m_name.c_str();
        //remember the property name in the name list also
        m_propNames[index] = (wchar_t*)ps->m_name_cstr;

        ps->m_recordIndex = index++;
        //TODO:
        //BOGUS: save data type, or -1 for geometry. -1 is a little risky
        ps->m_dataType = (dpd != NULL) ? dpd->GetDataType() : (FdoDataType)-1;

        //look for a raster property
        //currently just remember the first raster property
        //we encounter. This should be fine with the raster provider
        if (pd->GetPropertyType() == MgFeaturePropertyType::RasterProperty
            && m_rasterPropName.empty())
        {
            MgRasterPropertyDefinition* rpd = (MgRasterPropertyDefinition*)(pd.p);
            m_rasterPropName = rpd->GetName();
        }
    }

    //loop over the identity properties also

    Ptr<MgPropertyDefinitionCollection> idpdc = m_class->GetIdentityProperties();
    m_numIdProps = idpdc->GetCount();
    m_idpropNames = new wchar_t*[m_numIdProps];

    for (int i=0; i<m_numIdProps; i++)
    {
        Ptr<MgPropertyDefinition> idpd = idpdc->GetItem(i);
        STRING propertyName = idpd->GetName();

        //find PropertyStub in the cache list
        PropertyStub* ps;

        //search for the correct PropertyStub -- iterate linearly
        //since we have relatively few properties. Using a hash_map
        //was slow for this
        for (int j=0; j<m_numProps; j++)
        {
            ps = &m_vProps[j];

            if (wcscmp((wchar_t*)(propertyName.c_str()), ps->m_name_cstr) == 0)
                break;
        }

        m_idpropNames[i] = (wchar_t*)ps->m_name_cstr;
    }

    //if there is a raster property, use the raster property
    //and zero out the geometry property
    //otherwise we will use the geometry proeprty
    if (m_rasterPropName == geomPropName)
        m_geomPropName = L"";
}


RSMgdFeatureReader::~RSMgdFeatureReader()
{
    // Reader may be null if an Fdo exception is thrown
    // during a call to Reset()
    if (NULL != m_reader)
    {
        m_reader->Close();
    }

    SAFE_RELEASE(m_reader);
    SAFE_RELEASE(m_class);
    SAFE_RELEASE(m_svcFeature);
    SAFE_RELEASE(m_resId);
    SAFE_RELEASE(m_options);

    delete [] m_vProps;
    delete [] m_propNames;
    delete [] m_idpropNames;
}


bool RSMgdFeatureReader::ReadNext()
{
    RSFR_TRY()
    return m_reader->ReadNext();
    RSFR_CATCH()
}


void RSMgdFeatureReader::Close()
{
    RSFR_TRY()
    m_reader->Close();
    RSFR_CATCH()
}


void RSMgdFeatureReader::Reset()
{
    RSFR_TRY()

    m_reader->Close();
    SAFE_RELEASE(m_reader);

    m_reader = m_svcFeature->SelectFeatures(m_resId, m_class->GetQualifiedName(), m_options);

    RSFR_CATCH()
}


bool RSMgdFeatureReader::IsNull(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->IsNull(propertyName);
    RSFR_CATCH()
}


bool RSMgdFeatureReader::GetBoolean(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetBoolean(propertyName);
    RSFR_CATCH()
}


unsigned char RSMgdFeatureReader::GetByte(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetByte(propertyName);
    RSFR_CATCH()
}


FdoDateTime RSMgdFeatureReader::GetDateTime(const wchar_t* propertyName)
{
    RSFR_TRY()

    Ptr<MgDateTime> mgdate = m_reader->GetDateTime(propertyName);

    FdoDateTime date;
    date.day = mgdate->GetDay();
    date.hour = mgdate->GetHour();
    date.minute = mgdate->GetMinute();
    date.month = mgdate->GetMonth();
    date.seconds = (float)mgdate->GetSecond() + (float)mgdate->GetMicrosecond() * 1.0e-6f;
    date.year = mgdate->GetYear();

    return date;

    RSFR_CATCH()
}


float RSMgdFeatureReader::GetSingle(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetSingle(propertyName);
    RSFR_CATCH()
}


double RSMgdFeatureReader::GetDouble(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetDouble(propertyName);
    RSFR_CATCH()
}


short RSMgdFeatureReader::GetInt16(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetInt16(propertyName);
    RSFR_CATCH()
}


int RSMgdFeatureReader::GetInt32(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetInt32(propertyName);
    RSFR_CATCH()
}


long long RSMgdFeatureReader::GetInt64(const wchar_t* propertyName)
{
    RSFR_TRY()
    return m_reader->GetInt64(propertyName);
    RSFR_CATCH()
}


const wchar_t* RSMgdFeatureReader::GetString(const wchar_t* propertyName)
{
    try
    {
        INT32 len = 0; //not used, as string is null terminated
        return m_reader->GetString(propertyName, len);
    }
    //Ignore FDO exceptions.  May simply be a null string value.
    catch (MgException* ex)
    {
        STRING message;
        message = ex->GetExceptionMessage();

        bool shouldThrow = (NULL == dynamic_cast<MgFdoException*>(ex));
        ex->Release();
        if (shouldThrow)
        {
            throw FdoException::Create(message.c_str());
        }
    }

    return L"";
}


LineBuffer* RSMgdFeatureReader::GetGeometry(const wchar_t*   propertyName,
                                           LineBuffer*      lb,
                                           CSysTransformer* xformer)
{
    RSFR_TRY()

    INT32 sz;
    unsigned char* agf = (unsigned char*)m_reader->GetGeometry(propertyName, sz);

    //The MgFeatureReader returns null instead of letting the null geometry exception through
    //This is incorrect, but I am not sure if any other code that uses the API relies on that
    //so let's check for null and throw the exception
    if (!agf)
        throw FdoException::Create(FdoException::NLSGetMessage(FDO_NLSID(FDO_60_NULL_POINTER)));

    _ASSERT(lb);
    lb->LoadFromAgf(agf, sz, xformer);

    return lb;

    RSFR_CATCH()
}


RS_Raster* RSMgdFeatureReader::GetRaster(const wchar_t* propertyName)
{
    RSFR_TRY()
    Ptr<MgRaster> raster = m_reader->GetRaster(propertyName);
    return new RSMgdRaster(raster);
    RSFR_CATCH()
}


RS_InputStream* RSMgdFeatureReader::GetBLOB(const wchar_t* propertyName)
{
    RSFR_TRY()
    Ptr<MgByteReader> rdr = m_reader->GetBLOB(propertyName);
    return new RSMgdInputStream(rdr);
    RSFR_CATCH()
}


RS_InputStream* RSMgdFeatureReader::GetCLOB(const wchar_t* propertyName)
{
    RSFR_TRY()
    Ptr<MgByteReader> rdr = m_reader->GetCLOB(propertyName);
    return new RSMgdInputStream(rdr);
    RSFR_CATCH()
}


int RSMgdFeatureReader::GetPropertyType(const wchar_t* propertyName)
{
    PropertyStub* ps;

    //search for the correct PropertyStub -- iterate linearly
    //since we have relatively few properties. Using a hash_map
    //was slow for this
    for (int i=0; i<m_numProps; i++)
    {
        ps = &m_vProps[i];

        if (wcscmp((wchar_t*)propertyName, ps->m_name_cstr) == 0)
            break;
    }

    //we need to return an FdoDataType, so map Mg data types
    //to FDO types. I think they are equal, but we do this just to be safe in the future
    switch (ps->m_dataType)
    {
    case MgPropertyType::Null:
        //huh?
        break;
    case MgPropertyType::Boolean:
        return FdoDataType_Boolean;
    case MgPropertyType::Byte:
        return FdoDataType_Byte;
    case MgPropertyType::DateTime:
        return FdoDataType_DateTime;
    case MgPropertyType::Single:
        return FdoDataType_Single;
    case MgPropertyType::Double:
        return FdoDataType_Double;
    case MgPropertyType::Int16:
        return FdoDataType_Int16;
    case MgPropertyType::Int32:
        return FdoDataType_Int32;
    case MgPropertyType::Int64:
        return FdoDataType_Int64;
    case MgPropertyType::String:
        return FdoDataType_String;
    case MgPropertyType::Blob:
        return FdoDataType_BLOB;
    case MgPropertyType::Clob:
        return FdoDataType_CLOB;
    case MgPropertyType::Feature:
        //TODO: no value for object property? Return -1 for now
    case MgPropertyType::Geometry:
    case MgPropertyType::Raster:
    default:
        return -1; //Stylization treats -1 as geometry/raster
    }

    //should not happen
    return 0;
}


const wchar_t* RSMgdFeatureReader::GetAsString(const wchar_t* propertyName)
{
    static const size_t MAX_STRING = 64;

    try
    {
        switch (GetPropertyType(propertyName))
        {
        case FdoDataType_Boolean:
            {
                bool bVal = GetBoolean(propertyName);
                if (bVal)
                    m_cachePropValue = L"true";
                else
                    m_cachePropValue = L"false";
            }
            break;
        case FdoDataType_Byte:
            {
                unsigned char ucVal = GetByte(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%d", ucVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_DateTime:
            {
                FdoPtr<FdoDateTimeValue> dt = FdoDateTimeValue::Create(GetDateTime(propertyName));
                m_cachePropValue = dt->ToString();
            }
            break;
        case FdoDataType_Decimal:
        // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
        // An MgDecimalProperty class should be implemented in a future release.
        // Fall through to double code.
        case FdoDataType_Double:
            {
                double dVal = GetDouble(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%.15g", dVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_Int16:
            {
                short sVal = GetInt16(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%d", sVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_Int32:
            {
                int iVal = GetInt32(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%d", iVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_Int64:
            {
                long long llVal = GetInt64(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%lld", llVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_Single:
            {
                float fVal = GetSingle(propertyName);
                wchar_t temp[MAX_STRING];
                swprintf(temp, MAX_STRING, L"%.7g", fVal);
                m_cachePropValue = temp;
            }
            break;
        case FdoDataType_String:
            {
                m_cachePropValue = GetString(propertyName);
            }
            break;
        default:
            m_cachePropValue = L"";
            break;
        }
    }
    catch (FdoException* e)
    {
        e->Release();

        //value is likely null in the provider, so just use empty string
        //to put into DWF
        m_cachePropValue = L"";
    }

    return m_cachePropValue.c_str();
}


const wchar_t* RSMgdFeatureReader::GetGeomPropName()
{
    if (m_geomPropName.empty())
        return NULL;
    else
        return m_geomPropName.c_str();
}


const wchar_t* RSMgdFeatureReader::GetRasterPropName()
{
    //The Mapping service will add this computed property
    //to the query
    if (!m_rasterPropName.empty())
        return L"clipped_raster";
    else
        return NULL;
    //return m_rasterPropName;
}


const wchar_t*const* RSMgdFeatureReader::GetIdentPropNames(int& count)
{
    count = m_numIdProps;
    return m_idpropNames;
}


const wchar_t*const* RSMgdFeatureReader::GetPropNames(int& count)
{
    count = m_numProps;
    return m_propNames;
}


FdoIFeatureReader* RSMgdFeatureReader::GetInternalReader()
{
    MgdFeatureReader *fr = dynamic_cast<MgdFeatureReader*>(m_reader);
    if (fr)
        return fr->GetInternalReader();

    /*
    MgServerFeatureReader* sfr = dynamic_cast<MgServerFeatureReader*>(m_reader);
    if (sfr)
        return sfr->GetInternalReader();

    MgServerGwsFeatureReader* gfr = dynamic_cast<MgServerGwsFeatureReader*>(m_reader);
    if (gfr)
        return gfr->GetJoinFeatureReader();
    */
    // encountered a case we don't handle
    _ASSERT(false);
    return NULL;
}
