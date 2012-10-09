#include "RdbmsFeatureSourceParams.h"

MgdRdbmsFeatureSourceParams::MgdRdbmsFeatureSourceParams()
{

}

MgdRdbmsFeatureSourceParams::~MgdRdbmsFeatureSourceParams()
{

}

void MgdRdbmsFeatureSourceParams::Serialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdRdbmsFeatureSourceParams::Serialize");
}

void MgdRdbmsFeatureSourceParams::Deserialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdRdbmsFeatureSourceParams::Serialize");
}

void MgdRdbmsFeatureSourceParams::Dispose() { delete this; }

STRING MgdRdbmsFeatureSourceParams::GetProviderName() { return m_provider; }

void MgdRdbmsFeatureSourceParams::SetProviderName(CREFSTRING providerName) { m_provider = providerName; }

STRING MgdRdbmsFeatureSourceParams::GetService() { return m_service; }

void MgdRdbmsFeatureSourceParams::SetService(CREFSTRING service) { m_service = service; }

STRING MgdRdbmsFeatureSourceParams::GetPassword() { return m_password; }

void MgdRdbmsFeatureSourceParams::SetPassword(CREFSTRING pwd) { m_password = pwd; }

STRING MgdRdbmsFeatureSourceParams::GetUsername() { return m_username; }

void MgdRdbmsFeatureSourceParams::SetUsername(CREFSTRING username) { m_username = username; }

STRING MgdRdbmsFeatureSourceParams::GetDataStore() { return m_datastore; }

void MgdRdbmsFeatureSourceParams::SetDataStore(CREFSTRING dataStore) { m_datastore = dataStore; }

STRING MgdRdbmsFeatureSourceParams::GetSpatialContextName() { return m_scName; }

void MgdRdbmsFeatureSourceParams::SetSpatialContextName(CREFSTRING name) { m_scName = name; }

STRING MgdRdbmsFeatureSourceParams::GetSpatialContextDescription() { return m_scDescription; }

void MgdRdbmsFeatureSourceParams::SetSpatialContextDescription(CREFSTRING description) { m_scDescription = description; }

STRING MgdRdbmsFeatureSourceParams::GetCoordinateSystemWkt() { return m_scCsWkt; }

void MgdRdbmsFeatureSourceParams::SetCoordinateSystemWkt(CREFSTRING srsWkt) { m_scCsWkt = srsWkt; }

STRING MgdRdbmsFeatureSourceParams::GetCoordinateSystemName() { return m_scCsName; }

void MgdRdbmsFeatureSourceParams::SetCoordinateSystemName(CREFSTRING csName) { m_scCsName = csName; }

double MgdRdbmsFeatureSourceParams::GetXYTolerance() { return m_xyTolerance; }

void MgdRdbmsFeatureSourceParams::SetXYTolerance(double tolerance) { m_xyTolerance = tolerance; }

double MgdRdbmsFeatureSourceParams::GetZTolerance() { return m_zTolerance; }

void MgdRdbmsFeatureSourceParams::SetZTolerance(double tolerance) { m_zTolerance = tolerance; }

MgFeatureSchema* MgdRdbmsFeatureSourceParams::GetFeatureSchema() { return SAFE_ADDREF((MgFeatureSchema*)m_schema); }

void MgdRdbmsFeatureSourceParams::SetFeatureSchema(MgFeatureSchema* featureSchema) { m_schema = SAFE_ADDREF(featureSchema); }

MgEnvelope* MgdRdbmsFeatureSourceParams::GetSpatialContextExtents() { return SAFE_ADDREF((MgEnvelope*)m_extents); }

void MgdRdbmsFeatureSourceParams::SetSpatialContextExtents(MgEnvelope* extents) { m_extents = SAFE_ADDREF(extents); }

bool MgdRdbmsFeatureSourceParams::GetIsFdoEnabled() { return m_useFdoMetadata; }

void MgdRdbmsFeatureSourceParams::SetIsFdoEnabled(bool useFdoMetadata) { m_useFdoMetadata = useFdoMetadata; }