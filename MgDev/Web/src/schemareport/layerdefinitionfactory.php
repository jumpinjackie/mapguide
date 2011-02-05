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

class LayerDefinitionFactory
{

    static function CreateAreaRule($foreGroundColor)
    {
        $areaRule = file_get_contents("templatefiles/arearule.templ");
        $areaRule = sprintf($areaRule, $foreGroundColor);
        return $areaRule;
    }

    static function CreateAreaTypeStyle($areaRules)
    {
        $style = file_get_contents("templatefiles/areatypestyle.templ");
        $style = sprintf($style, $areaRules);
        return $style;
    }

    static function CreateLineRule($color)
    {
        $lineRule = file_get_contents("templatefiles/linerule.templ");
        $lineRule = sprintf($lineRule, $color);
        return $lineRule;
    }

    static function CreateLineTypeStyle($lineRules)
    {
        $lineStyle = file_get_contents("templatefiles/linetypestyle.templ");
        $lineStyle = sprintf($lineStyle, $lineRules);
        return $lineStyle;
    }

    static function CreateMarkSymbol($resourceId, $symbolName, $width, $height, $color)
    {
        $markSymbol = file_get_contents("templatefiles/marksymbol.templ");
        $markSymbol = sprintf($markSymbol, $width, $height, $resourceId, $symbolName, $color);
        return $markSymbol;
    }

    static function CreateTextSymbol($text, $fontHeight, $foregroundColor)
    {
        $textSymbol = file_get_contents("templatefiles/textsymbol.templ");
        $textSymbol = sprintf($textSymbol, $fontHeight, $fontHeight, $text, $foregroundColor);
        return $textSymbol;
    }

    static function CreatePointRule($color)
    {
        $pointRule = file_get_contents("templatefiles/pointrule.templ");
        $pointRule = sprintf($pointRule, $color);
        return $pointRule;
    }

    static function CreatePointTypeStyle($pointRule)
    {
        $pointTypeStyle = file_get_contents("templatefiles/pointtypestyle.templ");
        $pointTypeStyle = sprintf($pointTypeStyle, $pointRule);
        return $pointTypeStyle;
    }

    static function CreateScaleRange($minScale, $maxScale, $typeStyle)
    {
        $scaleRange = file_get_contents("templatefiles/scalerange.templ");
        $scaleRange = sprintf($scaleRange, $minScale, $maxScale, $typeStyle);
        return $scaleRange;
    }

    static function CreateLayerDefinition($resourceId, $featureClass, $geometry, $featureClassRange)
    {
        $layerDef = file_get_contents("templatefiles/layerdefinition.templ");
        $layerDef = sprintf($layerDef, $resourceId, $featureClass, $geometry, $featureClassRange);
        return $layerDef;
    }
}

?>
