#ifndef DESKTOP_FEATURE_SERVICE_H
#define DESKTOP_FEATURE_SERVICE_H

#include "MgDesktop.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"

#include "Services/Feature/SpatialContextCacheItem.h"

class FdoIConnection;
class FdoFeatureSchemaCollection;
class MgResourceIdentifier;
class MgFeatureSchemaCollection;
class MgdResourceService;

class FdoFilter;
class FdoIBaseSelect;
class FdoICommandCapabilities;
class FdoIConnectionCapabilities;
class FdoIFilterCapabilities;
class FdoIGeometryCapabilities;
class FdoIExpressionCapabilities;
class FdoISchemaCapabilities;
class FdoIRasterCapabilities;
class FdoITopologyCapabilities;

namespace MdfModel
{
    class Extension;
}
/// \ingroup Desktop_Service_Module
/// \{
class MG_DESKTOP_API MgdFeatureService : public MgFeatureService
{
    DECLARE_CLASSNAME(MgdFeatureService)

INTERNAL_API:
	MgdFeatureService();

EXTERNAL_API:
	virtual ~MgdFeatureService();

protected:
	virtual void Dispose() { delete this; }

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

    //------- Desktop API additions --------//

    MgdScrollableFeatureReader* SelectFeaturesExtended(MgResourceIdentifier* resource,
                                                       CREFSTRING className,
                                                       MgFeatureQueryOptions* options);

    void RegisterProvider(CREFSTRING providerLibraryPath);

    void UnregisterProvider(CREFSTRING providerName);

	MgFeatureReader* InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues);

    MgFeatureReader* InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, MgTransaction* trans);

    MgPropertyCollection* InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues);

    MgPropertyCollection* InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues, MgTransaction* trans);

	int UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter);

    int UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans);

	int DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter);

	int DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter, MgTransaction* trans);

EXTERNAL_API:
    void PurgeCache(MgResourceIdentifier* resource);

    MgByteReader* QueryCacheInfo();

INTERNAL_API:

    //Add save point
    virtual STRING AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName);

    //Roll back to a save point
    virtual bool RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName);

    //Release a save point
    virtual bool ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName);

    virtual MgPropertyDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                                  CREFSTRING schemaName,
                                                                  CREFSTRING className);

    virtual MgClassDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                               CREFSTRING schemaName,
                                                               MgStringCollection* classNames);

    // Feature
    virtual MgBatchPropertyCollection* GetFeatures(CREFSTRING featureReader);
    virtual bool CloseFeatureReader(CREFSTRING featureReader);

    // SQL
    virtual MgBatchPropertyCollection* GetSqlRows(CREFSTRING sqlReader);
    virtual bool CloseSqlReader(CREFSTRING sqlReader);

    // Data
    virtual MgBatchPropertyCollection* GetDataRows(CREFSTRING dataReader);
    virtual bool CloseDataReader(CREFSTRING dataReader);

    // Raster
    virtual MgByteReader* GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName);

    virtual STRING GetFdoCacheInfo();

    virtual MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
                                                      CREFSTRING schemaName,
                                                      MgStringCollection* classNames,
                                                      bool serialize);

    virtual MgClassDefinition* GetClassDefinition(MgResourceIdentifier* resource,
                                                  CREFSTRING schemaName,
                                                  CREFSTRING className,
                                                  bool serialize);

    // Commit the transaction specified by the transaction id.
    virtual bool CommitTransaction(CREFSTRING transactionId);

    // Rollback the transaction specified by the transaction id.
    virtual bool RollbackTransaction(CREFSTRING transactionId);

    virtual MgSqlDataReader* ExecuteSqlQuery(MgResourceIdentifier* resource,
                                             CREFSTRING sqlStatement,
                                             MgParameterCollection* parameters,
                                             MgTransaction* transaction,
                                             INT32 fetchSize);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_FeatureService_FeatureService;

protected:
    virtual INT32 GetClassId() { return m_cls_id; }
};
/// \}
#endif