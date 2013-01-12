<?php

require_once dirname(__FILE__).'/../../../layers/MapGuide/php/Constants.php';
require_once 'classes/markupschemafactory.php';

class MarkupManager
{
    const MARKUP_REGISTRY_NAME = 'MarkupRegistry';

	private $args = null;
	private $site = null;
    private $markupRegistryId = null;

	function __construct($args)
	{
		$this->args = $args;
		$this->site = new MgSiteConnection();
		$this->site->Open(new MgUserInformation($args['SESSION']));
        $this->InitMarkupRegistry();
	}
    
    function SetArgument($arg, $value)
    {
        $this->args[$arg] = $value;
    }
    
    function InitMarkupRegistry()
    {
        //NOTE: EnumerateResources does not work for session repositories. So to be able to "enumerate"
        //resources, we create a registry feature source that would store this information
    
        $this->markupRegistryId = new MgResourceIdentifier($this->GetResourceIdPrefix().MarkupManager::MARKUP_REGISTRY_NAME.".FeatureSource");
        $resourceService = $this->site->CreateService(MgServiceType::ResourceService);
        
        //Create the markup registry feature source if it doesn't already exist
        if (!$resourceService->ResourceExists($this->markupRegistryId))
        {
            $featureService = $this->site->CreateService(MgServiceType::FeatureService);
        
            //Markup Registry Feature Source Schema
            //
            //Default
            //  MarkupRegistry
            //    ResourceId (String, Identity, Not Null)
            //    LayerDefintion (String, Not Null)
            //    Name (String, Not Null)
        
            $markupRegSchema = new MgFeatureSchema("Default", "");
            $markupRegClass = new MgClassDefinition();
            $markupRegClass->SetName("MarkupRegistry");
            
            $markupRegId = new MgDataPropertyDefinition("ResourceId");
            $markupRegId->SetDataType(MgPropertyType::String);
            $markupRegId->SetLength(1024);
            $markupRegId->SetNullable(false);
            
            $layerDefId = new MgDataPropertyDefinition("LayerDefinition");
            $layerDefId->SetDataType(MgPropertyType::String);
            $layerDefId->SetLength(1024);
            $layerDefId->SetNullable(false);
            
            $markupRegName = new MgDataPropertyDefinition("Name");
            $markupRegName->SetDataType(MgPropertyType::String);
            $markupRegName->SetLength(512);
            $markupRegName->SetNullable(false);
            
            $dataProps = $markupRegClass->GetProperties();
            $dataProps->Add($markupRegId);
            $dataProps->Add($layerDefId);
            $dataProps->Add($markupRegName);
            
            $idProps = $markupRegClass->GetIdentityProperties();
            $idProps->Add($markupRegId);
            
            $classes = $markupRegSchema->GetClasses();
            $classes->Add($markupRegClass);
            
            $createSdf = new MgCreateSdfParams("Default", "", $markupRegSchema);
            $featureService->CreateFeatureSource($this->markupRegistryId, $createSdf);
        }
    }
    
    function GetResourceIdPrefix()
    {
        return "Session:" . $this->args["SESSION"] . "//";
    }
    
    function GetFeatureSource($layerDefinitionId)
    {
        $featureService = $this->site->CreateService(MgServiceType::FeatureService);
        $query = new MgFeatureQueryOptions();
        $query->SetFilter("LayerDefinition = '$layerDefinitionId'");
        $fr = $featureService->SelectFeatures($this->markupRegistryId, "Default:MarkupRegistry", $query);
        
        $fsId = "";
        if($fr->ReadNext())
        {
            $fsId = $fr->GetString("ResourceId");
        }
        $fr->Close();
        return $fsId;
    }

