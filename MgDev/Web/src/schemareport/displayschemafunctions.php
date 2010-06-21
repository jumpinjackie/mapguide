<?php

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

?>

<?php

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
