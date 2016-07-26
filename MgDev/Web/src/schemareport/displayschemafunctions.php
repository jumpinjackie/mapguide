<?php

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

?>

<?php

// Builds a MgFeatureQueryOptions with an explicit property list based
// on the given class definition
//
// This is to ensure that a "select * from featureclass" query from relational
// providers will have its column list bounded to what is recognized in the
// class definition. Some providers are known to produce "select * from table"
// queries that "leak" out column types that the provider does not know how to
// process. Using an explicit property list from its class definition will
// allow us to avoid such issues
function BuildFeatureQueryOptions($classDef)
{
    $query = new MgFeatureQueryOptions();
    $geomPropName = $classDef->GetDefaultGeometryPropertyName(); 
    $propertyList = $classDef->GetProperties(); 

    for ($i = 0; $i < $propertyList->GetCount(); $i++) 
    {
        $propertyDef = $propertyList->GetItem($i); 
        $property = $propertyList->GetItem($i)->GetName(); 

        if (($property != $geomPropName) && ($propertyDef->GetPropertyType() == MgFeaturePropertyType::DataProperty)) 
        { 
            $propertyType = $propertyList->GetItem($i)->GetDataType(); 
            switch ($propertyType) { 
                case MgPropertyType::Boolean: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Byte: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::DateTime: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Single: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Double: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Int16: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Int32: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::Int64: 
                    $query->AddFeatureProperty($property); 
                    break; 
                case MgPropertyType::String: 
                    $query->AddFeatureProperty($property); 
                    break; 
            }
        } else if ($property == $geomPropName){ 
            $query->AddFeatureProperty($property); 
        } 
    }
    return $query;
}

class MBR 
{
    public $coordinateSystem;
    public $extentGeometry;
}

function GetFeatureClassMBR($featuresId, $className, $geomProp, $featureSrvc)
{
    $extentGeometryAgg = null;
    $extentGeometrySc = null;
    $extentByteReader = null;
    
    $mbr = new MBR();
    $geomName = $geomProp->GetName();
    $spatialContext = $geomProp->GetSpatialContextAssociation();

    // Finds the coordinate system
    $agfReaderWriter = new MgAgfReaderWriter();
    $spatialcontextReader = $featureSrvc->GetSpatialContexts($featuresId, false);
    while ($spatialcontextReader->ReadNext())
    {
        if ($spatialcontextReader->GetName() == $spatialContext)
        {
            $mbr->coordinateSystem = $spatialcontextReader->GetCoordinateSystemWkt();

            // Finds the extent
            $extentByteReader = $spatialcontextReader->GetExtent();
            break;
        }
    }
    $spatialcontextReader->Close();
    if ($extentByteReader != null)
    {
        // Get the extent geometry from the spatial context
        $extentGeometrySc = $agfReaderWriter->Read($extentByteReader);
    }

    // Try to get the extents using the selectaggregate as sometimes the spatial context
    // information is not set
    $aggregateOptions = new MgFeatureAggregateOptions();
    $featureProp = 'SPATIALEXTENTS("' . $geomName . '")';
    $aggregateOptions->AddComputedProperty('EXTENTS', $featureProp);

    try
    {
        $dataReader = $featureSrvc->SelectAggregate($featuresId, $className, $aggregateOptions);
        if($dataReader->ReadNext())
        {
            // Get the extents information
            $byteReader = $dataReader->GetGeometry('EXTENTS');
            $extentGeometryAgg = $agfReaderWriter->Read($byteReader);
        }
        $dataReader->Close();
    }
    catch (MgException $e)
    {
        if ($extentGeometryAgg == null) 
        {
            //We do have one last hope. EXTENT() is an internal MapGuide custom function that's universally supported
            //as it operates against an underlying select query result. This raw-spins the reader server-side so there
            //is no server -> web tier transmission overhead involved.
            try
            {
                $aggregateOptions = new MgFeatureAggregateOptions();
                $aggregateOptions->AddComputedProperty("COMP_EXTENT", "EXTENT(".$geomName.")");
                
                $dataReader = $featureSrvc->SelectAggregate($featuresId, $className, $aggregateOptions);
                if($dataReader->ReadNext())
                {
                    // Get the extents information
                    $byteReader = $dataReader->GetGeometry('COMP_EXTENT');
                    $extentGeometryAgg = $agfReaderWriter->Read($byteReader);
                }
                $dataReader->Close();
            }
            catch (MgException $e2) 
            {
                
            }
        }
    }
    
    $mbr->extentGeometry = null;
    // Prefer SpatialExtents() of EXTENT() result over spatial context extent
    if ($extentGeometryAgg != null)
        $mbr->extentGeometry = $extentGeometryAgg;
    if ($mbr->extentGeometry == null) { //Stil null? Now try spatial context
        if ($extentGeometrySc != null)
            $mbr->extentGeometry = $extentGeometrySc;
    }
    return $mbr;
}