	function GetAvailableMarkup()
	{
		$markup = array();
        
        //Query the markup registry
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
        $query = new MgFeatureQueryOptions();
        $fr = $featureService->SelectFeatures($this->markupRegistryId, "Default:MarkupRegistry", $query);
        while($fr->ReadNext())
        {
            $resId = $fr->GetString("LayerDefinition");
            $resName = $fr->GetString("Name");
        
            $markup[$resId] = $resName;
        }
        $fr->Close();
        
        /*
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
        $resourceID = new MgResourceIdentifier("Library://Markup/");
		
		try
		{		
			$byteReader = $resourceService->EnumerateResources($resourceID, 1, "LayerDefinition");
			$resourceListXML = $byteReader->ToString();
			
			$doc = DOMDocument::loadXML($resourceListXML);
			$nodeList = $doc->getElementsByTagName('ResourceId');
			
			foreach ($nodeList as $node)
			{
				$resourceId = new MgResourceIdentifier($node->nodeValue);
				$markup[$resourceId->ToString()] = $resourceId->GetName();
			}
			asort($markup);
		}
		catch (MgResourceNotFoundException $mge)
		{
			// If the Library://Markup folder does not exist, create it.
			$resourceService->SetResource($resourceID, null, null); 
		}*/
		
		return $markup;
	}
    
    function StripAlphaFromColorString($str) {
        return substr($str, 2, strlen($str) - 2);
    }
    
