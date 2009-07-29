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

#include "Data/BlobProperty.cpp"
#include "Data/BooleanProperty.cpp"
#include "Data/ByteProperty.cpp"
#include "Data/ClobProperty.cpp"
#include "Data/Color.cpp"
#include "Data/DateTimeProperty.cpp"
#include "Data/DoubleProperty.cpp"
#include "Data/Int16Property.cpp"
#include "Data/Int32Property.cpp"
#include "Data/Int64Property.cpp"
#include "Data/IntCollection.cpp"
#include "Data/Point3D.cpp"
#include "Data/PropertyDefinitionCollection.cpp"
#include "Data/SerializableCollection.cpp"
#include "Data/SingleProperty.cpp"
#include "Data/StringPropertyCollection.cpp"
#include "Data/Vector3D.cpp"
#include "Exception/ArrayTypeMismatchException.cpp"
#include "Exception/DuplicateResourceDataException.cpp"
#include "Exception/DuplicateResourceException.cpp"
#include "Exception/EmptyFeatureSetException.cpp"
#include "Exception/FdoException.cpp"
#include "Exception/FeatureServiceException.cpp"
#include "Exception/InvalidMapDefinitionException.cpp"
#include "Exception/InvalidLayerDefinitionException.cpp"
#include "Exception/InvalidRepositoryNameException.cpp"
#include "Exception/InvalidRepositoryTypeException.cpp"
#include "Exception/InvalidResourceDataNameException.cpp"
#include "Exception/InvalidResourceDataTypeException.cpp"
#include "Exception/InvalidResourceNameException.cpp"
#include "Exception/InvalidResourcePathException.cpp"
#include "Exception/InvalidResourcePreProcessingTypeException.cpp"
#include "Exception/InvalidResourceTypeException.cpp"
#include "Exception/InvalidSymbolDefinitionException.cpp"
#include "Exception/LayerNotFoundException.cpp"
#include "Exception/ResourceBusyException.cpp"
#include "Exception/ResourceDataNotFoundException.cpp"
#include "Exception/ResourceNotFoundException.cpp"
#include "Exception/ServiceNotAvailableException.cpp"
#include "Exception/ServiceNotSupportedException.cpp"
#include "Exception/UserNotFoundException.cpp"
#include "MapLayer/ChangeList.cpp"
#include "MapLayer/LayerBase.cpp"
#include "MapLayer/LayerCollection.cpp"
#include "MapLayer/LayerGroup.cpp"
#include "MapLayer/LayerGroupCollection.cpp"
#include "MapLayer/MapBase.cpp"
#include "MapLayer/MapCollection.cpp"
#include "MapLayer/ObjectChange.cpp"
#include "MapLayer/ReadOnlyLayerCollection.cpp"
#include "MapLayer/SelectionBase.cpp"
#include "Services/BaseServiceDefs.cpp"
#include "Services/ClassDefinition.cpp"
#include "Services/ClassDefinitionCollection.cpp"
#include "Services/CreateSdfParams.cpp"
#include "Services/DataPropertyDefinition.cpp"
#include "Services/DataReader.cpp"
#include "Services/DeleteFeatures.cpp"
#include "Services/FeatureAggregateOptions.cpp"
#include "Services/FeatureCommandCollection.cpp"
#include "Services/FeatureDefs.cpp"
#include "Services/FeatureProperty.cpp"
#include "Services/FeatureQueryOptions.cpp"
#include "Services/FeatureReader.cpp"
#include "Services/FeatureSchema.cpp"
#include "Services/FeatureSchemaCollection.cpp"
#include "Services/FeatureService.cpp"
#include "Services/FeatureSet.cpp"
#include "Services/FeatureSourceParams.cpp"
#include "Services/GeometricPropertyDefinition.cpp"
#include "Services/GeometryProperty.cpp"
#include "Services/GeometryTypeInfo.cpp"
#include "Services/InsertFeatures.cpp"
#include "Services/LockFeatures.cpp"
#include "Services/LongTransactionData.cpp"
#include "Services/LongTransactionReader.cpp"
#include "Services/ObjectPropertyDefinition.cpp"
#include "Services/Raster.cpp"
#include "Services/RasterProperty.cpp"
#include "Services/RasterPropertyDefinition.cpp"
#include "Services/RasterTypes.cpp"
#include "Services/Resource.cpp"
#include "Services/ResourceDefs.cpp"
#include "Services/ResourceIdentifier.cpp"
#include "Services/ResourceService.cpp"
#include "Services/Service.cpp"
#include "Services/ServiceRegistry.cpp"
#include "Services/SpatialContextData.cpp"
#include "Services/SpatialContextReader.cpp"
#include "Services/SqlDataReader.cpp"
#include "Services/UnlockFeatures.cpp"
#include "Services/UpdateFeatures.cpp"
#include "Services/Warnings.cpp"
#include "Services/PrintLayoutService/MapView.cpp"
#include "Services/PrintLayoutService/MapViewportBase.cpp"
#include "Services/PrintLayoutService/PrintLayoutBase.cpp"
#include "Services/PrintLayoutService/PrintLayoutDefs.cpp"
#include "Services/PrintLayoutService/PrintLayoutElementBase.cpp"
#include "Services/PrintLayoutService/PrintLayoutElementCollection.cpp"
#include "Services/PrintLayoutService/PrintLayoutElementFactoryBase.cpp"
#include "Services/PrintLayoutService/PrintLayoutServiceBase.cpp"
#include "Services/PrintLayoutService/PropertyMapping.cpp"
#include "Services/PrintLayoutService/PropertyMappingCollection.cpp"