function GetFeatureCount($featuresId, $schemaName, $className, $resourceSrvc, $featureSrvc)
{
    //Try the SelectAggregate shortcut. This is faster than raw spinning a feature reader
    //
    //NOTE: If MapGuide supported scrollable readers like FDO, we'd have also tried 
    //that as well.
    $totalEntries = 0;
    $featureName = $schemaName . ":" . $className;
    $canCount = false;
    $gotCount = false;
    $fsBr = $resourceSrvc->GetResourceContent($featuresId);
    $fsXml = $fsBr->ToString();
    $fsDoc = new DOMDocument();
    $fsDoc->loadXML($fsXml);
    $providerNodeList = $fsDoc->getElementsByTagName("Provider");
    $providerName = $providerNodeList->item(0)->nodeValue;
    $capsBr = $featureSrvc->GetCapabilities($providerName);
    $capsXml = $capsBr->ToString();
    //This should be good enough to find out if Count() is supported
    $canCount = !(strstr($capsXml, "<Name>Count</Name>") === false);
    
    if ($canCount) {
        $clsDef = $featureSrvc->GetClassDefinition($featuresId, $schemaName, $className);
        $idProps = $clsDef->GetIdentityProperties();
        if ($idProps->GetCount() > 0)
        {
            $pd = $idProps->GetItem(0);
            $expr = "COUNT(" .$pd->GetName(). ")";
            $query = new MgFeatureAggregateOptions();
            $query->AddComputedProperty("TotalCount", $expr);
            try 
            {
                $dataReader = $featureSrvc->SelectAggregate($featuresId, $featureName, $query);
                if ($dataReader->ReadNext())
                {
                    // When there is no data, the property will be null.
                    if($dataReader->IsNull("TotalCount"))
                    {
                        $totalEntries = 0;
                        $gotCount = true;
                    }
                    else
                    {
                        $ptype = $dataReader->GetPropertyType("TotalCount");
                        switch ($ptype)
                        {
                            case MgPropertyType::Int32:
                                $totalEntries = $dataReader->GetInt32("TotalCount");
                                $gotCount = true;
                                break;
                            case MgPropertyType::Int64:
                                $totalEntries = $dataReader->GetInt64("TotalCount");
                                $gotCount = true;
                                break;
                        }
                        $dataReader->Close();
                    }
                }
            }
            catch (MgException $ex) //Some providers like OGR can lie
            {
                $gotCount = false;
            }
        }
    }
    
    if ($gotCount == false)
    {
        $featureReader = null;
        try 
        {
            $featureReader = $featureSrvc->SelectFeatures($featuresId, $featureName, null);
        }
        catch (MgException $ex)
        {
            $totalEntries = -1; //Can't Count() or raw spin? Oh dear!
        }
        
        if ($featureReader != null)
        {
            while($featureReader->ReadNext())
                $totalEntries++;
            $featureReader->Close();
        }
    }
    
    return $totalEntries;
}

function GetPropertyName($featureReader, $property, $propertyType)
{
    if($featureReader->IsNull($property))
    {
        $val = FormText::NullProperty;
        return $val;
    }

    switch ($propertyType) {
        case MgPropertyType::Boolean :
            $val = $featureReader->GetBoolean($property);
            break;
        case MgPropertyType::Byte :
            $val = $featureReader->GetByte($property);
            break;
        case MgPropertyType::DateTime :
            $dateTimeVal = $featureReader->GetDateTime($property);
            if($dateTimeVal != NULL)
                $val = PrintDateTime($dateTimeVal);
            else
                $val = NULL;
            break;
        case MgPropertyType::Single :
            $val = $featureReader->GetSingle($property);
            break;
        case MgPropertyType::Double :
            $val = $featureReader->GetDouble($property);
            break;
        case MgPropertyType::Int16 :
            $val = $featureReader->GetInt16($property);
            break;
        case MgPropertyType::Int32 :
            $val = $featureReader->GetInt32($property);
            break;
        case MgPropertyType::Int64 :
            $val = $featureReader->GetInt64($property);
            break;
        case MgPropertyType::String :
            $val = $featureReader->GetString($property);
            break;
        default : $val = NULL;
    }

    return $val;
}

