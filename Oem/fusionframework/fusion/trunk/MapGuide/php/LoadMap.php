<?php
/**
 * LoadMap
 *
 * $Id: LoadMap.php 1226 2008-02-14 20:57:26Z madair $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*****************************************************************************
 * Purpose: get map initial information
 *****************************************************************************/

include('Common.php');
include('Utilities.php');

try
{
    $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

    // Get a runtime map from a map definition
    if (isset($_REQUEST['mapid']))
    {
        $mapid = $_REQUEST['mapid'];
        //echo $mapid;
        $resourceID = new  MgResourceIdentifier($mapid);
        $map = new MgMap();
        $mapTitle = $resourceID->GetName();

        //echo "<br> maname $mapName <br>";

        $map->Create($resourceService, $resourceID, $mapTitle);

        $mapName = uniqid($mapTitle);
        $mapStateId = new MgResourceIdentifier("Session:" . $sessionID . "//" . $mapName . "." . MgResourceType::Map);


        //create an empty selection object and store it in the session repository
        $sel = new MgSelection($map);
        $sel->Save($resourceService, $mapName);


        $map->Save($resourceService, $mapStateId);
    } else {
        $map = new MgMap();
        $map->Open($resourceService, $mapName);
        $mapTitle = $map->GetName();
        $mapid = $map->GetMapDefinition()->ToString();
    }

    //$sessionId =  $map->GetSessionId();
    //$mapName = $map->GetName() ;
    $extents = $map->GetMapExtent();
    @$oMin = $extents->GetLowerLeftCoordinate();
    @$oMax = $extents->GetUpperRightCoordinate();

    @$srs = $map->GetMapSRS();
    if($srs != "")
    {
      @$csFactory = new MgCoordinateSystemFactory();
      @$cs = $csFactory->Create($srs);
      @$metersPerUnit = $cs->ConvertCoordinateSystemUnitsToMeters(1.0);
      //  $unitsType = $cs->GetUnits();
    }
    else
    {
      $metersPerUnit = 1.0;
      //$unitsType = "Meters";
    }   


    header('Content-type: text/x-json');
    header('X-JSON: true');
    echo "{";
    echo "sessionId:'$sessionID',";
    echo "mapId:'$mapid',";
    echo "metersPerUnit:$metersPerUnit,";
    //echo "mapTitle:'".addslashes(htmlentities($mapTitle))."',";
    //echo "mapName:'".addslashes(htmlentities($mapName))."',";
    echo "mapTitle:'".addslashes($mapTitle)."',";
    echo "mapName:'".addslashes($mapName)."',";
    echo "extent:[";
    echo $oMin->GetX().",";
    echo $oMin->GetY().",";
    echo $oMax->GetX().",";
    echo $oMax->GetY()."],";
    
    $layers=$map->GetLayers();
    
    echo "layers:[";
    $layerSep = '';
    for($i=0;$i<$layers->GetCount();$i++) 
    { 
        $layer=$layers->GetItem($i);

        //only output layers that are part of the 'Normal Group' and 
        //not the base map group used for tile maps.
        echo $layerSep.'{';
        OutputLayerInfo($layer, $resourceService, $featureService);
        echo '}';
        $layerSep = ',';
    } 
    echo "],"; 

    //Get layer groups as xml
    $groups = $map->GetLayerGroups();
    echo "groups:["; 
    $groupSep = '';
    for($i=0;$i<$groups->GetCount();$i++) 
    { 
        $group=$groups->GetItem($i);
        echo $groupSep.'{';
        OutputGroupInfo($group);
        echo '}';
        $groupSep = ',';
    } 
    echo"],"; 

    //FiniteDisplayScales for tiled maps
    echo "FiniteDisplayScales:[";
    for ($i=0; $i<$map->GetFiniteDisplayScaleCount(); $i++)
    {
        if ($i>0)
          echo ",";
        echo $map->GetFiniteDisplayScaleAt($i);
    }
    echo"]";
    
   
    echo "}";
}
catch (MgException $e)
{
  echo "ERROR: " . $e->GetMessage() . "\n";
  echo $e->GetDetails() . "\n";
  echo $e->GetStackTrace() . "\n";
}
exit;

