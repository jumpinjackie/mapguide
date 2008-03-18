AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerFeatureService.vcproj ServerFeatureService.rc

prefix=@prefix@/server

INCLUDES = \
  -I$(map_fdo_include) \
  -I$(map_fdo_include)/ExpressionEngine \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../Gws/Include \
  -I../../Gws/GwsCommon/inc \
  -I../../Gws/GwsQueryEngine/inc \
  -I../../../../Common/Security \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I../../../../Common/Stylization \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Cache \
  -I../../Common/Manager \
  -I../../Common/ServerUtil 

lib_LTLIBRARIES = libMgServerFeatureService.la

libMgServerFeatureService_la_SOURCES = \
  ServerFeatureServiceBuild.cpp

include_SOURCES = \
  FeatureOperation.cpp \
  FeatureOperationFactory.cpp \
  FeatureServiceHandler.cpp \
  GwsConnectionPool.cpp \
  OpCloseGwsFeatureReader.cpp \
  OpCreateFeatureSource.cpp \
  OpDescribeSchema.cpp \
  OpDescribeSchemaAsXml.cpp \
  OpExecuteSqlNonQuery.cpp \
  OpExecuteSqlQuery.cpp \
  OpGetCapabilities.cpp \
  OpGetConnectionPropertyValues.cpp \
  OpGetFdoCacheInfo.cpp \
  OpGetFeatureProviders.cpp \
  OpGetIdentityProperties.cpp \
  OpGetSpatialContexts.cpp \
  OpSelectFeatures.cpp \
  OpSelectFeaturesSpatial.cpp \
  OpTestConnection.cpp \
  OpTestFeatureSourceConnection.cpp \
  OpUpdateFeatures.cpp \
  ServerDescribeSchema.cpp \
  ServerFeatureConnection.cpp \
  ServerFeatureService.cpp \
  ServerGetFeatureProviders.cpp \
  ServerGetProviderCapabilities.cpp \
  ServerFeatureReaderIdentifier.cpp \
  ServerGetFeatures.cpp \
  ServerGwsFeatureReader.cpp \
  ServerGwsGetFeatures.cpp \
  ServerSelectFeatures.cpp \
  OpCloseFeatureReader.cpp \
  OpGetFeatures.cpp \
  ServerFeatureReader.cpp \
  ServerFdoFeatureReader.cpp \
  ServerFeatureReaderIdentifierPool.cpp \
  OpGetLongTransactions.cpp \
  ServerGetConnectionPropertyValues.cpp \
  ServerGetLongTransactions.cpp \
  ServerGetSpatialContexts.cpp \
  OpGetClassDefinition.cpp \
  OpGetClasses.cpp \
  OpGetSchemas.cpp \
  OpGetRaster.cpp \
  OpCloseSqlReader.cpp \
  OpGetSqlRows.cpp \
  ServerDataReaderPool.cpp \
  ServerFeatureUtil.cpp \
  ServerSqlCommand.cpp \
  ServerSqlDataReader.cpp \
  ServerSqlProcessor.cpp \
  FeatureServiceCommand.cpp \
  FeatureDistribution.cpp \
  OpCloseDataReader.cpp \
  OpGetDataRows.cpp \
  SelectAggregateCommand.cpp \
  SelectCommand.cpp \
  ServerCreateFeatureSource.cpp \
  ServerDataProcessor.cpp \
  ServerDataReader.cpp \
  ServerSqlDataReaderPool.cpp \
  FeatureNumericFunctions.cpp \
  FeatureStringFunctions.cpp \
  FeatureGeometricFunctions.cpp \
  FeatureManipulationCommand.cpp \
  ServerDeleteCommand.cpp \
  ServerInsertCommand.cpp \
  ServerUpdateCommand.cpp \
  ServerUpdateFeatures.cpp \
  OpSchemaToXml.cpp \
  OpXmlToSchema.cpp \
  ByteSourceRasterStreamImpl.cpp \
  OpDescribeWfsFeatureType.cpp \
  OpGetWfsFeature.cpp \
  FilterUtil.cpp \
  OpEnumerateDataStores.cpp \
  OpGetSchemaMapping.cpp \
  ServerEnumerateDataStores.cpp \
  ServerGetSchemaMapping.cpp \
  OpSetLongTransaction.cpp \
  MgCSTrans.cpp \
  TransformCache.cpp \
  JoinFeatureReader.cpp

