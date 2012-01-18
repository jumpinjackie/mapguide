#include "MgDesktop.h"
#include "RasterHelper.h"

MgRasterHelper::MgRasterHelper(MgdFeatureReader* reader) : MgFeatureService() 
{
    m_freader = SAFE_ADDREF(reader);
}

MgRasterHelper::MgRasterHelper(MgdDataReader* reader) : MgFeatureService() 
{
    m_dreader = SAFE_ADDREF(reader);
}

MgRasterHelper::MgRasterHelper(MgdGwsFeatureReader* reader) : MgFeatureService() 
{
    m_gwsReader = SAFE_ADDREF(reader);
}

MgByteReader* MgRasterHelper::GetFeatureProviders() { NOT_IMPLEMENTED(L"MgRasterHelper::GetFeatureProviders"); }

MgStringCollection* MgRasterHelper::GetConnectionPropertyValues(CREFSTRING providerName,
                                                        CREFSTRING propertyName,
                                                        CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgRasterHelper::GetConnectionPropertyValues"); }

bool MgRasterHelper::TestConnection(CREFSTRING providerName, CREFSTRING connectionString) { NOT_IMPLEMENTED(L"MgRasterHelper::TestConnection"); }

bool MgRasterHelper::TestConnection(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgRasterHelper::TestConnection"); }

MgByteReader* MgRasterHelper::GetCapabilities(CREFSTRING providerName) { NOT_IMPLEMENTED(L"MgRasterHelper::GetCapabilities"); }

MgByteReader* MgRasterHelper::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString) { NOT_IMPLEMENTED(L"MgRasterHelper::GetCapabilities"); }

void MgRasterHelper::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema) { NOT_IMPLEMENTED(L"MgRasterHelper::ApplySchema"); }

MgFeatureSchemaCollection* MgRasterHelper::DescribeSchema(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName,
                                                  MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeSchema"); }

MgFeatureSchemaCollection* MgRasterHelper::DescribeSchema(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeSchema"); }

STRING MgRasterHelper::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                   CREFSTRING schemaName,
                                   MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeSchemaAsXml"); }

STRING MgRasterHelper::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                   CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeSchemaAsXml"); }

STRING MgRasterHelper::SchemaToXml(MgFeatureSchemaCollection* schema) { NOT_IMPLEMENTED(L"MgRasterHelper::SchemaToXml"); }

MgFeatureSchemaCollection* MgRasterHelper::XmlToSchema(CREFSTRING xml) { NOT_IMPLEMENTED(L"MgRasterHelper::XmlToSchema"); }

MgFeatureReader* MgRasterHelper::SelectFeatures(MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options) { NOT_IMPLEMENTED(L"MgRasterHelper::SelectFeatures"); }

MgFeatureReader* MgRasterHelper::SelectFeatures(MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options,
                                        CREFSTRING coordinateSystem) { NOT_IMPLEMENTED(L"MgRasterHelper::SelectFeatures"); }

MgDataReader* MgRasterHelper::SelectAggregate(MgResourceIdentifier* resource,
                                      CREFSTRING className,
                                      MgFeatureAggregateOptions* options) { NOT_IMPLEMENTED(L"MgRasterHelper::SelectAggregate"); }

MgPropertyCollection* MgRasterHelper::UpdateFeatures(MgResourceIdentifier* resource,
                                             MgFeatureCommandCollection* commands,
                                             bool useTransaction) { NOT_IMPLEMENTED(L"MgRasterHelper::UpdateFeatures"); }

MgPropertyCollection* MgRasterHelper::UpdateFeatures(MgResourceIdentifier* resource,
                                             MgFeatureCommandCollection* commands,
                                             MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgRasterHelper::UpdateFeatures"); }

MgFeatureReader* MgRasterHelper::GetLockedFeatures(MgResourceIdentifier* resource,
                                           CREFSTRING className,
                                           MgFeatureQueryOptions* options) { NOT_IMPLEMENTED(L"MgRasterHelper::GetLockedFeatures"); }

MgTransaction* MgRasterHelper::BeginTransaction(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgRasterHelper::BeginTransaction"); }

MgSqlDataReader* MgRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement) { NOT_IMPLEMENTED(L"MgRasterHelper::ExecuteSqlQuery"); }

MgSqlDataReader* MgRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement,
                                         MgParameterCollection* parameters,
                                         MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgRasterHelper::ExecuteSqlQuery"); }

INT32 MgRasterHelper::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                 CREFSTRING sqlNonSelectStatement) { NOT_IMPLEMENTED(L"MgRasterHelper::ExecuteSqlNonQuery"); }

INT32 MgRasterHelper::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                 CREFSTRING sqlNonSelectStatement,
                                 MgParameterCollection* parameters,
                                 MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgRasterHelper::ExecuteSqlNonQuery"); }

MgSpatialContextReader* MgRasterHelper::GetSpatialContexts(MgResourceIdentifier* resource,
                                                   bool bActiveOnly) { NOT_IMPLEMENTED(L"MgRasterHelper::GetSpatialContexts"); }

MgLongTransactionReader* MgRasterHelper::GetLongTransactions(MgResourceIdentifier* resource,
                                                     bool bActiveOnly) { NOT_IMPLEMENTED(L"MgRasterHelper::GetLongTransactions"); }

bool MgRasterHelper::SetLongTransaction(MgResourceIdentifier* featureSourceId,
                                CREFSTRING longTransactionName) { NOT_IMPLEMENTED(L"MgRasterHelper::SetLongTransaction"); }

MgStringCollection* MgRasterHelper::GetSchemas(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgRasterHelper::GetSchemas"); }

MgStringCollection* MgRasterHelper::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgRasterHelper::GetClasses"); }

MgClassDefinition* MgRasterHelper::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className) { NOT_IMPLEMENTED(L"MgRasterHelper::GetClassDefinition"); }

void MgRasterHelper::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams) { NOT_IMPLEMENTED(L"MgRasterHelper::CreateFeatureSource"); }

MgByteReader* MgRasterHelper::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                             MgStringCollection* featureClasses) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeWfsFeatureType"); }