function PrintDateTime($dT)
{
    $dateTimeString = '';

    if($dT->GetYear() != -1)
    {
        $dateTimeString .= $dT->GetYear() . '-';
        $dateTimeString .= $dT->GetMonth() . '-';
        $dateTimeString .= $dT->GetDay() . ' ';
    }
    if($dT->GetHour() != -1)
    {
        $dateTimeString .= $dT->GetHour() . ':';
        $dateTimeString .= $dT->GetMinute() . ':';
        $dateTimeString .= $dT->GetSecond() . '.';
        $dateTimeString .= $dT->GetMicrosecond();
    }

    return $dateTimeString;
}

function CreateLayerDef($factory, $featureClass, $featureName, $geomName, $geomType)
{
    $typeStyle = "";
    $geomTypes = explode(' ', $geomType);
    for($i=0;$i<(sizeof($geomTypes)-1);$i++)
    {
        switch ($geomTypes[$i]) {
            case "point":
                // Create a point rule
                $color = LayerDef::PointColor;
                $pointRule = $factory->CreatePointRule($color);
                $typeStyle .= $factory->CreatePointTypeStyle($pointRule);
                break;
            case "curve":
                // Create a line rule
                $color = LayerDef::CurveColor;
                $lineRule = $factory->CreateLineRule($color);
                // Create a line type style
                $typeStyle .= $factory->CreateLineTypeStyle($lineRule);
                break;
            case "surface":
                // Create three area rules for thress different scale ranges
                $color = LayerDef::SurfaceColor;
                $areaRule = $factory->CreateAreaRule($color);
                // Create an area type style
                $typeStyle .= $factory->CreateAreaTypeStyle($areaRule);
                break;
        }
    }

    // Create a scale range
    $minScale = LayerDef::MinScale;
    $maxScale = LayerDef::MaxScale;
    $scaleRange = $factory->CreateScaleRange($minScale, $maxScale, $typeStyle);
    $layerDefinition = $factory->CreateLayerDefinition($featureClass, $featureName, $geomName, $scaleRange);
    return $layerDefinition;
}

function CreateDebugMapDef($factory, $resourceSrvc, $className, $sessionId, $resId, $coordinate, $minX, $maxX, $minY, $maxY, $featureCount)
{
    // The full WKT probably won't fit on the map, so tidy it up for display on a map
    $csClean = str_replace("[", "[\n    ", $coordinate);
    $csClean = str_replace("],", "],\n", $csClean);
    // Make a debug text watermark containing relevant debugging information
    $message = sprintf(Debug::WatermarkDebugTemplate,
        $className,
        $featureCount,
        $minX,
        $minY,
        $maxX,
        $maxY,
        $csClean);
    $wmark = $factory->CreateTextWatermark($message);
    $wmidstr = "Session:$sessionId//".$className."_DEBUG.WatermarkDefinition";
    $wmid = new MgResourceIdentifier($wmidstr);
    $wmSource = new MgByteSource($wmark, strlen($wmark));
    $wmRdr = $wmSource->GetReader();
    $resourceSrvc->SetResource($wmid, $wmRdr, null);

    // Inputs for Extents
    $extents = $factory->CreateExtents($minX, $maxX, $minY, $maxY);

    // Inputs for MapLayer
    $layerName = $className;
    $selectable = MapDef::Selectable;
    $showLegend = MapDef::ShowLegend;
    $legendLabel = $className;
    $expandLegend = MapDef::ExpandLegend;
    $visible = MapDef::Visible;
    $groupName = MapDef::GroupName;
    $mapLayer = $factory->CreateMapLayer($layerName, $resId, $selectable, $showLegend, $legendLabel, $expandLegend, $visible, $groupName);

    // Inputs for MapLayerGroup
    $groupName = MapDef::GroupName;
    $visible = MapDef::Visible;
    $showLegend = MapDef::ShowLegend;
    $expandLegend = MapDef::ExpandLegend;
    $legendLabel = MapDef::LegendLabel;
    $mapLayerGroup = $factory->CreateMapLayerGroup($groupName, $visible, $showLegend, $expandLegend, $legendLabel);
    
    $dbgWatermarks = "<Watermarks><Watermark><Name>DEBUG</Name><ResourceId>$wmidstr</ResourceId></Watermark></Watermarks>";
    
    // Inputs for MapDefinition
    $mapName = MapDef::MapName;
    $backgroundColor = MapDef::BgColor;
    $mapDefinition = $factory->CreateMapDefinition($mapName, $coordinate, $extents, $backgroundColor, $mapLayer, $mapLayerGroup, $dbgWatermarks);

    return $mapDefinition;
}

