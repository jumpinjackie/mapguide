#include "RdbmsFeatureSourceParams.h"

MgRdbmsFeatureSourceParams::MgRdbmsFeatureSourceParams()
{

}

MgRdbmsFeatureSourceParams::~MgRdbmsFeatureSourceParams()
{

}

void MgRdbmsFeatureSourceParams::Serialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgRdbmsFeatureSourceParams::Serialize");
}

void MgRdbmsFeatureSourceParams::Deserialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgRdbmsFeatureSourceParams::Serialize");
}

void MgRdbmsFeatureSourceParams::Dispose() { delete this; }

STRING MgRdbmsFeatureSourceParams::GetProviderName() { return m_provider; }

void MgRdbmsFeatureSourceParams::SetProviderName(CREFSTRING providerName) { m_provider = providerName; }

STRING MgRdbmsFeatureSourceParams::GetService() { return m_service; }

void MgRdbmsFeatureSourceParams::SetService(CREFSTRING service) { m_service = service; }

STRING MgRdbmsFeatureSourceParams::GetPassword() { return m_password; }

void MgRdbmsFeatureSourceParams::SetPassword(CREFSTRING pwd) { m_password = pwd; }

STRING MgRdbmsFeatureSourceParams::GetUsername() { return m_username; }

void MgRdbmsFeatureSourceParams::SetUsername(CREFSTRING username) { m_username = username; }

STRING MgRdbmsFeatureSourceParams::GetDataStore() { return m_datastore; }

void MgRdbmsFeatureSourceParams::SetDataStore(CREFSTRING dataStore) { m_datastore = dataStore; }

STRING MgRdbmsFeatureSourceParams::GetSpatialContextName() { return m_scName; }

void MgRdbmsFeatureSourceParams::SetSpatialContextName(CREFSTRING name) { m_scName = name; }

STRING MgRdbmsFeatureSourceParams::GetSpatialContextDescription() { return m_scDescription; }

void MgRdbmsFeatureSourceParams::SetSpatialContextDescription(CREFSTRING description) { m_scDescription = description; }

STRING MgRdbmsFeatureSourceParams::GetCoordinateSystemWkt() { return m_scCsWkt; }

void MgRdbmsFeatureSourceParams::SetCoordinateSystemWkt(CREFSTRING srsWkt) { m_scCsWkt = srsWkt; }

STRING MgRdbmsFeatureSourceParams::GetCoordinateSystemName() { return m_scCsName; }

void MgRdbmsFeatureSourceParams::SetCoordinateSystemName(CREFSTRING csName) { m_scCsName = csName; }

double MgRdbmsFeatureSourceParams::GetXYTolerance() { return m_xyTolerance; }

void MgRdbmsFeatureSourceParams::SetXYTolerance(double tolerance) { m_xyTolerance = tolerance; }

double MgRdbmsFeatureSourceParams::GetZTolerance() { return m_zTolerance; }

void MgRdbmsFeatureSourceParams::SetZTolerance(double tolerance) { m_zTolerance = tolerance; }

MgFeatureSchema* MgRdbmsFeatureSourceParams::GetFeatureSchema() { return SAFE_ADDREF((MgFeatureSchema*)m_schema); }

void MgRdbmsFeatureSourceParams::SetFeatureSchema(MgFeatureSchema* featureSchema) { m_schema = SAFE_ADDREF(featureSchema); }

MgEnvelope* MgRdbmsFeatureSourceParams::GetSpatialContextExtents() { return SAFE_ADDREF((MgEnvelope*)m_extents); }

void MgRdbmsFeatureSourceParams::SetSpatialContextExtents(MgEnvelope* extents) { m_extents = SAFE_ADDREF(extents); }

bool MgRdbmsFeatureSourceParams::GetIsFdoEnabled() { return m_useFdoMetadata; }

void MgRdbmsFeatureSourceParams::SetIsFdoEnabled(bool useFdoMetadata) { m_useFdoMetadata = useFdoMetadata; }