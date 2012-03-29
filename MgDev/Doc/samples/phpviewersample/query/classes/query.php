<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

?>

<?php

require_once('property.php');
require_once('feature.php');
require_once '../JSON.php';

class Query
{
	private $args = null;
	private $site = null;

	public $numOperators;
	public $numExpressions;
	public $strOperators;
	public $strExpressions;

	function __construct($args)
	{
		$this->args = $args;
		$this->site = new MgSiteConnection();
		$this->site->Open(new MgUserInformation($args['SESSION']));

		$this->numOperators = array('Equal to', 'Not equal to', 'Greater than', 'Greater than or equal to', 'Less than', 'Less than or equal to');
		$this->numExpressions = array(' = %s', ' != %s', ' > %s', ' >= %s', ' < %s', ' <= %s');

		$this->strOperators = array('Begins with', 'Contains', 'Equal to');
		$this->strExpressions = array(" like '%s%%'", " like '%%%s%%'", " = '%s'");
	}

    function GetMapLayerNames()
    {
        $layerNames = array();

		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
        $layers = $map->GetLayers();

        for ($i = 0; $i < $layers->GetCount(); $i++)
        {
            $layer = $layers->GetItem($i);

            //TODO: Exclude Raster and Drawing Layers???

            if((substr($layer->GetName(), 0, 1) != "_") && (substr(strtoupper($layer->GetFeatureSourceId()), 0, 7) != "SESSION"))
		    {
		        $layerNames[$layer->GetName()] = $layer->GetLegendLabel();
		    }
        }
        asort($layerNames);

        return $layerNames;
    }

    function GetLayerProperties()
    {
        $properties = array();

		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
        $layer = $map->GetLayers()->GetItem($this->args['LAYERNAME']);

		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
        $resId = new MgResourceIdentifier($layer->GetFeatureSourceId());
        $schemaClass = explode(':', $layer->GetFeatureClassName());

        $classDef = $featureService->GetClassDefinition($resId, $schemaClass[0], $schemaClass[1]);

        for ($i = 0; $i < $classDef->GetProperties()->GetCount(); $i++)
        {
            $propertyDef = $classDef->GetProperties()->GetItem($i);

            if ($propertyDef->GetPropertyType() == MgFeaturePropertyType::DataProperty)
            {
				$dataType = $propertyDef->GetDataType();
				if ($this->IsValidDataType($dataType))
				{
	                array_push($properties, new Property($propertyDef->GetName(), $dataType == MgPropertyType::String));
				}
            }
        }
        return $properties;
    }

	function ToggleSpatialFilter()
	{
		$result = true;

		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);

		$layers = $map->GetLayers();
		if ($layers->Contains('_QuerySpatialFilter'))
		{
			$layer = $layers->GetItem('_QuerySpatialFilter');
			if ($this->args['VISIBLE'] == 'true')
				$layer->SetVisible(true);
			else
				$layer->SetVisible(false);

			$map->Save($resourceService);
		}

