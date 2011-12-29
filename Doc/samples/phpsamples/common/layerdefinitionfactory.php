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

class LayerDefinitionFactory
{
    //Creates Area Rule
    //Parameters:
    //$foreGroundColor - color code for the foreground color
    //$legendLabel - string for the legend label
    //$filterText - filter string
    //$textSymbol - use textsymbol.templ to create it
    static function CreateAreaRule($legendLabel, $filterText, $foreGroundColor)
    {
        $areaRule = file_get_contents(dirname(__FILE__)."/../viewerfiles/arearule.templ");
        $areaRule = sprintf($areaRule, $legendLabel, $filterText, $foreGroundColor);
        return $areaRule;
    }

    //Creates AreaTypeStyle.
    //Parameters:
    //$areaRules - call CreateAreaRule to create area rules
    static function CreateAreaTypeStyle($areaRules)
    {
        $style = file_get_contents(dirname(__FILE__)."/../viewerfiles/areatypestyle.templ");
        $style = sprintf($style, $areaRules);
        return $style;
    }

    //Creates line rule
    //Parameters:
    //$color - color code for the line
    //$legendLabel - string for the legend label
    //$filter - filter string
    static function CreateLineRule($legendLabel, $filter, $color)
    {
        $lineRule = file_get_contents(dirname(__FILE__)."/../viewerfiles/linerule.templ");
        $lineRule = sprintf($lineRule, $legendLabel, $filter, $color);
        return $lineRule;
    }

    //Creates LineTypeStyle
    //Parameters:
    //$lineRules - call CreateLineRule to create line rules
    static function CreateLineTypeStyle($lineRules)
    {
        $lineStyle = file_get_contents(dirname(__FILE__)."/../viewerfiles/linetypestyle.templ");
        $lineStyle = sprintf($lineStyle, $lineRules);
        return $lineStyle;
    }

    //Creates mark symbol
    //Parameters:
    //$resourceId - resource identifier for the resource to be used
    //$symbolName - the name of the symbol
    //$width - the width of the symbol
    //$height - the height of the symbol
    //$color - color code for the symbol color
    static function CreateMarkSymbol($resourceId, $symbolName, $width, $height, $color)
    {
        $markSymbol = file_get_contents(dirname(__FILE__)."/../viewerfiles/marksymbol.templ");
        $markSymbol = sprintf($markSymbol, $width, $height, $resourceId, $symbolName, $color);
        return $markSymbol;
    }

    //Creates text symbol
    //Parameters:
    //$text - string for the text
    //$fontHeight - the height for the font
    //TODO:Can we pass it as a integer (ex. 10) or string (ex"10")
    //$foregroundColor - color code for the foreground color
    static function CreateTextSymbol($text, $fontHeight, $foregroundColor)
    {
        $textSymbol = file_get_contents(dirname(__FILE__)."/../viewerfiles/textsymbol.templ");
        $textSymbol = sprintf($textSymbol, $fontHeight, $fontHeight, $text, $foregroundColor);
        return $textSymbol;
    }

    //Creates a point rule
    //Parameters:
    //$pointSym - point symbolization. Use CreateMarkSymbol to create it
    //$legendlabel - string for the legend label
    //$filter - string for the filter
    //$label - use CreateTextSymbol to create it
    static function CreatePointRule($legendLabel, $filter, $label, $pointSym)
    {
        $pointRule = file_get_contents(dirname(__FILE__)."/../viewerfiles/pointrule.templ");
        $pointRule = sprintf($pointRule, $legendLabel, $filter, $label, $pointSym);
        return $pointRule;
    }

    //Creates PointTypeStyle
    //Parameters:
    //$pointRule - use CreatePointRule to define rules
    static function CreatePointTypeStyle($pointRule)
    {
        $pointTypeStyle = file_get_contents(dirname(__FILE__)."/../viewerfiles/pointtypestyle.templ");
        $pointTypeStyle = sprintf($pointTypeStyle, $pointRule);
        return $pointTypeStyle;
    }

    //Creates ScaleRange
    //Parameterss
    //$minScale - minimum scale
    //$maxScale - maximum scale
    //$typeStyle - use one CreateAreaTypeStyle, CreateLineTypeStyle, or CreatePointTypeStyle
    static function CreateScaleRange($minScale, $maxScale, $typeStyle)
    {
        $scaleRange = file_get_contents(dirname(__FILE__)."/../viewerfiles/scalerange.templ");
        $scaleRange = sprintf($scaleRange, $minScale, $maxScale, $typeStyle);
        return $scaleRange;
    }

    //Creates a layer definition
    //$resourceId - resource identifier for the new layer
    //featureClass - the name of the feature class
    //$geometry - the name of the geometry
    //$featureClassRange - use CreateScaleRange to define it.
    static function CreateLayerDefinition($resourceId, $featureClass, $geometry, $featureClassRange)
    {
        $layerDef = file_get_contents(dirname(__FILE__)."/../viewerfiles/layerdefinition.templ");
        $layerDef = sprintf($layerDef, $resourceId, $featureClass, $geometry, $featureClassRange);
        return $layerDef;
    }
}

?>