    function GetLayerStyle($layerDef)
    {
        $style = new LayerStyle();
        $resId = new MgResourceIdentifier($layerDef);
        $resourceService = $this->site->CreateService(MgServiceType::ResourceService);
        $br = $resourceService->GetResourceContent($resId);
        
        $doc = DOMDocument::LoadXML($br->ToString());
        $vsr = $doc->getElementsByTagName("VectorScaleRange")->item(0);
        
        $pts = $vsr->getElementsByTagName("PointTypeStyle");
        $ats = $vsr->getElementsByTagName("AreaTypeStyle");
        $lts = $vsr->getElementsByTagName("LineTypeStyle");
        
        if ($pts->length > 0) {
            $pr = $pts->item(0);
            $prLabel = $pr->getElementsByTagName("Label")->item(0);
            $prMark = $pr->getElementsByTagName("PointSymbolization2D")->item(0)->getElementsByTagName("Mark")->item(0);
            
            $style->LABEL_SIZE_UNITS = $prLabel->getElementsByTagName("Unit")->item(0)->nodeValue;
            $style->LABEL_FONT_SIZE = $prLabel->getElementsByTagName("SizeX")->item(0)->nodeValue;
            $style->LABEL_FORE_COLOR = $this->StripAlphaFromColorString($prLabel->getElementsByTagName("ForegroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_COLOR = $this->StripAlphaFromColorString($prLabel->getElementsByTagName("BackgroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_STYLE = $prLabel->getElementsByTagName("BackgroundStyle")->item(0)->nodeValue;
            $style->LABEL_BOLD = $prLabel->getElementsByTagName("Bold")->item(0)->nodeValue;
            $style->LABEL_ITALIC = $prLabel->getElementsByTagName("Italic")->item(0)->nodeValue;
            $style->LABEL_UNDERLINE = $prLabel->getElementsByTagName("Underlined")->item(0)->nodeValue;
            
            $style->MARKER_SIZE_UNITS = $prMark->getElementsByTagName("Unit")->item(0)->nodeValue;
            $style->MARKER_SIZE = $prMark->getElementsByTagName("SizeX")->item(0)->nodeValue;
            $style->MARKER_TYPE = $prMark->getElementsByTagName("Shape")->item(0)->nodeValue;
            
            $style->MARKER_COLOR = $this->StripAlphaFromColorString($prMark->getElementsByTagName("ForegroundColor")->item(0)->nodeValue);
            //$style->MARKER_COLOR = $this->StripAlphaFromColorString($prMark->item(7)->childNodes->item(2)->nodeValue);
        }
        if ($lts->length > 0) {
            $lr = $lts->item(0);
            $lrLabel = $lr->getElementsByTagName("Label")->item(0);
            $lrSym = $lr->getElementsByTagName("LineSymbolization2D")->item(0);
            
            $style->LABEL_SIZE_UNITS = $lrLabel->getElementsByTagName("Unit")->item(0)->nodeValue;
            $style->LABEL_FONT_SIZE = $lrLabel->getElementsByTagName("SizeX")->item(0)->nodeValue;
            $style->LABEL_FORE_COLOR = $this->StripAlphaFromColorString($lrLabel->getElementsByTagName("ForegroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_COLOR = $this->StripAlphaFromColorString($lrLabel->getElementsByTagName("BackgroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_STYLE = $lrLabel->getElementsByTagName("BackgroundStyle")->item(0)->nodeValue;
     
            if ($pts->length == 0) {
                $style->LABEL_BOLD = $lrLabel->getElementsByTagName("Bold")->item(0)->nodeValue;
                $style->LABEL_ITALIC = $lrLabel->getElementsByTagName("Italic")->item(0)->nodeValue;
                $style->LABEL_UNDERLINE = $lrLabel->getElementsByTagName("Underlined")->item(0)->nodeValue;
            }
            
            $style->LINE_PATTERN = $lrSym->getElementsByTagName("LineStyle")->item(0)->nodeValue;
            $style->LINE_THICKNESS = $lrSym->getElementsByTagName("Thickness")->item(0)->nodeValue;
            $style->LINE_COLOR = $this->StripAlphaFromColorString($lrSym->getElementsByTagName("Color")->item(0)->nodeValue);
            $style->LINE_SIZE_UNITS = $lrSym->getElementsByTagName("Unit")->item(0)->nodeValue;
        }
        if ($ats->length > 0) {
            $ar = $ats->item(0);
            $arLabel = $ar->getElementsByTagName("Label")->item(0);
            $arSym = $ar->getElementsByTagName("AreaSymbolization2D")->item(0);
            
            $style->LABEL_SIZE_UNITS = $arLabel->getElementsByTagName("Unit")->item(0)->nodeValue;
            $style->LABEL_FONT_SIZE = $arLabel->getElementsByTagName("SizeX")->item(0)->nodeValue;
            $style->LABEL_FORE_COLOR = $this->StripAlphaFromColorString($arLabel->getElementsByTagName("ForegroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_COLOR = $this->StripAlphaFromColorString($arLabel->getElementsByTagName("BackgroundColor")->item(0)->nodeValue);
            $style->LABEL_BACK_STYLE = $arLabel->getElementsByTagName("BackgroundStyle")->item(0)->nodeValue;
            
            if ($pts->length == 0 && $lts->length == 0)
            {
                $style->LABEL_BOLD = $arLabel->getElementsByTagName("Bold")->item(0)->nodeValue;
                $style->LABEL_ITALIC = $arLabel->getElementsByTagName("Italic")->item(0)->nodeValue;
                $style->LABEL_UNDERLINE = $arLabel->getElementsByTagName("Underlined")->item(0)->nodeValue;
            }
            
            $arSymFill = $arSym->getElementsByTagName("Fill")->item(0);
            $arSymStroke = $arSym->getElementsByTagName("Stroke")->item(0);
            
            $style->FILL_PATTERN = $arSymFill->getElementsByTagName("FillPattern")->item(0)->nodeValue;
            $style->FILL_FORE_COLOR = $this->StripAlphaFromColorString($arSymFill->getElementsByTagName("ForegroundColor")->item(0)->nodeValue);
            $style->FILL_BACK_COLOR = $this->StripAlphaFromColorString($arSymFill->getElementsByTagName("BackgroundColor")->item(0)->nodeValue);
            
            $style->BORDER_PATTERN = $arSymStroke->getElementsByTagName("LineStyle")->item(0)->nodeValue;
            $style->BORDER_THICKNESS = $arSymStroke->getElementsByTagName("Thickness")->item(0)->nodeValue;
            $style->BORDER_COLOR = $this->StripAlphaFromColorString($arSymStroke->getElementsByTagName("Color")->item(0)->nodeValue);
            $style->BORDER_SIZE_UNITS = $arSymStroke->getElementsByTagName("Unit")->item(0)->nodeValue;
        }
        return $style;
    }
		
	function OpenMarkup()
	{
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		
		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);

		// Create the Layer Groups

		$markupGroup = null;
		$layerGroups = $map->GetLayerGroups();
		if ($layerGroups->Contains('_Markup'))
		{
			$markupGroup = $layerGroups->GetItem('_Markup');
		}
		else
		{
			$markupGroup = new MgLayerGroup('_Markup');
			$markupGroup->SetVisible(true);
			$markupGroup->SetLegendLabel('Markup');
			$markupGroup->SetDisplayInLegend(true);
			$layerGroups->Add($markupGroup);
		}

		// Add the Markup Layer
		
		$markupLayerResId = new MgResourceIdentifier($this->args['MARKUPLAYER']);
		$markupLayer = new MgLayer($markupLayerResId, $resourceService);
        $markupLayer->SetName('_' . $markupLayerResId->GetName());
        $markupLayer->SetLegendLabel($markupLayerResId->GetName());
		$markupLayer->SetDisplayInLegend(true);
		$markupLayer->SetSelectable(true);
		$markupLayer->SetGroup($markupGroup);
		$map->GetLayers()->Insert(0, $markupLayer);
		
		$map->Save($resourceService);
	}

	function CloseMarkup()
	{
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		
		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);

		// Add the Markup Layer

		$markupLayerResId = new MgResourceIdentifier($this->args['OPENMARKUP']);
		$index = $map->GetLayers()->IndexOf('_' . $markupLayerResId->GetName());
		$map->GetLayers()->RemoveAt($index);
		
		$map->Save($resourceService);
	}
	
	function UniqueMarkupName(&$markupName)
	{
		$availableMarkups = $this->GetAvailableMarkup();
		foreach ($availableMarkups as &$availableMarkupName)
		{		
			if($availableMarkupName == $markupName)
			{
				$markupName = $markupName . '_1';
				$this->UniqueMarkupName($markupName);
				break;
			}
		}
	}
    
    function CreateMarkupLayerDefinitionContent($featureSourceId)
    {
        // Create the Markup Layer Definition. Create or update, this code is the same.

		$hexFgTransparency = sprintf("%02x", 255 * (100 - $this->args['FILLTRANSPARENCY']) / 100); // Convert % to an alpha value
		$hexBgTransparency = $this->args['FILLBACKTRANS'] ? "FF" : "00";							 // All or nothing
		$bold = array_key_exists('LABELBOLD', $this->args) ? "true" : "false";
		$italic = array_key_exists('LABELITALIC', $this->args) ? "true" : "false";
		$underline = array_key_exists('LABELUNDERLINE', $this->args) ? "true" : "false";
		
        $markupLayerDefinition = file_get_contents("templates/markuplayerdefinition.xml");
        $markupLayerDefinition = sprintf($markupLayerDefinition, 
			$featureSourceId,						            //<ResourceId> - Feature Source
			$this->args['LABELSIZEUNITS'],						//<Unit> - Mark Label
			$this->args['LABELFONTSIZE'],						//<SizeX> - Mark Label Size
			$this->args['LABELFONTSIZE'],						//<SizeY> - Mark Label Size
			'FF' . $this->args['LABELFORECOLOR'],				//<ForegroundColor> - Mark Label
			'FF' . $this->args['LABELBACKCOLOR'],				//<BackgroundColor> - Mark Label
			$this->args['LABELBACKSTYLE'],						//<BackgroundStyle> - Mark Label
			$bold,												//<Bold> - Mark Label
			$italic,											//<Bold> - Mark Label
			$underline,											//<Underlined> - Mark Label
			$this->args['MARKERSIZEUNITS'],						//<Unit> - Mark
			$this->args['MARKERSIZE'],							//<SizeX> - Mark
			$this->args['MARKERSIZE'],							//<SizeY> - Mark
			$this->args['MARKERTYPE'],							//<Shape> - Mark
			'FF' . $this->args['MARKERCOLOR'],					//<ForegroundColor> - Mark
			'FF' . $this->args['MARKERCOLOR'],					//<Color> - Mark
			$this->args['LABELSIZEUNITS'],						//<Unit> - Line Label
			$this->args['LABELFONTSIZE'],						//<SizeX> - Line Label Size
			$this->args['LABELFONTSIZE'],						//<SizeY> - Line Label Size
			'FF' . $this->args['LABELFORECOLOR'],				//<ForegroundColor> - Line Label
			'FF' . $this->args['LABELBACKCOLOR'],				//<BackgroundColor> - Line Label
			$this->args['LABELBACKSTYLE'],						//<BackgroundStyle> - Line Label
			$bold,												//<Bold> - Line Label
			$italic,											//<Bold> - Line Label
			$underline,											//<Underlined> - Line Label			
			$this->args['LINEPATTERN'],							//<LineStyle> - Line
			$this->args['LINETHICKNESS'],						//<Thickness> - Line
			'FF' . $this->args['LINECOLOR'],					//<Color> - Line
			$this->args['LINESIZEUNITS'],						//<Unit> - Line
			$this->args['LABELSIZEUNITS'],						//<Unit> - Polygon Label
			$this->args['LABELFONTSIZE'],						//<SizeX> - Polygon Label Size
			$this->args['LABELFONTSIZE'],						//<SizeY> - Polygon Label Size
			'FF' . $this->args['LABELFORECOLOR'],				//<ForegroundColor> - Polygon Label
			'FF' . $this->args['LABELBACKCOLOR'],				//<BackgroundColor> - Polygon Label
			$this->args['LABELBACKSTYLE'],						//<BackgroundStyle> - Polygon Label
			$bold,												//<Bold> - Polygon Label
			$italic,											//<Bold> - Polygon Label
			$underline,											//<Underlined> - Polygon Label
			$this->args['FILLPATTERN'], 						//<FillPattern> - Fill
			$hexFgTransparency . $this->args['FILLFORECOLOR'], 	//<ForegroundColor> - Fill
			$hexBgTransparency . $this->args['FILLBACKCOLOR'], 	//<BackgroundColor> - Fill
			$this->args['BORDERPATTERN'],						//<LineStyle> - Fill
			$this->args['BORDERTHICKNESS'], 					//<Thickness> - Fill
			'FF' . $this->args['BORDERCOLOR'], 					//<Color> - Fill
			$this->args['BORDERSIZEUNITS']); 					//<Unit> - Fill
            
        return $markupLayerDefinition;
    }
    
	function CreateMarkup()
	{
        $markupName = $this->args["MARKUPNAME"];
		$this->UniqueMarkupName($markupName);
		
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		$featureService = $this->site->CreateService(MgServiceType::FeatureService);
        
        $map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);

        $featureSourceId = "";
        $bUpdate = array_key_exists("EDITMARKUPLAYER", $this->args) && array_key_exists("MARKUPLAYERNAME", $this->args) && array_key_exists("EDITFEATURESOURCE", $this->args);
        
		// Create the Markup Feature Source (SDF) if not updating
        if (!$bUpdate)
        {
            $markupSdfResId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $markupName . '.FeatureSource');
            
            $markupSchema = MarkupSchemaFactory::CreateMarkupSchema();
            $sdfParams = new MgCreateSdfParams('Default', $map->GetMapSRS(), $markupSchema);
            $featureService->CreateFeatureSource($markupSdfResId, $sdfParams);
            $featureSourceId = $markupSdfResId->ToString();
        }
        else 
        {
            $featureSourceId = $this->args["EDITFEATURESOURCE"];
        }
		
		$markupLayerDefinition = $this->CreateMarkupLayerDefinitionContent($featureSourceId);
		$byteSource = new MgByteSource($markupLayerDefinition, strlen($markupLayerDefinition));
        //Save to new resource or overwrite existing
        $layerDefId = new MgResourceIdentifier($bUpdate ? $this->args["EDITMARKUPLAYER"] : ($this->GetResourceIdPrefix() . $markupName . '.LayerDefinition'));
		$resourceService->SetResource($layerDefId, $byteSource->GetReader(), null);
        
        $cmds = new MgFeatureCommandCollection();
        //Register markup with markup registry if not updating
        if (!$bUpdate)
        {
            $props = new MgPropertyCollection();
            $props->Add(new MgStringProperty("ResourceId", $markupSdfResId->ToString()));
            $props->Add(new MgStringProperty("LayerDefinition", $layerDefId->ToString()));
            $props->Add(new MgStringProperty("Name", $layerDefId->GetName()));
            $insertCmd = new MgInsertFeatures("Default:MarkupRegistry", $props);
            
            $cmds->Add($insertCmd);
        }
        
        if ($cmds->GetCount() > 0) {
            $res = $featureService->UpdateFeatures($this->markupRegistryId, $cmds, false);
            MarkupManager::CleanupReaders($res);
        }
        
        return $layerDefId->ToString();
	}
    
    //Utility function to close all feature readers in a MgPropertyCollection
    static function CleanupReaders($propCol)
    {
        for ($i = 0; $i < $propCol->GetCount(); $i++)
        {
            $prop = $propCol->GetItem($i);
            if ($prop->GetPropertyType() == MgPropertyType::Feature)
            {
                $fr = $prop->GetValue();
                $fr->Close();
            }
        }
    }
	
	function DeleteMarkup()
	{
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
        $featureService = $this->site->CreateService(MgServiceType::FeatureService);

		$markupLayerResId = new MgResourceIdentifier($this->args['MARKUPLAYER']);
		$markupSdfResId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $markupLayerResId->GetName() . '.FeatureSource');

		$resourceService->DeleteResource($markupLayerResId);
		$resourceService->DeleteResource($markupSdfResId);
        
        //Delete from markup registry
        $delete = new MgDeleteFeatures("Default:MarkupRegistry", "ResourceId = '" . $markupSdfResId->ToString() . "' AND LayerDefinition = '" . $markupLayerResId->ToString() . "'");
        $cmds = new MgFeatureCommandCollection();
        $cmds->Add($delete);
        
        $res = $featureService->UpdateFeatures($this->markupRegistryId, $cmds, false);
	}
    
    function DownloadMarkup()
    {
        $resourceService = $this->site->CreateService(MgServiceType::ResourceService);
    
        $markupLayerResId = new MgResourceIdentifier($this->args['MARKUPLAYER']);
		$markupSdfResId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $markupLayerResId->GetName() . '.FeatureSource');

        $dataName = $markupLayerResId->GetName().".sdf";
        $byteReader = $resourceService->GetResourceData($markupSdfResId, $dataName);
        $len = $byteReader->GetLength();
        
        $outputBuffer = '';
        $buffer = '';
        while ($byteReader->Read($buffer, 50000) != 0)
        {
            $outputBuffer .= $buffer;
        }
        
        header("Content-Type: " . $byteReader->GetMimeType());
        header("Content-Disposition: attachment; filename=$dataName");
        header("Content-Length: " . strlen($outputBuffer));
        echo $outputBuffer;
    }
    
