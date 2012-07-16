#ifndef DESKTOP_CREATE_FEATURE_SOURCE_H
#define DESKTOP_CREATE_FEATURE_SOURCE_H

class FdoIConnection;
class MgResourceIdentifier;
class MgResourceService;
class MgFeatureSourceParams;
class MgFileFeatureSourceParams;
template class FdoPtr<FdoIConnection>;

class MgCreateFeatureSource : public MgGuardDisposable
{
public:
    MgCreateFeatureSource();
    virtual ~MgCreateFeatureSource();
    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    virtual void Dispose() { delete this; }
};

class MgCreateRdbmsFeatureSource : public MgGuardDisposable
{
public:
    MgCreateRdbmsFeatureSource(MgResourceIdentifier* resource, MgRdbmsFeatureSourceParams* params);
    virtual ~MgCreateRdbmsFeatureSource();

    void CreateFeatureSource();
    virtual void Dispose();

private:
    void WriteFeatureSourceContent();

    MgResourceIdentifier* m_resource;
    MgRdbmsFeatureSourceParams* m_params;
};

class MgCreateFileFeatureSource : public MgGuardDisposable
{
public:
    MgCreateFileFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgCreateFileFeatureSource();
    void CreateFeatureSource(bool bCheckFeatureClass = false, bool bCheckSpatialContext = false);
    virtual void Dispose();

protected:
    virtual void CreateDataStore(FdoIConnection* conn);
    virtual void ApplySchemaAndCreateSpatialContext(FdoIConnection* conn);
    virtual void SetFeatureSourceDefinition(MgResourceService* resourceService);
    virtual void SetResourceData(MgResourceService* resourceService);
    virtual STRING GetFirstConnectionString();
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;

protected:
    STRING GetFileName() const;
    MgResourceService* GetResourceService();
    void ApplySchemaAndCreateSpatialContextInternal(FdoIConnection* conn);
    void SetResourceDataInternal(MgResourceService* resourceService, STRING source, STRING target);

protected:
    STRING m_fileExtension;
    STRING m_connectParamName;
    STRING m_tempFileName;
    MgResourceIdentifier* m_resource;
    MgFileFeatureSourceParams* m_params;
};

class MgCreateSdfFeatureSource: public MgCreateFileFeatureSource
{
public:
    MgCreateSdfFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgCreateSdfFeatureSource();
protected:
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgCreateShpFeatureSource: public MgCreateFileFeatureSource
{
public:
    MgCreateShpFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgCreateShpFeatureSource();
protected:
    virtual void CreateDataStore(FdoIConnection* conn);
    virtual void ApplySchemaAndCreateSpatialContext(FdoIConnection* conn);
    virtual void SetResourceData(MgResourceService* resourceService);
    virtual STRING GetFirstConnectionString();
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgCreateSqliteFeatureSource: public MgCreateFileFeatureSource
{
public:
    MgCreateSqliteFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgCreateSqliteFeatureSource();
protected:
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

#endif