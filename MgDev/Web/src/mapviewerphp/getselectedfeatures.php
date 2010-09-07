<?php

/*

TODO:

- Use the property mapping. This will have to be old-fashioned parsing the <PropertyMapping> elements 
of the layer definition. I really wanted to use MgSelection::GetSelectedFeatures(layer, class, true) but realised
that unless the geometry is mapped, you're not going to see it in the Feature Reader (ie. No zoom point). Right now 
it is returning all properties.

- I know of a few features in the Sheboygan Dataset that trip up the JSON response. Need to find out of addslashes() fixes this.

- General cleanup of the code


*/

//require_once('FirePHPCore/fb.php');

//
// Used to model the JSON response.
//
class SelectionSet
{
    private $layers;
    
    public function __construct()
    {
        $this->layers = array();
    }
    
    public function AddFeature($feat)
    {
        $layerName = $feat->layerName;
        if(!array_key_exists($layerName, $this->layers))
            $this->layers[$layerName] = array();
            
        array_push($this->layers[$layerName], $feat);
    }
    
    public function GetLayers()
    {
        return array_keys($this->layers);
    }
    
    public function GetFeatures($layerName)
    {
        if(array_key_exists($layerName, $this->layers))
            return $this->layers[$layerName];
        
        return null;
    }
}

class ZoomPoint
{
    public $x;
    public $y;
}

//
// Represents a selected feature property
//
class FeatureProperty
{
    public $name;
    public $value;
}

//
// Represents a selected feature
//
class Feature
{
    public $layerName;
    public $zoom;
    
    private $properties;
    
    public function __construct($layerName)
    {
        $this->layerName = $layerName;
        $this->properties = array();
    }
    
    public function AddProperty($prop)
    {
        $this->properties[$prop->name] = $prop;
    }
    
    public function GetProperties()
    {
        return $this->properties;
    }
}

	include 'common.php';
    include 'constants.php';

    $mapName = "";
    $sessionId = "";
    $locale = "";

    GetRequestParameters();
    
    if (null == $locale || strlen($locale) == 0)
        $locale = GetDefaultLocale();
        
    setlocale(LC_ALL, $locale);
   
	try
	{
		InitializeWebTier();
		
		$cred = new MgUserInformation($sessionId);
        $cred->SetClientIp(GetClientIp());
        $cred->SetClientAgent(GetClientAgent());

        //connect to the site and get an instance of the resource and feature service
        //
        $site = new MgSiteConnection();
        $site->Open($cred);
        $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);

        //load the map runtime state
        //
        $map = new MgMap($site);
        $map->Open($mapName);
		
		// Create the selection set
        $selection = new MgSelection($map);
        $selection->Open($resourceSrvc, $mapName);
		
		$layers = $selection->GetLayers();
        if($layers != null)
        {
            $nLayers = $layers->GetCount();
            $agfRW = new MgAgfReaderWriter();
            
            $selectionSet = new SelectionSet();
            
            for ($i = 0; $i < $nLayers; $i++) {
                $lyr = $layers->GetItem($i);
                $layerName = $lyr->GetName();
                
                //FB::log("Layer Name: " . $lyr->GetName());
                
                $featResId = new MgResourceIdentifier($lyr->GetFeatureSourceId());
                $class = $lyr->GetFeatureClassName();
                
                $queryOptions = new MgFeatureQueryOptions();
                
                $mappings = GetLayerPropertyMappings($resourceSrvc, $lyr);
                //FB::log("Property Mappings for Layer: $layerName");
                foreach($mappings as $name => $value) {
                    $queryOptions->AddFeatureProperty($name);
                    //FB::log("$name => $value");
                }
                $geomName = $lyr->GetFeatureGeometryName();
                $queryOptions->AddFeatureProperty($geomName);
                
                $filter = $selection->GenerateFilter($lyr, $class);
                $queryOptions->SetFilter($filter);
                $fr = $lyr->SelectFeatures($queryOptions);
                
                $clsDef = $fr->GetClassDefinition();
                $props = $clsDef->GetProperties();
                
                //FB::log("Geometry: $geomName");
                
                while($fr->ReadNext())
                {
                    $feat = new Feature($layerName);
                    $zoom = null;
                    
                    for ($k = 0; $k < $props->GetCount(); $k++)
                    {
                        $prop = $props->GetItem($k);
                        $propName = $prop->GetName();
                        $propType = $fr->GetPropertyType($propName);
                        
                        //We only care about mapped properties or geometry properties
                        if (array_key_exists($propName, $mappings) || $propType == MgPropertyType::Geometry)
                        {
                            $value = "";
                            if (!$fr->IsNull($propName))
                            {
                                if(strcmp($propName,$geomName) == 0)
                                {
                                    //We want the centroid so we have a zoom-to point
                                    $agf = $fr->GetGeometry($propName);
                                    $geom = $agfRW->Read($agf);
                                    $pt = $geom->GetCentroid()->GetCoordinate();
                                    
                                    $zoom = new ZoomPoint();
                                    $zoom->x = $pt->GetX();
                                    $zoom->y = $pt->GetY();
                                    
                                    $feat->zoom = $zoom;
                                    //FB::log("zoom: (".$zoom->x.",".$zoom->y.")");
                                }
                                else
                                {
                                    $value = GetPropertyValueFromFeatureReader($fr, $agfRW, $propType, $propName);
                                }
                            }
                        
                            if (array_key_exists($propName, $mappings))
                            {
                                $fp = new FeatureProperty();
                                $fp->name = $mappings[$propName];
                                $fp->value = $value;
                                
                                $feat->AddProperty($fp);
                            }
                        }
                    }
                    
                    $c = count($feat->GetProperties());
                    //FB::log("Selected feature processed ($c)");
                    
                    $selectionSet->AddFeature($feat);
                }
                $fr->Close();
            }
            
            //Now output the selection set
            
            header("Content-Type: application/json");
            header("X-JSON: true");
            echo GetJson($selectionSet);
        }
    }
    catch(Exception $e)
    {
        header("Content-Type: application/json");
        header("X-JSON: true");
        echo JsonifyError($e->getMessage(), $e->getTraceAsString());
    }
	catch(MgException $e)
    {
        header("Content-Type: application/json");
        header("X-JSON: true");
        echo JsonifyError($e->GetDetails(), $e->GetStackTrace());
    }

