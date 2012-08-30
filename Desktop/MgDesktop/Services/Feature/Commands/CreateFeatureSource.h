#ifndef DESKTOP_CREATE_FEATURE_SOURCE_H
#define DESKTOP_CREATE_FEATURE_SOURCE_H

class FdoIConnection;
class MgResourceIdentifier;
class MgResourceService;
class MgFeatureSourceParams;
class MgFileFeatureSourceParams;
template class FdoPtr<FdoIConnection>;

class MgdCreateFeatureSource : public MgGuardDisposable
{
public:
    MgdCreateFeatureSource();
    virtual ~MgdCreateFeatureSource();
    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    virtual void Dispose() { delete this; }
};

class MgdCreateRdbmsFeatureSource : public MgGuardDisposable
{
public:
    MgdCreateRdbmsFeatureSource(MgResourceIdentifier* resource, MgdRdbmsFeatureSourceParams* params);
    virtual ~MgdCreateRdbmsFeatureSource();

    void CreateFeatureSource();
    virtual void Dispose();

private:
    void WriteFeatureSourceContent();

    MgResourceIdentifier* m_resource;
    MgdRdbmsFeatureSourceParams* m_params;
};

class MgdCreateFileFeatureSource : public MgGuardDisposable
{
public:
    MgdCreateFileFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgdCreateFileFeatureSource();
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

class MgdCreateSdfFeatureSource: public MgdCreateFileFeatureSource
{
public:
    MgdCreateSdfFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgdCreateSdfFeatureSource();
protected:
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgdCreateShpFeatureSource: public MgdCreateFileFeatureSource
{
public:
    MgdCreateShpFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgdCreateShpFeatureSource();
protected:
    virtual void CreateDataStore(FdoIConnection* conn);
    virtual void ApplySchemaAndCreateSpatialContext(FdoIConnection* conn);
    virtual void SetResourceData(MgResourceService* resourceService);
    virtual STRING GetFirstConnectionString();
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgdCreateSqliteFeatureSource: public MgdCreateFileFeatureSource
{
public:
    MgdCreateSqliteFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgdCreateSqliteFeatureSource();
protected:
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

#endif