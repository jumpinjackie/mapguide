//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGPLATFORMBASE_H_
#define MGPLATFORMBASE_H_

#include "GeometryCommon.h"

// PlatformBase exports
#ifdef _WIN32
#ifdef PLATFORMBASE_EXPORTS
#    define MG_PLATFORMBASE_API __declspec(dllexport)
#else
#    define MG_PLATFORMBASE_API __declspec(dllimport)
#endif
#else
#define MG_PLATFORMBASE_API
#endif

// Data types and Properties
#include "Services/PlatformBaseClassId.h"
#include "Data/BlobProperty.h"
#include "Data/BooleanProperty.h"
#include "Data/ByteProperty.h"
#include "Data/ClobProperty.h"
#include "Data/Color.h"
#include "Data/DateTimeProperty.h"
#include "Data/DoubleProperty.h"
#include "Data/Int16Property.h"
#include "Data/Int32Property.h"
#include "Data/Int64Property.h"
#include "Data/IntCollection.h"
#include "Data/Point3D.h"
#include "Data/PropertyDefinitionCollection.h"
#include "Data/SerializableCollection.h"
#include "Data/SingleProperty.h"
#include "Data/StringPropertyCollection.h"
#include "Data/Vector3D.h"

// Exceptions
#include "Exception/ArrayTypeMismatchException.h"
#include "Exception/DuplicateResourceDataException.h"
#include "Exception/DuplicateResourceException.h"
#include "Exception/EmptyFeatureSetException.h"
#include "Exception/FdoException.h"
#include "Exception/FeatureServiceException.h"
#include "Exception/InvalidLayerDefinitionException.h"
#include "Exception/InvalidMapDefinitionException.h"
#include "Exception/InvalidRepositoryNameException.h"
#include "Exception/InvalidRepositoryTypeException.h"
#include "Exception/InvalidResourceDataNameException.h"
#include "Exception/InvalidResourceDataTypeException.h"
#include "Exception/InvalidResourceNameException.h"
#include "Exception/InvalidResourcePathException.h"
#include "Exception/InvalidResourcePreProcessingTypeException.h"
#include "Exception/InvalidResourceTypeException.h"
#include "Exception/InvalidSymbolDefinitionException.h"
#include "Exception/LayerNotFoundException.h"
#include "Exception/ResourceBusyException.h"
#include "Exception/ResourceDataNotFoundException.h"
#include "Exception/ResourceNotFoundException.h"
#include "Exception/ServiceNotAvailableException.h"
#include "Exception/ServiceNotSupportedException.h"
#include "Exception/UserNotFoundException.h"

// Predeclare certain headers required by headers below
#include "Services/ResourceDefs.h"
#include "Services/ResourceIdentifier.h"
#include "Services/Warnings.h"
#include "Services/FeatureSourceParams.h"
#include "Services/FeatureSchema.h"
#include "Services/ReaderType.h"
#include "Services/Reader.h"
#include "Services/FeatureCommand.h"
#include "Services/FeatureCommandType.h"
#include "Services/FeatureQueryOptions.h"

#include "Services/BaseService.h"
#include "Services/BaseServiceDefs.h"
#include "Services/ClassDefinition.h"
#include "Services/ClassDefinitionCollection.h"
#include "Services/CreateSdfParams.h"
#include "Services/DataPropertyDefinition.h"
#include "Services/DataReader.h"
#include "Services/DeleteFeatures.h"
#include "Services/FeatureAggregateOptions.h"
#include "Services/FeatureCommandCollection.h"
#include "Services/FeatureDefs.h"
#include "Services/FeatureGeometricType.h"
#include "Services/FeatureProperty.h"
#include "Services/FeatureReader.h"
#include "Services/FeatureSchemaCollection.h"
#include "Services/FeatureService.h"
#include "Services/FeatureServiceCommon.h"
#include "Services/FeatureSet.h"
#include "Services/FeatureSpatialOperations.h"
#include "Services/GeometricPropertyDefinition.h"
#include "Services/GeometryProperty.h"
#include "Services/GeometryTypeInfo.h"
#include "Services/GwsFeatureReader.h"
#include "Services/InsertFeatures.h"
#include "Services/LongTransactionData.h"
#include "Services/LongTransactionReader.h"
#include "Services/ObjectPropertyDefinition.h"
#include "Services/ObjectPropertyType.h"
#include "Services/OrderingOption.h"
#include "Services/Raster.h"
#include "Services/RasterProperty.h"
#include "Services/RasterPropertyDefinition.h"
#include "Services/RasterTypes.h"
#include "Services/Resource.h"
#include "Services/ResourceService.h"
#include "Services/Service.h"
#include "Services/ServiceRegistry.h"
#include "Services/SpatialContextData.h"
#include "Services/SpatialContextExtentType.h"
#include "Services/SpatialContextReader.h"
#include "Services/SqlDataReader.h"
#include "Services/UpdateFeatures.h"
#include "Services/LockFeatures.h"
#include "Services/UnlockFeatures.h"

#include "Services/PrintLayoutService/MapView.h"
#include "Services/PrintLayoutService/MapViewportBase.h"
#include "Services/PrintLayoutService/PrintLayoutBase.h"
#include "Services/PrintLayoutService/PrintLayoutDefs.h"
#include "Services/PrintLayoutService/PrintLayoutElementBase.h"
#include "Services/PrintLayoutService/PrintLayoutElementDefinitionBase.h"
#include "Services/PrintLayoutService/PrintLayoutServiceBase.h"
#include "Services/PrintLayoutService/PropertyMapping.h"
#include "Services/PrintLayoutService/PropertyMappingCollection.h"

#include "MapLayer/LayerType.h"
#include "MapLayer/LayerGroupType.h"
#include "MapLayer/LayerBase.h"
#include "MapLayer/LayerCollection.h"
#include "MapLayer/LayerGroup.h"
#include "MapLayer/LayerGroupCollection.h"
#include "MapLayer/MapBase.h"
#include "MapLayer/MapCollection.h"
#include "MapLayer/ReadOnlyLayerCollection.h"
#include "MapLayer/SelectionBase.h"

#endif
