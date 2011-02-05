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

class MapDefinitionFactory
{
    static function CreateMapLayerGroup($name, $visible, $showLegend, $expandLegend, $legendLabel)
    {
        $mapLayerGroup = file_get_contents("templatefiles/maplayergroup.templ");
        $mapLayerGroup = sprintf($mapLayerGroup, $name, $visible, $showLegend, $expandLegend, $legendLabel);
        return $mapLayerGroup;
    }

    static function CreateMapLayer($name, $resId, $select, $showLegend, $legendLabel, $expandLegend, $visible, $group)
    {
        $mapLayer = file_get_contents("templatefiles/maplayer.templ");
        $mapLayer = sprintf($mapLayer, $name, $resId, $select, $showLegend, $legendLabel, $expandLegend, $visible, $group);
        return $mapLayer;
    }

    static function CreateExtents($minX, $maxX, $minY, $maxY)
    {
        $extents = file_get_contents("templatefiles/extents.templ");
        $extents = sprintf($extents, $minX, $maxX, $minY, $maxY);
        return $extents;
    }

    static function CreateMapDefinition($name, $coordinate, $extents, $backgroundColor, $mapLayer, $mapLayerGroup)
    {
        $mapDef = file_get_contents("templatefiles/mapdefinition.templ");
        $mapDef = sprintf($mapDef, $name, $coordinate, $extents, $backgroundColor, $mapLayer, $mapLayerGroup);
        return $mapDef;
    }
}

?>