function CreateMapDef($factory, $className, $resId, $coordinate, $minX, $maxX, $minY, $maxY)
{
    // Inputs for Extents
    $extents = $factory->CreateExtents($minX, $maxX, $minY, $maxY);

    // Inputs for MapLayer
    $layerName = $className;
    $selectable = MapDef::Selectable;
    $showLegend = MapDef::ShowLegend;
    $legendLabel = $className;
    $expandLegend = MapDef::ExpandLegend;
    $visible = MapDef::Visible;
    $groupName = MapDef::GroupName;
    $mapLayer = $factory->CreateMapLayer($layerName, $resId, $selectable, $showLegend, $legendLabel, $expandLegend, $visible, $groupName);

    // Inputs for MapLayerGroup
    $groupName = MapDef::GroupName;
    $visible = MapDef::Visible;
    $showLegend = MapDef::ShowLegend;
    $expandLegend = MapDef::ExpandLegend;
    $legendLabel = MapDef::LegendLabel;
    $mapLayerGroup = $factory->CreateMapLayerGroup($groupName, $visible, $showLegend, $expandLegend, $legendLabel);
    
    // Inputs for MapDefinition
    $mapName = MapDef::MapName;
    $backgroundColor = MapDef::BgColor;
    $mapDefinition = $factory->CreateMapDefinition($mapName, $coordinate, $extents, $backgroundColor, $mapLayer, $mapLayerGroup);

    return $mapDefinition;
}

function CreateWebLay($factory, $resId, $useBasicViewer)
{
    // Inputs for WebLayout
    $layoutName = WebLay::LayoutName;
    $webLayout = $factory->CreateWebLayout($layoutName, $resId, $useBasicViewer);

    return $webLayout;
}

function DisplayPaging($index, $resName, $schemaName, $className, $sessionId, $maxEntries, $currentPage, $maxPage, $lastEntry)
{
    // Display paging
    if($index!=0)
    {
        echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=0">&lt;&lt;' . PageNavigation::First . '</a>';
        echo '&nbsp;&nbsp;&nbsp;';
        echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . ($index-$maxEntries) . '">&lt;' . PageNavigation::Prev . '</a>';
    }
    else
        echo '&lt;&lt;' . PageNavigation::First . '&nbsp&nbsp&nbsp&lt;' . PageNavigation::Prev;

    echo '&nbsp;&nbsp;&nbsp;' . sprintf(PageNavigation::Current, $currentPage, $maxPage) . '&nbsp;&nbsp;&nbsp;';

    if($currentPage<$maxPage)
    {
        echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . ($index+$maxEntries) . '">' . PageNavigation::Next . '&gt;</a>';
        echo '&nbsp;&nbsp;&nbsp;';
        echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . $lastEntry . '">' . PageNavigation::Last . '&gt;&gt;</a>';
    }
    else
        echo PageNavigation::Next . '&gt;&nbsp;&nbsp;&nbsp;' . PageNavigation::Last . '&gt;&gt;';
}

function GetClientIp()
{
    $clientIp = "";
    if (array_key_exists('HTTP_CLIENT_IP', $_SERVER)
        && strcasecmp($_SERVER['HTTP_CLIENT_IP'], 'unknown') != 0)
    {
        $clientIp = $_SERVER['HTTP_CLIENT_IP'];
    }
    else if (array_key_exists('HTTP_X_FORWARDED_FOR', $_SERVER)
        && strcasecmp($_SERVER['HTTP_X_FORWARDED_FOR'], 'unknown') != 0)
    {
        $clientIp = $_SERVER['HTTP_X_FORWARDED_FOR'];
    }
    else if (array_key_exists('REMOTE_ADDR', $_SERVER))
    {
        $clientIp = $_SERVER['REMOTE_ADDR'];
    }
    return $clientIp;
}

function GetClientAgent()
{
    return "MapGuide Developer";

}

?>
