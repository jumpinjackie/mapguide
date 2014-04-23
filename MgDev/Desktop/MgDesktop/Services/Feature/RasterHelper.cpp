#include "MgDesktop.h"
#include "RasterHelper.h"

MgdRasterHelper::MgdRasterHelper(MgdFeatureReader* reader) : MgFeatureService() 
{
    m_freader = SAFE_ADDREF(reader);
}

MgdRasterHelper::MgdRasterHelper(MgdDataReader* reader) : MgFeatureService() 
{
    m_dreader = SAFE_ADDREF(reader);
}

MgdRasterHelper::MgdRasterHelper(MgdGwsFeatureReader* reader) : MgFeatureService() 
{
    m_gwsReader = SAFE_ADDREF(reader);
}

MgByteReader* MgdRasterHelper::GetFeatureProviders() { NOT_IMPLEMENTED(L"MgdRasterHelper::GetFeatureProviders"); }

MgStringCollection* MgdRasterHelper::GetConnectionPropertyValues(CREFSTRING providerName,
                                                        CREFSTRING propertyName,
                                                        CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetConnectionPropertyValues"); }

bool MgdRasterHelper::TestConnection(CREFSTRING providerName, CREFSTRING connectionString) { NOT_IMPLEMENTED(L"MgdRasterHelper::TestConnection"); }

bool MgdRasterHelper::TestConnection(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgdRasterHelper::TestConnection"); }

MgByteReader* MgdRasterHelper::GetCapabilities(CREFSTRING providerName) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetCapabilities"); }

MgByteReader* MgdRasterHelper::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetCapabilities"); }

void MgdRasterHelper::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema) { NOT_IMPLEMENTED(L"MgdRasterHelper::ApplySchema"); }

MgFeatureSchemaCollection* MgdRasterHelper::DescribeSchema(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName,
                                                  MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeSchema"); }

MgFeatureSchemaCollection* MgdRasterHelper::DescribeSchema(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeSchema"); }

STRING MgdRasterHelper::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                   CREFSTRING schemaName,
                                   MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeSchemaAsXml"); }

STRING MgdRasterHelper::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                   CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeSchemaAsXml"); }

STRING MgdRasterHelper::SchemaToXml(MgFeatureSchemaCollection* schema) { NOT_IMPLEMENTED(L"MgdRasterHelper::SchemaToXml"); }

MgFeatureSchemaCollection* MgdRasterHelper::XmlToSchema(CREFSTRING xml) { NOT_IMPLEMENTED(L"MgdRasterHelper::XmlToSchema"); }

MgFeatureReader* MgdRasterHelper::SelectFeatures(MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options) { NOT_IMPLEMENTED(L"MgdRasterHelper::SelectFeatures"); }

MgFeatureReader* MgdRasterHelper::SelectFeatures(MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options,
                                        CREFSTRING coordinateSystem) { NOT_IMPLEMENTED(L"MgdRasterHelper::SelectFeatures"); }

MgDataReader* MgdRasterHelper::SelectAggregate(MgResourceIdentifier* resource,
                                      CREFSTRING className,
                                      MgFeatureAggregateOptions* options) { NOT_IMPLEMENTED(L"MgdRasterHelper::SelectAggregate"); }

MgPropertyCollection* MgdRasterHelper::UpdateFeatures(MgResourceIdentifier* resource,
                                             MgFeatureCommandCollection* commands,
                                             bool useTransaction) { NOT_IMPLEMENTED(L"MgdRasterHelper::UpdateFeatures"); }

MgPropertyCollection* MgdRasterHelper::UpdateFeatures(MgResourceIdentifier* resource,
                                             MgFeatureCommandCollection* commands,
                                             MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgdRasterHelper::UpdateFeatures"); }

MgFeatureReader* MgdRasterHelper::GetLockedFeatures(MgResourceIdentifier* resource,
                                           CREFSTRING className,
                                           MgFeatureQueryOptions* options) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetLockedFeatures"); }

MgTransaction* MgdRasterHelper::BeginTransaction(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgdRasterHelper::BeginTransaction"); }

MgSqlDataReader* MgdRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement) { NOT_IMPLEMENTED(L"MgdRasterHelper::ExecuteSqlQuery"); }

MgSqlDataReader* MgdRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement,
                                         MgParameterCollection* parameters,
                                         MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgdRasterHelper::ExecuteSqlQuery"); }

INT32 MgdRasterHelper::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                 CREFSTRING sqlNonSelectStatement) { NOT_IMPLEMENTED(L"MgdRasterHelper::ExecuteSqlNonQuery"); }

INT32 MgdRasterHelper::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                 CREFSTRING sqlNonSelectStatement,
                                 MgParameterCollection* parameters,
                                 MgTransaction* transaction) { NOT_IMPLEMENTED(L"MgdRasterHelper::ExecuteSqlNonQuery"); }

MgSpatialContextReader* MgdRasterHelper::GetSpatialContexts(MgResourceIdentifier* resource,
                                                   bool bActiveOnly) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetSpatialContexts"); }

MgLongTransactionReader* MgdRasterHelper::GetLongTransactions(MgResourceIdentifier* resource,
                                                     bool bActiveOnly) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetLongTransactions"); }

bool MgdRasterHelper::SetLongTransaction(MgResourceIdentifier* featureSourceId,
                                CREFSTRING longTransactionName) { NOT_IMPLEMENTED(L"MgdRasterHelper::SetLongTransaction"); }

MgStringCollection* MgdRasterHelper::GetSchemas(MgResourceIdentifier* resource) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetSchemas"); }

MgStringCollection* MgdRasterHelper::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetClasses"); }

MgClassDefinition* MgdRasterHelper::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetClassDefinition"); }

void MgdRasterHelper::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams) { NOT_IMPLEMENTED(L"MgdRasterHelper::CreateFeatureSource"); }

MgByteReader* MgdRasterHelper::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                             MgStringCollection* featureClasses) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeWfsFeatureType"); }