function GetJson($selectionSet)
{
    if($selectionSet == null)
        return "";
        
    //FB::log("Processing JSON response");
    /*
	A sample of the JSON output this method will produce:
	
	
	{
		"Layer1" : [ 
			{ 
				'values' { "name" : "name1" , "value" : "value1" }, 
				'zoom' : { x: num1, y: num2 } 
			} , 
			..,
			..,
			..,
		],
		"Layer2" : [ 
			{ 
				'values' { "name" : "name2" , "value" : "value2" }, 
				'zoom' : { x: num1, y: num2 } 
			} , 
			..,
			..,
			..,
		]
	}
	*/
    
    $layers = $selectionSet->GetLayers();
    $totalLayers = array(); //The data portion of the JSON response
    
    //FB::log("Layers in selection set: ".count($layers));
    
    for ($i = 0; $i < count($layers); $i++)
    {   
        $layerName = $layers[$i];
		
        $features = $selectionSet->GetFeatures($layerName);
        
        //FB::log("Features: ".$features);
        
        if($features != null)
        {
            //FB::log("Processing layer: $layerName");
        
            $totalFeaturesOnLayer = array();
            
            if (count($features) > 0)
            {
                for ($j = 0; $j < count($features); $j++)
                {
                    $feat = $features[$j];
                    $featureProperties = array();
                    
                    $fps = $feat->GetProperties();
                    foreach($fps as $fp)
                    {
                        $name = $fp->name;
                        $value = $fp->value;
                        //Add JSONified feature property
                        array_push($featureProperties, "{\"name\" : \"$name\", \"value\" : \"$value\" }");
                    }
                    //Add JSONified feature
                    if($feat->zoom != null)
                        array_push($totalFeaturesOnLayer, "{\"values\" : [".join(",", $featureProperties)."], \"zoom\" : { \"x\": ".$feat->zoom->x.", \"y\": ".$feat->zoom->y." } }");
                    else
                        array_push($totalFeaturesOnLayer, "{\"values\" : [".join(",", $featureProperties)."], \"zoom\" : null }");
                        
                    //FB::log("Feature processed on layer: $layerName");
                }
            }
            
            array_push($totalLayers, "\"$layerName\" : [" . join(", ", $totalFeaturesOnLayer) . "]");
            //FB::log("Selected features on layer added to final JSON response");
        }
    }
    
    //FB::log("Selection layer count: ".count($totalLayers));
    
    $result = "{" . join(",",$totalLayers) . "}";
    //return json_encode($result);
    return $result;
}

