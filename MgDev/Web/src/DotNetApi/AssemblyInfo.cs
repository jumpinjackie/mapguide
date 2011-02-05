//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

using System;
using System.Reflection;
using System.Runtime.CompilerServices;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly: AssemblyTitle("MapGuide Dot Net Api Dynamic Link Library")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("Open Source Geospatial Foundation")]
[assembly: AssemblyProduct("MapGuide Open Source")]
[assembly: AssemblyCopyright("Copyright (C) 2006-2011 by Autodesk, Inc.")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]
[assembly: CLSCompliant(true)]

//
// We use the TypeForwardedToAttribute to keep backward compatibility of .NET API.
// It will forward call to the following types to new .NET API Assembly.
//
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeaturePropertyType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMimeType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPropertyType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerGroupType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServiceType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUser))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGroup))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRole))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureCommandType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureGeometricType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureSpatialOperations))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgObjectPropertyType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgOrderingOption))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRasterDataModelType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgReaderType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRepositoryType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceTag))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceDataName))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceDataType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourcePermission))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSpatialContextExtentType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateDimension))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryEntityType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometricPathInstructionType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryComponentType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemCodeFormat))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemErrorCode))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemGeodeticTransformationMethod))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemProjectionCode))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemProjectionFormatType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemProjectionLogicalType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemProjectionParameterType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemUnitCode))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemUnitType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemWktFlavor))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPageUnitsType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSpace))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMapPlotInstruction))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLogFileType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServerInformationProperties))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUnitType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConfigProperties))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebActions))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTargetType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTargetViewerType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTaskButtonType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebWidgetType))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLocalizer))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.ManagedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MapGuideApi))]
//[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MapGuideApiPINVOKE))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgObject))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDisposable))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGuardDisposable))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgAgfReaderWriter))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSerializable))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometricEntity))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometry))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgAggregateGeometry))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgApplicationException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryComponent))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveSegment))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgArcSegment))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSystemException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgOutOfRangeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgArgumentOutOfRangeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgArrayTypeMismatchException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgAuthenticationFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgBatchPropertyCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNamedSerializable))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNullableProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgBlobProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgBooleanProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgByteProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgByteReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgByteSink))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgByteSource))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgClassDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgClassDefinitionCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgClassNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgClobProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgColor))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConfigurationException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConfigurationLoadFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConfigurationSaveFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConnectionFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgConnectionNotOpenException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinate))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateIterator))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystem))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemCatalog))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemCategory))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemDictionaryBase))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemCategoryDictionary))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemComputationFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemConversionFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemDatum))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemDatumDictionary))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemDictionary))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemDictionaryUtility))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemEllipsoid))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemEllipsoidDictionary))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemEnum))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemEnumInteger32))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemFactory))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemFilter))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemFilterInteger32))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemFormatConverter))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemGeodeticTransformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemMathComparator))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMeasure))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemMeasure))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemMeasureFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemProjectionInformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgTransform))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemTransform))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemTransformFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateSystemUnitInformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateXY))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateXYM))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateXYZ))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCoordinateXYZM))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureSourceParams))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCreateSdfParams))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurve))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRegion))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurvePolygon))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurvePolygonCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRing))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveRing))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveRingCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveSegmentCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveString))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgCurveStringCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPropertyDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDataPropertyDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDataReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDateTime))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDateTimeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDateTimeProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgThirdPartyException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDbException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDbXmlException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDecryptionException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDeleteFeatures))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgIoException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFileIoException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDirectoryNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDisposableCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDivideByZeroException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDomainException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDoubleProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDrawingService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateDirectoryException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateFileException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateGroupException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateNameException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateObjectException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateParameterException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateRepositoryException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateResourceDataException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateResourceException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateRoleException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateServerException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateSessionException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDuplicateUserException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDwfException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDwfSectionNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDwfSectionResourceNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgDwfVersion))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgEmptyFeatureSetException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgEncryptionException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgStreamIoException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgEndOfStreamException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgEnvelope))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFdoException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureQueryOptions))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureAggregateOptions))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureCommandCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureInformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureSchema))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureSchemaCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFeatureServiceException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgFileNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometricPropertyDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryFactory))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGeometryTypeInfo))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGroupNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgGwsFeatureReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpHeader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpPrimitiveValue))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpRequest))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpRequestMetadata))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpRequestParam))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpResponse))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgHttpResult))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgIndexOutOfRangeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInsertFeatures))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInt16Property))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInt32Property))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInt64Property))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgIntCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidArgumentException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidCastException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidCoordinateSystemException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidCoordinateSystemTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidCoordinateSystemUnitsException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidDwfPackageException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidDwfSectionException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidFeatureSourceException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidIpAddressException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidLicenseException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidLogEntryException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidMapDefinitionException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidOperationException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidPasswordException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidPrintLayoutFontSizeUnitsException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidPrintLayoutPositionUnitsException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidPrintLayoutSizeUnitsException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidPropertyTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidRepositoryNameException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidRepositoryTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourceDataNameException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourceDataTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourceNameException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourcePathException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourcePreProcessingTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidResourceTypeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidServerNameException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgInvalidStreamHeaderException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgKmlService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerBase))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayer))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerGroup))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerGroupCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayerNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLayout))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLengthException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLicenseException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLicenseExpiredException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLinearRing))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLinearRingCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLinearSegment))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLineString))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLineStringCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLogicException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgLongTransactionReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResource))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMapBase))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMap))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMapCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMappingService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMapPlot))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMapPlotCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiCurvePolygon))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiCurveString))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiGeometry))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiLineString))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiPoint))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgMultiPolygon))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNotFiniteNumberException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNotImplementedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNullArgumentException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNullPropertyValueException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgNullReferenceException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgObjectNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgObjectPropertyDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgOperationProcessingException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgOutOfMemoryException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgOverflowException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPackageStatusInformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgParameterNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPathTooLongException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPlatformNotSupportedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPlotSpecification))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPoint))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPointCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPolygon))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPolygonCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPortNotAvailableException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPrintToScaleModeNotSelectedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPropertyCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgPropertyDefinitionCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRaster))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRasterProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRasterPropertyDefinition))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgReadOnlyLayerCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRenderingOptions))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRenderingService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRepositoryCreationFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRepositoryNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRepositoryNotOpenException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRepositoryOpenFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceBusyException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceDataNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceIdentifier))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourceService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourcesException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgResourcesLoadFailedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRoleNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgRuntimeException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSelectionBase))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSelection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServerAdmin))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServerNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServerNotOnlineException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServiceNotAvailableException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgServiceNotSupportedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSessionExpiredException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSessionNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSingleProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSite))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSiteConnection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSiteInfo))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSpatialContextReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgSqlDataReader))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgStringCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgStringProperty))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgStringPropertyCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgTemporaryFileNotAvailableException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgTileService))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUnauthorizedAccessException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUnclassifiedException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUnderflowException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUpdateFeatures))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUriFormatException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUserInformation))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgUserNotFoundException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWarnings))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebUiTargetCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebBufferCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebCommandCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebWidget))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebCommandWidget))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebWidgetCollection))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebContextMenu))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebFlyoutWidget))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebGetPrintablePageCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebHelpCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebUiPane))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebUiSizablePane))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebInformationPane))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebInvokeScriptCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebInvokeUrlCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebLayout))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebMeasureCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebPrintCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebSearchCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebSelectWithinCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebSeparatorWidget))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTaskBar))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTaskBarWidget))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebTaskPane))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebToolBar))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWebViewOptionsCommand))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgWktReaderWriter))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgXmlException))]
[assembly: TypeForwardedTo(typeof(OSGeo.MapGuide.MgXmlParserException))]

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly: AssemblyVersion("1.0.0.1")]
[assembly: AssemblyFileVersion("1.0.0.1")]
[assembly: AssemblyInformationalVersion("1.0.0.1")]

//
// In order to sign your assembly you must specify a key to use. Refer to the
// Microsoft .NET Framework documentation for more information on assembly signing.
//
// Use the attributes below to control which key is used for signing.
//
// Notes:
//   (*) If no key is specified, the assembly is not signed.
//   (*) KeyName refers to a key that has been installed in the Crypto Service
//       Provider (CSP) on your machine. KeyFile refers to a file which contains
//       a key.
//   (*) If the KeyFile and the KeyName values are both specified, the
//       following processing occurs:
//       (1) If the KeyName can be found in the CSP, that key is used.
//       (2) If the KeyName does not exist and the KeyFile does exist, the key
//           in the KeyFile is installed into the CSP and used.
//   (*) In order to create a KeyFile, you can use the sn.exe (Strong Name) utility.
//       When specifying the KeyFile, the location of the KeyFile should be
//       relative to the project output directory which is
//       %Project Directory%\obj\<configuration>. For example, if your KeyFile is
//       located in the project directory, you would specify the AssemblyKeyFile
//       attribute as [assembly: AssemblyKeyFile("..\\..\\mykey.snk")]
//   (*) Delay Signing is an advanced option - see the Microsoft .NET Framework
//       documentation for more information on this.
//
[assembly: AssemblyDelaySign(false)]