    function UploadMarkup()
    {
        $resourceService = $this->site->CreateService(MgServiceType::ResourceService);
        $featureService = $this->site->CreateService(MgServiceType::FeatureService);
        $bs = new MgByteSource($_FILES["UPLOADFILE"]["tmp_name"]);
        $br = $bs->GetReader();

        $uploadFileParts = pathinfo($_FILES["UPLOADFILE"]["name"]);
        //Use file name to drive all parameters
        $baseName = $uploadFileParts["filename"];
        $this->UniqueMarkupName($baseName); //Guard against potential duplicates
        $ext = $uploadFileParts["extension"];
        
        $markupLayerResId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $baseName . ".LayerDefinition");
		$markupSdfResId = new MgResourceIdentifier($this->GetResourceIdPrefix() . $baseName . '.FeatureSource');
        
        $dataName = $baseName . "." . $ext;
        $fsXml = sprintf(file_get_contents("templates/markupfeaturesource.xml"), $dataName);
        $bs2 = new MgByteSource($fsXml, strlen($fsXml));
        $resourceService->SetResource($markupSdfResId, $bs2->GetReader(), null);
        $resourceService->SetResourceData($markupSdfResId, $dataName, "File", $bs->GetReader());
        
        //Set up default style args
        $this->args["MARKUPNAME"] = $baseName;
        