MgByteReader* MgdRasterHelper::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                             MgStringCollection* featureClasses,
                                             CREFSTRING namespacePrefix,
                                             CREFSTRING namespaceUrl) { NOT_IMPLEMENTED(L"MgdRasterHelper::DescribeWfsFeatureType"); }

MgByteReader* MgdRasterHelper::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                    CREFSTRING featureClass,
                                    MgStringCollection* requiredProperties,
                                    CREFSTRING srs,
                                    CREFSTRING filter,
                                    INT32 maxFeatures) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetWfsFeature"); }

MgByteReader* MgdRasterHelper::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                    CREFSTRING featureClass,
                                    MgStringCollection* requiredProperties,
                                    CREFSTRING srs,
                                    CREFSTRING filter,
                                    INT32 maxFeatures,
                                    CREFSTRING wfsVersion,
                                    CREFSTRING outputFormat,
                                    CREFSTRING sortCriteria,
                                    CREFSTRING namespacePrefix,
                                    CREFSTRING namespaceUrl) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetWfsFeature"); }

MgByteReader* MgdRasterHelper::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgdRasterHelper::EnumerateDataStores"); }

MgByteReader* MgdRasterHelper::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetSchemaMapping"); }



// Feature
MgBatchPropertyCollection* MgdRasterHelper::GetFeatures(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetFeatures"); }
bool MgdRasterHelper::CloseFeatureReader(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::CloseFeatureReader"); }

// SQL
MgBatchPropertyCollection* MgdRasterHelper::GetSqlRows(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetSqlRows"); }
bool MgdRasterHelper::CloseSqlReader(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::CloseSqlReader"); }

// Data
MgBatchPropertyCollection* MgdRasterHelper::GetDataRows(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetDataRows"); }
bool MgdRasterHelper::CloseDataReader(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdRasterHelper::CloseDataReader"); }

// Raster
MgByteReader* MgdRasterHelper::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName) 
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
        throw new MgInvalidOperationException(L"MgdRasterHelper::GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdRasterHelper::GetRaster")
    return ret.Detach();
}

MgPropertyDefinitionCollection* MgdRasterHelper::GetIdentityProperties(MgResourceIdentifier* resource,
                                                              CREFSTRING schemaName,
                                                              CREFSTRING className) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetIdentityProperties"); }

MgClassDefinitionCollection* MgdRasterHelper::GetIdentityProperties(MgResourceIdentifier* resource,
                                                           CREFSTRING schemaName,
                                                           MgStringCollection* classNames) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetIdentityProperties"); }

STRING MgdRasterHelper::GetFdoCacheInfo() { NOT_IMPLEMENTED(L"MgdRasterHelper::GetFdoCacheInfo"); }

MgClassDefinition* MgdRasterHelper::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className,
                                              bool serialize) { NOT_IMPLEMENTED(L"MgdRasterHelper::GetClassDefinition"); }

bool MgdRasterHelper::CommitTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdRasterHelper::CommitTransaction"); }

bool MgdRasterHelper::RollbackTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdRasterHelper::RollbackTransaction"); }

MgSqlDataReader* MgdRasterHelper::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement,
                                         MgParameterCollection* parameters,
                                         MgTransaction* transaction,
                                         INT32 fetchSize) { NOT_IMPLEMENTED(L"MgdRasterHelper::ExecuteSqlQuery"); }

//Add save point
STRING MgdRasterHelper::AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName) { NOT_IMPLEMENTED(L"MgdRasterHelper::AddSavePoint"); }

//Roll back to a save point
bool MgdRasterHelper::RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdRasterHelper::RollbackSavePoint"); }

//Release a save point
bool MgdRasterHelper::ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdRasterHelper::ReleaseSavePoint"); }