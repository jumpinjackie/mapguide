#include "ScrollableFeatureReader.h"
#include "Services/Feature/FeatureDefs.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/RasterHelper.h"
#include "Fdo.h"

MgdScrollableFeatureReader::MgdScrollableFeatureReader(MgdFeatureConnection* conn, FdoIScrollableFeatureReader* reader)
: MgdFeatureReader(conn, (FdoIFeatureReader*)reader)
{
	m_scrollReader = FDO_SAFE_ADDREF(reader);
    m_connection = SAFE_ADDREF(conn);

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();
}

MgdScrollableFeatureReader::~MgdScrollableFeatureReader() 
{
	FDO_SAFE_RELEASE(m_scrollReader);
    m_connection = NULL;
} 

// --------- Scrollable API ----------- //
INT32 MgdScrollableFeatureReader::Count()
{
    INT32 ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_scrollReader->Count();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::Count")
    return ret;
}

bool MgdScrollableFeatureReader::ReadFirst()
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_scrollReader->ReadFirst();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::ReadFirst")
    return ret;
}

bool MgdScrollableFeatureReader::ReadLast()
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_scrollReader->ReadLast();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::ReadLast")
    return ret;
}

bool MgdScrollableFeatureReader::ReadPrevious()
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_scrollReader->ReadPrevious();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::ReadPrevious")
    return ret;
}

bool MgdScrollableFeatureReader::ReadAt(MgPropertyCollection* key)
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    FdoPtr<FdoPropertyValueCollection> propVals = FdoPropertyValueCollection::Create();
    MgdFeatureUtil::FillFdoPropertyCollection(key, propVals);
    ret = m_scrollReader->ReadAt(propVals);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::ReadAt")
    return ret;
}

bool MgdScrollableFeatureReader::ReadAtIndex(UINT32 recordIndex)
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_scrollReader->ReadAtIndex((unsigned int)recordIndex);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::ReadAtIndex")
    return ret;
}

UINT32 MgdScrollableFeatureReader::IndexOf(MgPropertyCollection* key)
{
    UINT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    FdoPtr<FdoPropertyValueCollection> propVals = FdoPropertyValueCollection::Create();
    MgdFeatureUtil::FillFdoPropertyCollection(key, propVals);
    ret = (UINT32)m_scrollReader->IndexOf(propVals);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdScrollableFeatureReader::IndexOf")
    return ret;
}

FdoIScrollableFeatureReader* MgdScrollableFeatureReader::GetInternalReader()
{
    return FDO_SAFE_ADDREF(m_scrollReader); 
}