MgByteReader* MgRasterHelper::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                             MgStringCollection* featureClasses,
                                             CREFSTRING namespacePrefix,
                                             CREFSTRING namespaceUrl) { NOT_IMPLEMENTED(L"MgRasterHelper::DescribeWfsFeatureType"); }

MgByteReader* MgRasterHelper::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                    CREFSTRING featureClass,
                                    MgStringCollection* requiredProperties,
                                    CREFSTRING srs,
                                    CREFSTRING filter,
                                    INT32 maxFeatures) { NOT_IMPLEMENTED(L"MgRasterHelper::GetWfsFeature"); }

MgByteReader* MgRasterHelper::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                    CREFSTRING featureClass,
                                    MgStringCollection* requiredProperties,
                                    CREFSTRING srs,
                                    CREFSTRING filter,
                                    INT32 maxFeatures,
                                    CREFSTRING wfsVersion,
                                    CREFSTRING outputFormat,
                                    CREFSTRING sortCriteria,
                                    CREFSTRING namespacePrefix,
                                    CREFSTRING namespaceUrl) { NOT_IMPLEMENTED(L"MgRasterHelper::GetWfsFeature"); }

MgByteReader* MgRasterHelper::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgRasterHelper::EnumerateDataStores"); }

MgByteReader* MgRasterHelper::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgRasterHelper::GetSchemaMapping"); }



// Feature
MgBatchPropertyCollection* MgRasterHelper::GetFeatures(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgRasterHelper::GetFeatures"); }
bool MgRasterHelper::CloseFeatureReader(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgRasterHelper::CloseFeatureReader"); }

// SQL
MgBatchPropertyCollection* MgRasterHelper::GetSqlRows(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgRasterHelper::GetSqlRows"); }
bool MgRasterHelper::CloseSqlReader(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgRasterHelper::CloseSqlReader"); }

// Data
MgBatchPropertyCollection* MgRasterHelper::GetDataRows(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgRasterHelper::GetDataRows"); }
bool MgRasterHelper::CloseDataReader(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgRasterHelper::CloseDataReader"); }

// Raster
MgByteReader* MgRasterHelper::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName) 
{
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    if (NULL != (MgdFeatureReader*)m_freader)
        ret = m_freader->GetRaster(propName, xSize, ySize);
    else if (NULL != (MgdDataReader*)m_dreader)
        ret = m_dreader->GetRaster(propName, xSize, ySize);
    else if (NULL != (MgdGwsFeatureReader*)m_gwsReader)
        ret = m_gwsReader->GetRaster(propName, xSize, ySize);
    else
        throw new MgInvalidOperationException(L"MgRasterHelper::GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgRasterHelper::GetRaster")
    return ret.Detach();
}

MgPropertyDefinitionCollection* MgRasterHelper::GetIdentityProperties(MgResourceIdentifier* resource,
                                                              CREFSTRING schemaName,
                                                              CREFSTRING className) { NOT_IMPLEMENTED(L"MgRasterHelper::GetIdentityProperties"); }

MgClassDefinitionCollection* MgRasterHelper::GetIdentityProperties(MgResourceIdentifier* resource,
                                                           CREFSTRING schemaName,
                                                           MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgRasterHelper::GetIdentityProperties"); }

STRING MgRasterHelper::GetFdoCacheInfo() { NOT_IMPLEMENTED(L"MgRasterHelper::GetFdoCacheInfo"); }

MgClassDefinition* MgRasterHelper::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className,
                                              bool serialize) { NOT_IMPLEMENTED(L"MgRasterHelper::GetClassDefinition"); }

bool MgRasterHelper::CommitTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgRasterHelper::CommitTransaction"); }

bool MgRasterHelper::RollbackTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgRasterHelper::RollbackTransaction"); }

MgSqlDataReader* MgRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement,
                                         MgParameterCollection* parameters,
                                         MgTransaction* transaction,
                                         INT32 fetchSize) { NOT_IMPLEMENTED(L"MgRasterHelper::ExecuteSqlQuery"); }

//Add save point
STRING MgRasterHelper::AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName) { NOT_IMPLEMENTED(L"MgRasterHelper::AddSavePoint"); }

//Roll back to a save point
bool MgRasterHelper::RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgRasterHelper::RollbackSavePoint"); }

//Release a save point
bool MgRasterHelper::ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgRasterHelper::ReleaseSavePoint"); }