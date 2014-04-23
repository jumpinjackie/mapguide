#ifndef DESKTOP_RASTER_HELPER_H
#define DESKTOP_RASTER_HELPER_H

class MgdGwsFeatureReader;
template class Ptr<MgdFeatureReader>;

class MgdRasterHelper : public MgFeatureService
{
PUBLISHED_API:
    virtual MgByteReader* GetFeatureProviders();

    virtual MgStringCollection* GetConnectionPropertyValues(CREFSTRING providerName,
                                                            CREFSTRING propertyName,
                                                            CREFSTRING partialConnString);

    virtual bool TestConnection(CREFSTRING providerName, CREFSTRING connectionString);

    virtual bool TestConnection(MgResourceIdentifier* resource);

    virtual MgByteReader* GetCapabilities(CREFSTRING providerName);

    virtual MgByteReader* GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString);

    virtual void ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema);

    virtual MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
                                                      CREFSTRING schemaName,
                                                      MgStringCollection* classNames);

    virtual MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
                                                      CREFSTRING schemaName);

    virtual STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                       CREFSTRING schemaName,
                                       MgStringCollection* classNames);

    virtual STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                       CREFSTRING schemaName);

    virtual STRING SchemaToXml(MgFeatureSchemaCollection* schema);

    virtual MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);

    virtual MgFeatureReader* SelectFeatures(MgResourceIdentifier* resource,
                                            CREFSTRING className,
                                            MgFeatureQueryOptions* options);

    virtual MgFeatureReader* SelectFeatures(MgResourceIdentifier* resource,
                                            CREFSTRING className,
                                            MgFeatureQueryOptions* options,
                                            CREFSTRING coordinateSystem);

    virtual MgDataReader* SelectAggregate(MgResourceIdentifier* resource,
                                          CREFSTRING className,
                                          MgFeatureAggregateOptions* options);

    virtual MgPropertyCollection* UpdateFeatures(MgResourceIdentifier* resource,
                                                 MgFeatureCommandCollection* commands,
                                                 bool useTransaction);

    virtual MgPropertyCollection* UpdateFeatures(MgResourceIdentifier* resource,
                                                 MgFeatureCommandCollection* commands,
                                                 MgTransaction* transaction);

    virtual MgFeatureReader* GetLockedFeatures(MgResourceIdentifier* resource,
                                               CREFSTRING className,
                                               MgFeatureQueryOptions* options);

    virtual MgTransaction* BeginTransaction(MgResourceIdentifier* resource);

    virtual MgSqlDataReader* ExecuteSqlQuery(MgResourceIdentifier* resource,
                                             CREFSTRING sqlStatement);

    virtual MgSqlDataReader* ExecuteSqlQuery(MgResourceIdentifier* resource,
                                             CREFSTRING sqlStatement,
                                             MgParameterCollection* parameters,
                                             MgTransaction* transaction);

    virtual INT32 ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                     CREFSTRING sqlNonSelectStatement);

    virtual INT32 ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                     CREFSTRING sqlNonSelectStatement,
                                     MgParameterCollection* parameters,
                                     MgTransaction* transaction);

    virtual MgSpatialContextReader* GetSpatialContexts(MgResourceIdentifier* resource,
                                                       bool bActiveOnly);

    virtual MgLongTransactionReader* GetLongTransactions(MgResourceIdentifier* resource,
                                                         bool bActiveOnly);

    virtual bool SetLongTransaction(MgResourceIdentifier* featureSourceId,
                                    CREFSTRING longTransactionName);

    virtual MgStringCollection* GetSchemas(MgResourceIdentifier* resource);

    virtual MgStringCollection* GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName);

    virtual MgClassDefinition* GetClassDefinition(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName,
                                                  CREFSTRING className);

    virtual void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    virtual MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                 MgStringCollection* featureClasses);

    virtual MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                 MgStringCollection* featureClasses,
                                                 CREFSTRING namespacePrefix,
                                                 CREFSTRING namespaceUrl);

    virtual MgByteReader* GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                        CREFSTRING featureClass,
                                        MgStringCollection* requiredProperties,
                                        CREFSTRING srs,
                                        CREFSTRING filter,
                                        INT32 maxFeatures);

    virtual MgByteReader* GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                        CREFSTRING featureClass,
                                        MgStringCollection* requiredProperties,
                                        CREFSTRING srs,
                                        CREFSTRING filter,
                                        INT32 maxFeatures,
                                        CREFSTRING wfsVersion,
                                        CREFSTRING outputFormat,
                                        CREFSTRING sortCriteria,
                                        CREFSTRING namespacePrefix,
                                        CREFSTRING namespaceUrl);

    virtual MgByteReader* EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString);

    virtual MgByteReader* GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString);
    
INTERNAL_API:

    MgdRasterHelper(MgdFeatureReader* reader);
    MgdRasterHelper(MgdDataReader* reader);
    MgdRasterHelper(MgdGwsFeatureReader* reader);

    virtual ~MgdRasterHelper() {}

    // Feature
    virtual MgBatchPropertyCollection* GetFeatures(CREFSTRING featureReader);
    virtual bool CloseFeatureReader(CREFSTRING featureReader);

    // SQL
    virtual MgBatchPropertyCollection* GetSqlRows(CREFSTRING sqlReader);
    virtual bool CloseSqlReader(CREFSTRING sqlReader);

    // Data
    virtual MgBatchPropertyCollection* GetDataRows(CREFSTRING dataReader);
    virtual bool CloseDataReader(CREFSTRING dataReader);

    //
    // This is the only method we implement here as it is the method called by MgRaster
    //
    virtual MgByteReader* GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName);

    virtual MgPropertyDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                                  CREFSTRING schemaName,
                                                                  CREFSTRING className);

    virtual MgClassDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                               CREFSTRING schemaName,
                                                               MgStringCollection* classNames);

    virtual STRING GetFdoCacheInfo();

    virtual MgClassDefinition* GetClassDefinition(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName,
                                                  CREFSTRING className,
                                                  bool serialize);

    virtual bool CommitTransaction(CREFSTRING transactionId);

    virtual bool RollbackTransaction(CREFSTRING transactionId);

    virtual MgSqlDataReader* ExecuteSqlQuery(MgResourceIdentifier* resource,
                                             CREFSTRING sqlStatement,
                                             MgParameterCollection* parameters,
                                             MgTransaction* transaction,
                                             INT32 fetchSize);

    //Add save point
    virtual STRING AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName);

    //Roll back to a save point
    virtual bool RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName);

    //Release a save point
    virtual bool ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName);

protected:

    virtual void Dispose() { delete this; }

private:
    Ptr<MgdGwsFeatureReader> m_gwsReader;
    Ptr<MgdFeatureReader> m_freader;
    Ptr<MgdDataReader> m_dreader;

};

#endif