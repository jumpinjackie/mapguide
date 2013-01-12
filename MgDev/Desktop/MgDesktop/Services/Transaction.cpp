#include "Transaction.h"
#include "Fdo.h"
#include "Services/Feature/FeatureDefs.h"
#include "Services/Feature/FeatureConnection.h"

MgdTransaction::MgdTransaction(MgdFeatureConnection* conn, MgResourceIdentifier* resource)
{
    CHECKARGUMENTNULL(conn, L"MgdTransaction::ctor");
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdTransaction::ctor");

    m_conn = SAFE_ADDREF(conn);
    FdoPtr<FdoIConnection> fdoConn = m_conn->GetConnection();

	m_trans = fdoConn->BeginTransaction();
	m_resource = SAFE_ADDREF(resource);
}
MgdTransaction::~MgdTransaction()
{
    SAFE_RELEASE(m_conn);
	FDO_SAFE_RELEASE(m_trans);
	SAFE_RELEASE(m_resource);
}

FdoITransaction* MgdTransaction::GetFdoTransaction()
{
    return FDO_SAFE_ADDREF(m_trans);
}

MgdFeatureConnection* MgdTransaction::GetConnection()
{
    return SAFE_ADDREF(m_conn);
}

void MgdTransaction::Serialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdTransaction::Serialize");
}

void MgdTransaction::Deserialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdTransaction::Deserialize");
}

void MgdTransaction::Commit()
{
    MG_FEATURE_SERVICE_TRY()
	m_trans->Commit();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdTransaction::Commit")
}

void MgdTransaction::Rollback()
{
    MG_FEATURE_SERVICE_TRY()
	m_trans->Rollback();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdTransaction::Rollback")
}

MgResourceIdentifier* MgdTransaction::GetFeatureSource()
{
	return SAFE_ADDREF(m_resource);
}

STRING MgdTransaction::AddSavePoint(CREFSTRING suggestName)
{
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
    FdoString* name = m_trans->AddSavePoint(suggestName.c_str());
    ret = STRING(name);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdTransaction::Rollback")
    return ret;
}

void MgdTransaction::ReleaseSavePoint(CREFSTRING savePointName)
{
    MG_FEATURE_SERVICE_TRY()
    m_trans->ReleaseSavePoint(savePointName.c_str());
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdTransaction::ReleaseSavePoint")
}

void MgdTransaction::Rollback(CREFSTRING savePointName)
{
    MG_FEATURE_SERVICE_TRY()
    m_trans->Rollback(savePointName.c_str());
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdTransaction::Rollback")
}