noinst_HEADERS = $(include_SOURCES) \
  FeatureOperation.h \
  FeatureOperationFactory.h \
  FeatureServiceHandler.h \
  ServerFeatureService.h \
  GwsConnectionPool.h \
  OpCloseGwsFeatureReader.h \
  OpCreateFeatureSource.h \
  OpDescribeSchema.h \
  OpDescribeSchemaAsXml.h \
  OpExecuteSqlNonQuery.h \
  OpExecuteSqlQuery.h \
  OpGetCapabilities.h \
  OpGetConnectionPropertyValues.h \
  OpGetFdoCacheInfo.h \
  OpGetFeatureProviders.h \
  OpGetSpatialContexts.h \
  OpGetIdentityProperties.h \
  OpSelectFeatures.h \
  OpSelectFeaturesSpatial.h \
  OpTestConnection.h \
  OpTestFeatureSourceConnection.h \
  OpUpdateFeatures.h \
  ServerDescribeSchema.h \
  ServerFeatureConnection.h \
  ServerFeatureDllExport.h \
  ServerFeatureService.h \
  ServerFeatureServiceDefs.h \
  ServerGetFeatureProviders.h \
  ServerGetProviderCapabilities.h \
  ../../Common/stdafx.h \
  ServerGetFeatures.h \
  ServerGwsFeatureReader.h \
  ServerGwsGetFeatures.h \
  ServerFeatureReaderIdentifier.h \
  ServerSelectFeatures.h \
  OpCloseFeatureReader.h \
  OpGetFeatures.h \
  ServerFeatureReader.h \
  ServerFdoFeatureReader.h \
  ServerFeatureReaderIdentifierPool.h \
  OpGetLongTransactions.h \
  ServerGetConnectionPropertyValues.h \
  ServerGetLongTransactions.h \
  ServerGetSpatialContexts.h \
  OpGetClassDefinition.h \
  OpGetClasses.h \
  OpGetSchemas.h \
  OpGetRaster.h \
  OpCloseSqlReader.h \
  OpGetSqlRows.h \
  ServerDataReaderPool.h \
  ServerFeatureUtil.h \
  ServerSqlCommand.h \
  ServerSqlDataReader.h \
  ServerSqlProcessor.h \
  FeatureServiceCommand.h \
  FeatureDistribution.h \
  OpCloseDataReader.h \
  OpGetDataRows.h \
  SelectAggregateCommand.h \
  SelectCommand.h \
  ServerCreateFeatureSource.h \
  ServerDataProcessor.h \
  ServerDataReader.h \
  ServerSqlDataReaderPool.h \
  Matrix.h \
  DataReaderCreator.h \
  DoubleDataReaderCreator.h \
  FeatureNumericFunctions.h \
  FeatureStringFunctions.h \
  ByteDataReaderCreator.h \
  Int16DataReaderCreator.h \
  Int32DataReaderCreator.h \
  Int64DataReaderCreator.h \
  SingleDataReaderCreator.h \
  StringDataReaderCreator.h \
  UniqueFunction.h \
  FeatureGeometricFunctions.h \
  GeometryDataReaderCreator.h \
  ServerFeatureProcessor.h \
  ServerFeatureServiceExceptionDef.h \
  FeatureManipulationCommand.h \
  ServerDeleteCommand.h \
  ServerInsertCommand.h \
  ServerUpdateCommand.h \
  ServerUpdateFeatures.h \
  OpSchemaToXml.h \
  OpXmlToSchema.h \
  BooleanDataReaderCreator.h \
  ByteSourceRasterStreamImpl.h \
  DateTimeDataReaderCreator.h \
  OpDescribeWfsFeatureType.h \
  OpGetWfsFeature.h \
  FilterUtil.h \
  OpEnumerateDataStores.h \
  OpGetSchemaMapping.h \
  ServerEnumerateDataStores.h \
  ServerGetSchemaMapping.h \
  OpSetLongTransaction.h \
  MgCSTrans.h \
  TransformCache.h \
  JoinFeatureReader.h
  

libMgServerFeatureService_la_LIBADD = -lMgGwsCommon -lMgGwsQueryEngine

libMgServerFeatureService_la_LDFLAGS = -version-info 7:0:0 \
  -L../../Gws/GwsCommon \
  -L../../Gws/GwsQueryEngine