		return $result;
	}

	function ShowSpatialFilter()
	{
		$result = true;

		$sdfResId = new MgResourceIdentifier('Session:' . $this->args['SESSION'] . '//Filter.FeatureSource');

		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);

        $updateCommands = new MgFeatureCommandCollection();

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);

		$layer = null;
		$layers = $map->GetLayers();
		if ($layers->Contains('_QuerySpatialFilter'))
		{
			$layer = $layers->GetItem('_QuerySpatialFilter');
            $updateCommands->Add(new MgDeleteFeatures('Filter', "ID like '%'"));
		}
		else
		{
			// Create the Feature Source (SDF)

			$sdfSchema = $this->CreateFilterSchema();
			$sdfParams = new MgCreateSdfParams('MAPCS', $map->GetMapSRS(), $sdfSchema);
			$featureService->CreateFeatureSource($sdfResId, $sdfParams);

			// Create the Layer

			$layerResId = new MgResourceIdentifier('Session:' . $this->args['SESSION'] . '//Filter.LayerDefinition');

	        $layerDefinition = file_get_contents("templates/filterlayerdefinition.xml");
    	    $layerDefinition = sprintf($layerDefinition, $sdfResId->ToString());

			$byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
			$resourceService->SetResource($layerResId, $byteSource->GetReader(), null);

			$layer = new MgLayer($layerResId, $resourceService);

			$layer->SetName('_QuerySpatialFilter');
			$layer->SetLegendLabel('_QuerySpatialFilter');
			$layer->SetDisplayInLegend(false);
			$layer->SetSelectable(false);

			$layers->Insert(0, $layer);
		}

		// Make the layer visible

		$layer->SetVisible(true);
		$map->Save($resourceService);

		// Add the geometry to the filter feature source

		$polygon = $this->CreatePolygonFromGeomText($this->args['GEOMTEXT']);
		$agfWriter = new MgAgfReaderWriter();
		$byteReader = $agfWriter->Write($polygon);

		$propertyValues = new MgPropertyCollection();
		$propertyValues->Add(new MgGeometryProperty('Geometry', $byteReader));

		$updateCommands->Add(new MgInsertFeatures('Filter', $propertyValues));

		$featureService->UpdateFeatures($sdfResId, $updateCommands, false);

		return $result;
	}

	function Execute()
	{
		$result = array();

		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
        $layer = $map->GetLayers()->GetItem($this->args['LAYERNAME']);

		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
        $resId = new MgResourceIdentifier($layer->GetFeatureSourceId());
		$featureClass = $layer->GetFeatureClassName();
		$featureGeometry = $layer->GetFeatureGeometryName();

		// Initialize the coordinate system transform

		$schemaAndClass = explode(":", $featureClass);
		$classDef = $featureService->GetClassDefinition($resId, $schemaAndClass[0], $schemaAndClass[1]);
		$geomProp = $classDef->GetProperties()->GetItem($featureGeometry);
		$spatialContext = $geomProp->GetSpatialContextAssociation();

		$csTransform = null;
		$csInverseTransform = null;
		$coordSysFactory = new MgCoordinateSystemFactory();

		$scReader = $featureService->GetSpatialContexts($resId, false);
		while ($scReader->ReadNext() && $csTransform == null)
		{
			if ($scReader->GetName() == $spatialContext)
			{
				$source = $coordSysFactory->Create($scReader->GetCoordinateSystemWkt());
				$target = $coordSysFactory->Create($map->GetMapSRS());
				$csTransform = new MgCoordinateSystemTransform($source, $target);
				$csInverseTransform = new MgCoordinateSystemTransform($target, $source);
			}
		}
		$scReader->Close();

		// Execute the query

		$queryMax = (int) $this->args['QUERYMAX'];
		$queryOptions = new MgFeatureQueryOptions();

		if ($this->args['USEPROPERTYFILTER'] == 'true')
		{
			$propertyFilter = $this->args['PROPERTYNAME'];
			if ($this->args['ISSTRING'] == 'true')
				$propertyFilter .= sprintf($this->strExpressions[$this->args['OPERATOR']], $this->args['VALUE']);
			else
				$propertyFilter .= sprintf($this->numExpressions[$this->args['OPERATOR']], $this->args['VALUE']);

			$queryOptions->SetFilter($propertyFilter);
		}

		if ($this->args['USESPATIALFILTER'] == 'true')
		{
			$polygon = $this->CreatePolygonFromGeomText($this->args['GEOMTEXT']);
			$polygon = $polygon->Transform($csInverseTransform);

			$queryOptions->SetSpatialFilter($featureGeometry, $polygon, MgFeatureSpatialOperations::Intersects);
		}

		$count = 0;
		$geometryReaderWriter = new MgAgfReaderWriter();
		$featureReader = $featureService->SelectFeatures($resId, $layer->GetFeatureClassName(), $queryOptions);
		while ($featureReader->ReadNext() && ($queryMax <= 0 || $count < $queryMax))
		{
			$displayValue = $this->GetFeaturePropertyValue($featureReader, $this->args['OUTPUTPROPERTY']);

			$byteReader = $featureReader->GetGeometry($featureGeometry);
            $geometry = $geometryReaderWriter->Read($byteReader);
            $centerPoint = $geometry->GetCentroid();
			$centerPoint = $centerPoint->Transform($csTransform);

			$idList = $this->GetFeatureIdList($map, $layer, $featureReader);

			array_push($result, new Feature($displayValue, $centerPoint, $idList));
			$count++;
		}

		return $result;
	}

	function GetSelectionXML()
	{
	    $json = new Services_JSON();
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);

		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
		$layer = $map->GetLayers()->GetItem($this->args['LAYERNAME']);
        $resId = new MgResourceIdentifier($layer->GetFeatureSourceId());
		$featureClass = $layer->GetFeatureClassName();
		$schemaAndClass = explode(":", $featureClass);
		$classDef = $featureService->GetClassDefinition($resId, $schemaAndClass[0], $schemaAndClass[1]);

		$properties = new MgPropertyCollection();
		$idList = $json->decode($this->args['IDLIST']);

		foreach ($idList as $key => $value)
		{
			switch($classDef->GetProperties()->GetItem($key)->GetDataType())
			{
				case MgPropertyType::Boolean :
					$properties->Add(new MgBooleanProperty($key, $value));
					break;

				case MgPropertyType::Byte :
					$properties->Add(new MgByteProperty($key, $value));
					break;

				case MgPropertyType::Single :
					$properties->Add(new MgSingleProperty($key, $value));
					break;

				case MgPropertyType::Double :
					$properties->Add(new MgDoubleProperty($key, $value));
					break;

				case MgPropertyType::Int16 :
					$properties->Add(new MgInt16Property($key, $value));
					break;

				case MgPropertyType::Int32 :
					$properties->Add(new MgInt32Property($key, $value));
					break;

				case MgPropertyType::Int64 :
					$properties->Add(new MgInt64Property($key, $value));
					break;

				case MgPropertyType::String :
					$properties->Add(new MgStringProperty($key, $value));
					break;

				case MgPropertyType::DateTime :
					$properties->Add(new MgDateTimeProperty($key, $value));
					break;

				case MgPropertyType::Null :
				case MgPropertyType::Blob :
				case MgPropertyType::Clob :
				case MgPropertyType::Feature :
				case MgPropertyType::Geometry :
				case MgPropertyType::Raster :
					break;
			}
		}

		$selection = new MgSelection($map);
		$selection->AddFeatureIds($layer, $featureClass, $properties);

		return $selection->ToXml();
	}

	private function GetFeaturePropertyValue($featureReader, $name)
	{
		$value = '';
		$propertyType = $featureReader->GetPropertyType($name);
		switch($propertyType)
		{
			case MgPropertyType::Boolean :
				$value = $featureReader->GetBoolean($name);
				break;

			case MgPropertyType::Byte :
				$value = $featureReader->GetByte($name);
				break;

			case MgPropertyType::Single :
				$value = $featureReader->GetSingle($name);
				break;

			case MgPropertyType::Double :
				$value = $featureReader->GetDouble($name);
				break;

			case MgPropertyType::Int16 :
				$value = $featureReader->GetInt16($name);
				break;

			case MgPropertyType::Int32 :
				$value = $featureReader->GetInt32($name);
				break;

			case MgPropertyType::Int64 :
				$value = $featureReader->GetInt64($name);
				break;

			case MgPropertyType::String :
				$value = $featureReader->GetString($name);
				break;

			case MgPropertyType::DateTime :
			case MgPropertyType::Null :
			case MgPropertyType::Blob :
			case MgPropertyType::Clob :
			case MgPropertyType::Feature :
			case MgPropertyType::Geometry :
			case MgPropertyType::Raster :
				$value = '[unsupported data type]';
				break;
		}
		return $value;
	}

	private function GetFeatureIdList($map, $layer, $featureReader)
	{
		$classDef = $featureReader->GetClassDefinition();
		$idProps = $classDef->GetIdentityProperties();
		$idList = array();

		for ($i = 0; $i < $idProps->GetCount(); $i++)
		{
			$idProp = $idProps->GetItem($i);

			switch($idProp->GetDataType()) {
				case MgPropertyType::Boolean :
					$idList[$idProp->GetName()] = $featureReader->GetBoolean($idProp->GetName());
					break;

				case MgPropertyType::Byte :
					$idList[$idProp->GetName()] = $featureReader->GetByte($idProp->GetName());
					break;

				case MgPropertyType::Single :
					$idList[$idProp->GetName()] = $featureReader->GetSingle($idProp->GetName());
					break;

				case MgPropertyType::Double :
					$idList[$idProp->GetName()] = $featureReader->GetDouble($idProp->GetName());
					break;

				case MgPropertyType::Int16 :
					$idList[$idProp->GetName()] = $featureReader->GetInt16($idProp->GetName());
					break;

				case MgPropertyType::Int32 :
					$idList[$idProp->GetName()] = $featureReader->GetInt32($idProp->GetName());
					break;

				case MgPropertyType::Int64 :
					$idList[$idProp->GetName()] = $featureReader->GetInt64($idProp->GetName());
					break;

				case MgPropertyType::String :
					$idList[$idProp->GetName()] = $featureReader->GetString($idProp->GetName());
					break;

				case MgPropertyType::DateTime :
					$idList[$idProp->GetName()] = $featureReader->GetDateTime($idProp->GetName());
					break;

				case MgPropertyType::Null :
				case MgPropertyType::Blob :
				case MgPropertyType::Clob :
				case MgPropertyType::Feature :
				case MgPropertyType::Geometry :
				case MgPropertyType::Raster :
					break;
			}
		}

		return $idList;
	}

    private function IsValidDataType($type)
    {
        $valid = true;

        switch($type)
        {
            case MgPropertyType::Blob:
				$valid = false;
				break;
            case MgPropertyType::Clob:
				$valid = false;
				break;
            case MgPropertyType::Byte:
				$valid = false;
				break;
            case MgPropertyType::Feature:
				$valid = false;
				break;
            case MgPropertyType::Geometry:
				$valid = false;
				break;
            case MgPropertyType::Null:
				$valid = false;
				break;
        }
        return $valid;
    }

	private function CreateFilterSchema()
	{
		$filterSchema = new MgFeatureSchema();
		$filterSchema->SetName('FilterSchema');

		$filterClass = new MgClassDefinition();
		$filterClass->SetName('Filter');
		$properties = $filterClass->GetProperties();

		$idProperty = new MgDataPropertyDefinition('ID');
		$idProperty->SetDataType(MgPropertyType::Int32);
		$idProperty->SetReadOnly(true);
		$idProperty->SetNullable(false);
		$idProperty->SetAutoGeneration(true);
		$properties->Add($idProperty);

		$geometryProperty = new MgGeometricPropertyDefinition('Geometry');
		$geometryProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
		$geometryProperty->SetHasElevation(false);
		$geometryProperty->SetHasMeasure(false);
		$geometryProperty->SetReadOnly(false);
		$geometryProperty->SetSpatialContextAssociation('MAPCS');
		$properties->Add($geometryProperty);

		$filterClass->GetIdentityProperties()->Add($idProperty);
		$filterClass->SetDefaultGeometryPropertyName('Geometry');

		$filterSchema->GetClasses()->Add($filterClass);

		return $filterSchema;
	}

	private function CreatePolygonFromGeomText($geomText)
	{
		$geometryFactory = new MgGeometryFactory();

		$vertices = explode(',', $geomText);
		$count = $vertices[0];

		$coords = new MgCoordinateCollection();
		for ($i = 0; $i < $count; $i++)
		{
			$coord = $geometryFactory->CreateCoordinateXY((double) $vertices[($i * 2) + 1], (double) $vertices[($i * 2) + 2]);
			$coords->Add($coord);
		}

		$linearRing = $geometryFactory->CreateLinearRing($coords);
		$polygon = $geometryFactory->CreatePolygon($linearRing, null);

		return $polygon;
	}
}
?>