        $this->args["MARKERCOLOR"] = DefaultStyle::MARKER_COLOR;
        $this->args["MARKERTYPE"] = DefaultStyle::MARKER_TYPE;
        $this->args["MARKERSIZEUNITS"] = DefaultStyle::MARKER_SIZE_UNITS;
        $this->args["MARKERSIZE"] = DefaultStyle::MARKER_SIZE;
        $this->args["LINECOLOR"] = DefaultStyle::LINE_COLOR;
        $this->args["LINEPATTERN"] = DefaultStyle::LINE_PATTERN;
        $this->args["LINESIZEUNITS"] = DefaultStyle::LINE_SIZE_UNITS;
        $this->args["LINETHICKNESS"] = DefaultStyle::LINE_THICKNESS;
        $this->args["FILLPATTERN"] = DefaultStyle::FILL_PATTERN;
        $this->args["FILLTRANSPARENCY"] = DefaultStyle::FILL_TRANSPARENCY;
        $this->args["FILLFORECOLOR"] = DefaultStyle::FILL_FORE_COLOR;
        $this->args["FILLBACKCOLOR"] = DefaultStyle::FILL_BACK_COLOR;
        $this->args["FILLBACKTRANS"] = DefaultStyle::FILL_BACK_TRANS;
        $this->args["BORDERPATTERN"] = DefaultStyle::BORDER_PATTERN;
        $this->args["BORDERSIZEUNITS"] = DefaultStyle::BORDER_SIZE_UNITS;
        $this->args["BORDERCOLOR"] = DefaultStyle::BORDER_COLOR;
        $this->args["BORDERTHICKNESS"] = DefaultStyle::BORDER_THICKNESS;
        $this->args["LABELSIZEUNITS"] = DefaultStyle::LABEL_SIZE_UNITS;
        $this->args["LABELFONTSIZE"] = DefaultStyle::LABEL_FONT_SIZE;
        
