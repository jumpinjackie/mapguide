<?php

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
	switch ($propertyType) {
		case MgPropertyType::Boolean :
			$val = $featureReader->GetBoolean($property);
			break;
		case MgPropertyType::Byte :
			$val = $featureReader->GetByte($property);
			break;
		case MgPropertyType::DateTime :
			$val = $featureReader->GetDateTime($property);
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

function CreateLayerDef($factory, $featureClass, $featureName, $geomName, $geomType)
{
	switch ($geomType) {
		case "point ":
			// Create a point rule
			$color = 'FFFF0000';
			$pointRule = $factory->CreatePointRule($color);
			$pointTypeStyle = $factory->CreatepointTypeStyle($pointRule);
			// Create a scale range
			$minScale = '0';
			$maxScale = '1000000000000';
			$scaleRange = $factory->CreateScaleRange($minScale, $maxScale, $pointTypeStyle);
			break;
		case "curve ":
			// Create a line rule
			$color = 'FF0000FF';
			$lineRule = $factory->CreateLineRule($color);
			// Create a line type style
			$lineTypeStyle = $factory->CreateLineTypeStyle($lineRule);
			// Create a scale range
			$minScale = '0';
			$maxScale = '1000000000000';
			$scaleRange = $factory->CreateScaleRange($minScale, $maxScale, $lineTypeStyle);
			break;
		case "surface ":
			// Create three area rules for thress different scale ranges
			$color = 'FFFFBF20';
			$areaRule = $factory->CreateAreaRule($color);
			// Create an area type style
			$areaTypeStyle = $factory->CreateAreaTypeStyle($areaRule);
			// Create a scale range
			$minScale = '0';
			$maxScale = '1000000000000';
			$scaleRange = $factory->CreateScaleRange($minScale, $maxScale, $areaTypeStyle);
			break;
	}
	
	$layerDefinition = $factory->CreateLayerDefinition($featureClass, $featureName, $geomName, $scaleRange);
	return $layerDefinition;
}

function CreateMapDef($factory, $className, $resId, $coordinate, $minX, $maxX, $minY, $maxY)
{
	// Inputs for Extents
	$extents = $factory->CreateExtents($minX, $maxX, $minY, $maxY);
	
	// Inputs for MapLayer
	$layerName = $className;
	$selectable = 'true';
	$showLegend = 'true';
	$legendLabel = $className;
	$expandLegend = 'true';
	$visible = 'true';
	$groupName = 'Geometry';
	$mapLayer = $factory->CreateMapLayer($layerName, $resId, $selectable, $showLegend, $legendLabel, $expandLegend, $visible, $groupName);
	
	// Inputs for MapLayerGroup
	$groupName = 'Geometry';
	$visible = 'true';
	$showLegend = 'true';
	$expandLegend = 'true';
	$legendLabel = 'Geometry';
	$mapLayerGroup = $factory->CreateMapLayerGroup($groupName, $visible, $showLegend, $expandLegend, $legendLabel);
	
	// Inputs for MapDefinition
	$mapName = 'Geometry Map';
	$backgroundColor = 'FFCDBD9C';
	$mapDefinition = $factory->CreateMapDefinition($mapName, $coordinate, $extents, $backgroundColor, $mapLayer, $mapLayerGroup);
	
	return $mapDefinition;
}

function CreateWebLay($factory, $resId)
{
	// Inputs for WebLayout
	$layoutName = 'Geometry Layout';
	$webLayout = $factory->CreateWebLayout($layoutName, $resId);
	
	return $webLayout;
}

function DisplayPaging($index, $resName, $schemaName, $className, $sessionId, $maxEntries, $currentPage, $maxPage, $lastEntry)
{
	// Display paging
	if($index!=0)
	{
	  	echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=0">&lt;&lt;FIRST</a>';
	  	echo '&nbsp;&nbsp;&nbsp;';
	  	echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . ($index-$maxEntries) . '">&lt;PREV</a>';
	}
	else
	  	echo '&lt;&lt;FIRST&nbsp&nbsp&nbsp&lt;PREV';
	  
	echo '&nbsp;&nbsp;&nbsp;Page ' . $currentPage . ' of ' . $maxPage . '&nbsp;&nbsp;&nbsp;';

	if($currentPage<$maxPage)
	{
	  	echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . ($index+$maxEntries) . '">NEXT&gt;</a>';
	  	echo '&nbsp;&nbsp;&nbsp;';
	  	echo '<a href="showclass.php?resId=' . $resName . '&schemaName=' . $schemaName . '&className=' . $className . '&sessionId=' . $sessionId . '&index=' . $lastEntry . '">LAST&gt;&gt;</a>';
	}
	else
	  	echo 'NEXT&gt;&nbsp;&nbsp;&nbsp;LAST&gt;&gt;';
}

?>
