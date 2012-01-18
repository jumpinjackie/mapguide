#ifndef DESKTOP_RDBMS_FEATURE_SOURCE_PARAMS_H
#define DESKTOP_RDBMS_FEATURE_SOURCE_PARAMS_H

#include "MgDesktop.h"

// Parameter class for creating RDBMS data stores
//
//
class MG_DESKTOP_API MgRdbmsFeatureSourceParams : public MgFeatureSourceParams
{
PUBLISHED_API:
    MgRdbmsFeatureSourceParams();

    STRING GetProviderName();   /// __get, __set
    void SetProviderName(CREFSTRING providerName);

    STRING GetService();   /// __get, __set
    void SetService(CREFSTRING name);

    STRING GetPassword();   /// __get, __set
    void SetPassword(CREFSTRING pwd);

    STRING GetUsername();   /// __get, __set
    void SetUsername(CREFSTRING username);

    STRING GetDataStore();  /// __get, __set
    void SetDataStore(CREFSTRING dataStore);

    STRING GetSpatialContextName();  /// __get, __set
    void SetSpatialContextName(CREFSTRING name);

    STRING GetSpatialContextDescription();  /// __get, __set
    void SetSpatialContextDescription(CREFSTRING description);

    STRING GetCoordinateSystemWkt();  /// __get, __set
    void SetCoordinateSystemWkt(CREFSTRING srsWkt);

    STRING GetCoordinateSystemName();  /// __get, __set
    void SetCoordinateSystemName(CREFSTRING csName);

    double GetXYTolerance();  /// __get, __set
    void SetXYTolerance(double tolerance);

    double GetZTolerance();  /// __get, __set
    void SetZTolerance(double tolerance);

    MgFeatureSchema* GetFeatureSchema();  /// __get, __set
    void SetFeatureSchema(MgFeatureSchema* featureSchema);

    MgEnvelope* GetSpatialContextExtents();  /// __get, __set
    void SetSpatialContextExtents(MgEnvelope* extents);

    bool GetIsFdoEnabled();  /// __get, __set
    void SetIsFdoEnabled(bool useFdoMetadata);

protected:
    virtual void Dispose();

    virtual ~MgRdbmsFeatureSourceParams();

private:
    STRING m_provider;

    STRING m_service;
    STRING m_username;
    STRING m_password;
    STRING m_datastore;

    STRING m_scName;
    STRING m_scDescription;
    STRING m_scCsName;
    STRING m_scCsWkt;
    double m_xyTolerance;
    double m_zTolerance;
    bool m_useFdoMetadata;

    Ptr<MgFeatureSchema> m_schema;
    Ptr<MgEnvelope> m_extents;

INTERNAL_API:
    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);
};

#endif