        //Omission is considered false, which is the default. If you ever change
        //the default style values, uncomment the matching "true" values
        //$this->args["LABELBOLD"] = DefaultStyle::LABEL_BOLD;
        //$this->args["LABELITALIC"] = DefaultStyle::LABEL_ITALIC;
        //$this->args["LABELUNDERLINE"] = DefaultStyle::LABEL_UNDERLINE;
        
        $this->args["LABELFORECOLOR"] = DefaultStyle::LABEL_FORE_COLOR;
        $this->args["LABELBACKCOLOR"] = DefaultStyle::LABEL_BACK_COLOR;
        $this->args["LABELBACKSTYLE"] = DefaultStyle::LABEL_BACK_STYLE;
        
        $markupLayerDefinition = $this->CreateMarkupLayerDefinitionContent($markupSdfResId->ToString());
        
		$layerBs = new MgByteSource($markupLayerDefinition, strlen($markupLayerDefinition));
        //Save to new resource or overwrite existing
		$resourceService->SetResource($markupLayerResId, $layerBs->GetReader(), null);
        
        //Add to markup registry
        $cmds = new MgFeatureCommandCollection();
        $props = new MgPropertyCollection();
        $props->Add(new MgStringProperty("ResourceId", $markupSdfResId->ToString()));
        $props->Add(new MgStringProperty("LayerDefinition", $markupLayerResId->ToString()));
        $props->Add(new MgStringProperty("Name", $markupLayerResId->GetName()));
        $insertCmd = new MgInsertFeatures("Default:MarkupRegistry", $props);

        $cmds->Add($insertCmd);
        $res = $featureService->UpdateFeatures($this->markupRegistryId, $cmds, false);
        MarkupManager::CleanupReaders($res);
        
        //Add to map
        $this->args["MARKUPLAYER"] = $markupLayerResId->ToString();
        $this->OpenMarkup();
    }

	function GetOpenMarkup()
	{
		$openMarkup = array();
		
		$resourceService = $this->site->CreateService(MgServiceType::ResourceService);
		
		$map = new MgMap();
		$map->Open($resourceService, $this->args['MAPNAME']);
		
		$layerGroups = $map->GetLayerGroups();
		if ($layerGroups->Contains('_Markup'))
		{
			$layers = $map->GetLayers();
			
			for ($i = 0; $i < $layers->GetCount(); $i++)
			{
				$layer = $layers->GetItem($i);
				if (($layer->GetGroup() != null) and ($layer->GetGroup()->GetName() == '_Markup'))
				{
					$openMarkup[$this->GetResourceIdPrefix() . $layer->GetLegendLabel() . '.LayerDefinition'] = $layer->GetLegendLabel();
				}		
			}
			asort($openMarkup);			
		}				
		return $openMarkup;
	}	
}

?>