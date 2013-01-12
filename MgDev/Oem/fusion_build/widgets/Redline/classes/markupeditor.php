<?php

require_once 'classes/markupmanager.php';

class MarkupEditor
{
	private $args = null;
	private $site = null;

	function __construct($args)
	{
		$this->args = $args;
		$this->site = new MgSiteConnection();
		$this->site->Open(new MgUserInformation($args['SESSION']));
	}
    
    function GetResourceIdPrefix()
    {
        return "Session:" . $this->args["SESSION"] . "//";
    }

	function GetMarkupLayer()
	{
		return $this->args['OPENMARKUP'];
	}

	function GetMarkupName()
	{
		$resId = new MgResourceIdentifier($this->GetMarkupLayer());
		return $resId->GetName();
	}

	function GetMarkupFeatures()
	{
		$features = array();
		
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
		$featureSourceId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $this->GetMarkupName() . '.FeatureSource');
		
		$featureReader = $featureService->SelectFeatures($featureSourceId, 'Markup', null);
		while ($featureReader->ReadNext())
		{
			$id = $featureReader->GetInt32('ID');
			$text = trim($featureReader->GetString('Text'));
			$features[$id] = $text;
		}
		
		return $features;
	}
	
	function GetTransform()
	{
		$coordSysFactory = new MgCoordinateSystemFactory();
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
        
        $featureSourceId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $this->GetMarkupName() . '.FeatureSource');
        $wkt = null;
        
        //Get the WKT from spatial context, because SDF only supports one spatial context it will be the first one
        $scReader = $featureService->GetSpatialContexts($featureSourceId, true);
        if ($scReader->ReadNext())
            $wkt = $scReader->GetCoordinateSystemWkt();
        $scReader->Close();
        
		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
        
        if (null == $wkt)
            return null; //It's bunk. Assume map SRS
        
        if (strcmp($wkt, $map->GetMapSRS()) == 0)
            return null; //Identical. No transformation needed
		
		$source = $coordSysFactory->Create($map->GetMapSRS());	
        if (null != $wkt)
            $target = $coordSysFactory->Create($wkt);
	
        return $coordSysFactory->GetTransform($source, $target);
	}

	function AddPoint()
	{
		$geometryFactory = new MgGeometryFactory();
		$agfWriter = new MgAgfReaderWriter();
		
		$vertices = explode(',', $this->args['GEOMETRY']);
		
		$coord = $geometryFactory->CreateCoordinateXY((double) $vertices[0], (double) $vertices[1]);
		$point = $geometryFactory->CreatePoint($coord);
        
        $trans = $this->GetTransform();
        if (null != $trans)
            $point = $point->Transform($trans);
		
        $byteReader = $agfWriter->Write($point);
		
		$propertyValues = new MgPropertyCollection();
		$propertyValues->Add(new MgStringProperty('Text', $this->args['TEXT']));
		$propertyValues->Add(new MgGeometryProperty('Geometry', $byteReader));
		
		$this->InsertMarkupFeature($propertyValues);
	}

	function AddLineString()
	{
		$geometryFactory = new MgGeometryFactory();
		$agfWriter = new MgAgfReaderWriter();
		
		$vertices = explode(',', $this->args['GEOMETRY']);
		$count = $vertices[0];

		$coords = new MgCoordinateCollection();
		for ($i = 0; $i < $count; $i++)
		{
			$coord = $geometryFactory->CreateCoordinateXY((double) $vertices[($i * 2) + 1], (double) $vertices[($i * 2) + 2]);
			$coords->Add($coord);
		}
		
		$lineString = $geometryFactory->CreateLineString($coords);
        
        $trans = $this->GetTransform();
        if (null != $trans)
            $lineString = $lineString->Transform($trans);
            
		$byteReader = $agfWriter->Write($lineString);
		
		$propertyValues = new MgPropertyCollection();
		$propertyValues->Add(new MgStringProperty('Text', $this->args['TEXT']));
		$propertyValues->Add(new MgGeometryProperty('Geometry', $byteReader));
		
		$this->InsertMarkupFeature($propertyValues);
	}

	function AddPolygon()
	{
		$geometryFactory = new MgGeometryFactory();
		$agfWriter = new MgAgfReaderWriter();
		
		$vertices = explode(',', $this->args['GEOMETRY']);
		$count = $vertices[0];

		$coords = new MgCoordinateCollection();
		for ($i = 0; $i < $count; $i++)
		{
			$coord = $geometryFactory->CreateCoordinateXY((double) $vertices[($i * 2) + 1], (double) $vertices[($i * 2) + 2]);
			$coords->Add($coord);
		}
		
		// Add the first vertex to make the ring close
		if($count > 2)
		{
			$coord = $geometryFactory->CreateCoordinateXY((double) $vertices[1], (double) $vertices[2]);
			$coords->Add($coord);
		}
		
		$linearRing = $geometryFactory->CreateLinearRing($coords);
		$polygon = $geometryFactory->CreatePolygon($linearRing, null);
        
        $trans = $this->GetTransform();
        if (null != $trans)
            $polygon = $polygon->Transform($trans);
		
        $byteReader = $agfWriter->Write($polygon);
		
		$propertyValues = new MgPropertyCollection();
		$propertyValues->Add(new MgStringProperty('Text', $this->args['TEXT']));
		$propertyValues->Add(new MgGeometryProperty('Geometry', $byteReader));
		
		$this->InsertMarkupFeature($propertyValues);
	}
	
	function InsertMarkupFeature($propertyValues)
	{
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);		
		$featureSourceId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $this->GetMarkupName() . '.FeatureSource');

		$commands = new MgFeatureCommandCollection();
		$commands->Add(new MgInsertFeatures('Markup', $propertyValues));

		$featureService->UpdateFeatures($featureSourceId, $commands, false);
	}
	
	function DeleteMarkup()
	{
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);		
		$featureSourceId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $this->GetMarkupName() . '.FeatureSource');

		$commands = new MgFeatureCommandCollection();
		$commands->Add(new MgDeleteFeatures('Markup', 'ID = ' . $this->args['MARKUPFEATURE']));

		$featureService->UpdateFeatures($featureSourceId, $commands, false);		
	}

	function UpdateMarkup()
	{
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);		
		$featureSourceId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $this->GetMarkupName() . '.FeatureSource');

		$propertyValues = new MgPropertyCollection();
		$propertyValues->Add(new MgStringProperty('Text', trim($this->args['UPDATETEXT'])));

		$commands = new MgFeatureCommandCollection();
		$commands->Add(new MgUpdateFeatures('Markup', $propertyValues, 'ID = ' . $this->args['MARKUPFEATURE']));

		$featureService->UpdateFeatures($featureSourceId, $commands, false);		
	}

	function GetSelectionXML()
	{
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		
		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
		$markupLayer = $map->GetLayers()->GetItem('_' . $this->GetMarkupName());

		$selection = new MgSelection($map);
		$selection->AddFeatureIdInt32($markupLayer, 'MarkupSchema:Markup', (int) $this->args['MARKUPFEATURE']);
		
		return $selection->ToXML();
	}
}

?>