function GetPropertyValueFromFeatureReader($fr, $agfRW, $propType, $propName)
{
    $val = "";
    $tStr = "";
    switch($propType)
    {
        case MgPropertyType::Null:
            $tStr = "Null";
            $val = "";
            break;
        case MgPropertyType::Boolean:
            $tStr = "Boolean";
            $val = sprintf("%s", $fr->GetBoolean($propName));
            break;
        case MgPropertyType::Byte:
            $tStr = "Byte";
            $val = sprintf("%d", $fr->GetByte($propName));
            break;
        case MgPropertyType::DateTime:
            $tStr = "DateTime";
            $val = printDateTime($fr->GetDateTime($propName)); // yyyy-mm-dd is enforced regardless of locale
            break;
        case MgPropertyType::Single:
            $tStr = "Single";
            $val = sprintf("%f", $fr->GetSingle($propName));
            break;
        case MgPropertyType::Double:
            $tStr = "Double";
            $val = sprintf("%f", $fr->GetDouble($propName));
            break;
        case MgPropertyType::Int16:
            $tStr = "Int16";
            $val = sprintf("%d", $fr->GetInt16($propName));
            break;
        case MgPropertyType::Int32:
            $tStr = "Int32";
            $val = sprintf("%d", $fr->GetInt32($propName));
            break;
        case MgPropertyType::Int64:
            $tStr = "Int64";
            $val = sprintf("%d", $fr->GetInt64($propName));
            break;
        case MgPropertyType::String:
            $tStr = "String";
            $val = JsonEscape($fr->GetString($propName)); //String content is arbitrary
            break;
        case MgPropertyType::Blob: //NOT PRESENTABLE IN PROPERTY GRID
            $tStr = "BLOB";
            break;
        case MgPropertyType::Clob: //NOT PRESENTABLE IN PROPERTY GRID
            $tStr = "CLOB";
            break;
        case MgPropertyType::Feature: //NOT PRESENTABLE IN PROPERTY GRID
            $tStr = "Feature";
            break;
        case MgPropertyType::Geometry: //NOT PRESENTABLE IN PROPERTY GRID
            $tStr = "Geometry";
            break;
        case MgPropertyType::Raster: //NOT PRESENTABLE IN PROPERTY GRID
            $tStr = "Raster";
            break;
    }
    //FB::log("$propName ($tStr) = $val");
    return $val;
}

/* retrieve the property mappings for a layer */
function GetLayerPropertyMappings($resourceService, $layer) {
    $mappings = array();
    $byteReader = $resourceService->GetResourceContent($layer->GetLayerDefinition());
    $xmldoc = DOMDocument::loadXML(ByteReaderToString($byteReader));
    $mappingNodeList = $xmldoc->getElementsByTagName('PropertyMapping');
    for ($i=0; $i<$mappingNodeList->length; $i++) {
        $mapping = $mappingNodeList->item($i);
        $nameElt = $mapping->getElementsByTagName('Name');
        $name = $nameElt->item(0)->nodeValue;
        $valueElt = $mapping->getElementsByTagName('Value');
        $value = $valueElt->item(0)->nodeValue;
        $mappings[$name] = $value;
    }
    return $mappings;
}

function ByteReaderToString($byteReader)
{
    return $byteReader->ToString();
}

function printDateTime($mgDateTime)
{
   $dayToday = $mgDateTime->GetDay();
   $month = $mgDateTime->GetMonth();
   $year = $mgDateTime->GetYear();
   return $dayToday.".".$month.".".$year;
}

function GetParameters($params)
{
    global $mapName, $sessionId, $locale;

    $mapName = $params['MAPNAME'];
    $sessionId = $params['SESSION'];
    $locale = $params['LOCALE'];
}

function JsonEscape($str)
{
    return str_replace("\\", "\\\\", EscapeForHtml($str));
}

function JsonifyError($msg, $st)
{
    /*
    {
        "Error" : {
            "Message" : <exception-message>,
            "StackTrace" : <exception-stack-trace>
        }
    }
    */
    $emsg = JsonEscape($msg);
    $est = JsonEscape($st);
    $json = "{\"Error\":{\"Message\":\"$emsg\",\"StackTrace\":\"$est\"}}";
    return $json;
}

function UnescapeMagicQuotes($str)
{
    if(ini_get("magic_quotes_sybase") == "1")
        return str_replace("''", "'", $str);
    else if(get_magic_quotes_gpc() == "1")
    {
        //Unescape double quotes
        $str = str_replace('\\"', '"', $str);

        //remove additional backslash
        return str_replace("\\", "", $str);
    }
    return $str;
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>