function buildScaleRanges($layer) {
    global $resourceService;
    $resID = $layer->GetLayerDefinition();
    $layerContent = $resourceService->GetResourceContent($resID);

    $xmldoc = DOMDocument::loadXML(ByteReaderToString($layerContent));
    $type = 0;
    $scaleRanges = $xmldoc->getElementsByTagName('VectorScaleRange');
    if($scaleRanges->length == 0) {
        $scaleRanges = $xmldoc->getElementsByTagName('GridScaleRange');
        if($scaleRanges->length == 0) {
            $scaleRanges = $xmldoc->getElementsByTagName('DrawingLayerDefinition');
            if($scaleRanges->length == 0) {
                return;
            }
            $type = 2;
        } else {
            $type = 1;
        }
    }
    $typeStyles = array("PointTypeStyle", "LineTypeStyle", "AreaTypeStyle");
    $ruleNames = array("PointRule", "LineRule", "AreaRule", );
    $output = 'scaleRanges: [';
    $scaleSep = '';
    for($sc = 0; $sc < $scaleRanges->length; $sc++)
    {
        $scaleRange = $scaleRanges->item($sc);
        $minElt = $scaleRange->getElementsByTagName('MinScale');
        $maxElt = $scaleRange->getElementsByTagName('MaxScale');
        $minScale = "0";
        $maxScale = "'infinity'";  // as MDF's VectorScaleRange::MAX_MAP_SCALE
        if($minElt->length > 0)
            $minScale = $minElt->item(0)->nodeValue;
        if($maxElt->length > 0)
            $maxScale = $maxElt->item(0)->nodeValue;
            
        $output .= $scaleSep."{";
        $output .= "minScale:".$minScale.",";
        $output .= "maxScale:".$maxScale;
        
        if($type != 0) {
            $output .= "}";
            $scaleSep = ',';
            break;
        }
            
        $output .= ',styles:[';
        $styleIndex = 0;
        $styleSep = '';
        for($ts=0, $count = count($typeStyles); $ts < $count; $ts++)
        {
            $typeStyle = $scaleRange->getElementsByTagName($typeStyles[$ts]);
            $catIndex = 0;
            for($st = 0; $st < $typeStyle->length; $st++) {
                $rules = $typeStyle->item(0)->getElementsByTagName($ruleNames[$ts]);
                for($r = 0; $r < $rules->length; $r++) {
                    $rule = $rules->item($r);
                    $label = $rule->getElementsByTagName("LegendLabel");
                    $filter = $rule->getElementsByTagName("Filter");

                    $labelText = $label->length==1? $label->item(0)->nodeValue: "";
                    $filterText = $filter->length==1? $filter->item(0)->nodeValue: "";
                    $output .= $styleSep."{";
                    $output .= "legendLabel:'".addslashes(trim($labelText))."',";
                    $output .= "filter:'".addslashes(trim($filterText))."',";
                    $output .= "geometryType:".($ts+1).",";
                    $output .= "categoryIndex:".($catIndex++);
                    $output .= '}';
                    $styleSep = ',';
                }
            }
        }
        $output .= ']}';
        $scaleSep = ',';
    }
    $output .= ']';
    return $output;
}



// Converts a boolean to "yes" or "no"
// --from MG Web Tier API Reference
function BooleanToString($boolean)
{
    if (is_bool($boolean))
        return ($boolean ? "true" : "false");
    else
        return "'ERROR in BooleanToString.'";
}

function OutputGroupInfo($group)
{
    echo "groupName:'".addslashes($group->GetName())."',";
    echo "legendLabel:'".addslashes($group->GetLegendLabel())."',";
    echo "uniqueId:'".$group->GetObjectId()."',";
    echo "displayInLegend:".BooleanToString($group->GetDisplayInLegend()).",";
    echo "expandInLegend:".BooleanToString($group->GetExpandInLegend()).",";
    echo "layerGroupType:'".$group->GetLayerGroupType()."',";
    $parent = $group->GetGroup();
    echo "parentUniqueId:";
    echo $parent != null ? "'".$parent->GetObjectId()."'," : "null,";
    echo "visible:".BooleanToString($group->GetVisible()).",";
    echo "actuallyVisible:".BooleanToString($group->isVisible());
}

function OutputLayerInfo($layer, $resourceService, $featureService)
{
    $mappings = GetLayerPropertyMappings($resourceService, $layer);
    if (!isset($_SESSION['property_mappings'])) {
        $_SESSION['property_mappings'] = array();
    }
    $_SESSION['property_mappings'][$layer->GetObjectId()] = $mappings;
    $layerDefinition = $layer->GetLayerDefinition();
    $aLayerTypes = GetLayerTypes($featureService, $layer);
    //echo '<pre>'; print_r($aLayerTypes); echo '</pre>'; exit; 
        
    echo "propertyMappings:{";
    $sep = '';
    foreach($mappings as $name => $value) {
        echo $sep."'$name':'$value'";
        $sep = ',';
    }
    echo "},";
    echo "uniqueId:'".$layer->GetObjectId()."',";
    echo "layerName:'".addslashes($layer->GetName())."',";
    echo 'layerTypes:[';
    $sep = '';
    for ( $j=0; $j < count($aLayerTypes); $j++ )
    { 
        echo $sep . $aLayerTypes[$j];
        $sep = ',';
    }
    echo '],';
    echo "displayInLegend:".BooleanToString($layer->GetDisplayInLegend()).",";
    echo "expandInLegend:".BooleanToString($layer->GetExpandInLegend()).",";
    echo "resourceId:'".$layerDefinition->ToString()."',";
    echo "parentGroup:";
    echo $layer->GetGroup() ? "'".$layer->GetGroup()->GetObjectId()."'," : 'null,';
    echo "legendLabel:'".addslashes($layer->GetLegendLabel())."',";
    echo "selectable:".BooleanToString($layer->GetSelectable()).",";
    echo "visible:".BooleanToString($layer->GetVisible()).",";
    echo "actuallyVisible:".BooleanToString($layer->isVisible()).",";
    if (IsLayerEditable($resourceService, $layer)) {
        echo "editable:true,";
    } else {
        echo "editable:false,";
    }
    echo buildScaleRanges($layer);

}

?>
