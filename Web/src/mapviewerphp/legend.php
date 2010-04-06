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

    include 'common.php';
    include 'constants.php';

    class TreeItem
    {
        public function __construct($name, $isGroup, $rtObject, $layerData)
        {
            $this->name = $name;
            $this->isGroup = $isGroup;
            $this->rtObject = $rtObject;
            $this->layerData = $layerData;
            if($isGroup)
                $this->children = array();
            else
                $this->children = null;
            $this->parent = null;
        }

        public function Attach($child)
        {
            if($this->children == null)
                $this->children = array();

            array_push($this->children, $child);
        }

        public $name;
        public $isGroup;
        public $rtObject;
        public $children;
        public $parentName;
        public $layerData;
    }

    $mapName = "";
    $sessionId = "";
    $summary = false;
    $layerCount = 0;
    $intermediateVar = 0;

    GetRequestParameters();

    $updateType = -1;
    $output = "\nvar layerData = new Array();\n";

    try
    {
        InitializeWebTier();

        // connect to the site and get a resource service instance
        //
        $userInfo = new MgUserInformation();
        $userInfo->SetMgSessionId($sessionId);
        $userInfo->SetClientIp(GetClientIp());
        $userInfo->SetClientAgent(GetClientAgent());

        $site = new MgSiteConnection();
        $site->Open($userInfo);
        $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);

        //Load the map runtime state.
        //
        $map = new MgMap();
        $map->Open($resourceSrvc, $mapName);

        $layerMap = null;
        $tree = BuildLayerTree($map, $resourceSrvc);

        if($summary)
        {
            $updateType = 0;
            // return only the layer structure, that is mainly groups/layers/layer-ids. Do not parse layer definitions.
            //
            BuildClientSideTree($tree, null, "null", false, "layerData", null, null);
        }
        else
        {
            if($layerCount == 0)
                $updateType = 1;
            else
            {
                $updateType = 2;
                $layerMap = BuildLayerMap($map);
            }
            BuildClientSideTree($tree, null, "null", true, "layerData", $resourceSrvc, $layerMap);
        }
    }
    catch(MgException $e)
    {
        $errorMsg = EscapeForHtml($e->GetMessage());
        $traceMsg = EscapeForHtml($e->GetStackTrace());
        echo $errorMsg . "<p>" . $traceMsg;
        return;
    }
    catch(Exception $ne)
    {
        return;
    }

    //output the HTML page
    $templ = file_get_contents("../viewerfiles/legendupdate.templ");
    printf($templ, $updateType, $output, GetSurroundVirtualPath() . "legend.php");

function BuildLayerTree($map, $resSrvc)
{
    $tree = array();
    $knownGroups = array();
    $unresolved = array();
    $groups = $map->GetLayerGroups();

    for($i = 0; $i < $groups->GetCount(); $i++)
    {
        $rtGroup = $groups->GetItem($i);
        $node = new TreeItem($rtGroup->GetName(), true, $rtGroup, null);
        $knownGroups[$node->name] = $node;

        $parentGroup = $rtGroup->GetGroup();
        if($parentGroup == null) {
            array_push($tree, $node);
        }
        else
        {
            $parentName = $parentGroup->GetName();
            $parentNode = $knownGroups[$parentName];
            if($parentNode != null)
                $parentNode->Attach($node);
            else
            {
                $node->parentName = $parentName;
                array_push($unresolved, $node);
            }
        }
    }
    if(count($unresolved) > 0)
    {
        for($i = 0; $i < $count($unresolved); $i++)
        {
            $node = $unresolved[$i];
            $parentNode = $knownGroups[$node->parentName];
            if($parentNode != null)
                $parentNode->Attach($node);
            else
                array_push($tree, $node); //should not happen. place group in the root if parent is not known
        }
    }
    // Get the layers
    $layers = $map->GetLayers();

    // Get the resource Ids of the layers
    $resIds = new MgStringCollection();
    for($i = 0; $i < $layers->GetCount(); $i++)
    {
        $rtLayer = $layers->GetItem($i);
        $resId = $rtLayer->GetLayerDefinition();
        $resIds->Add($resId->ToString());
    }
    $layersData = $resSrvc->GetResourceContents($resIds, null);

    for($i = 0; $i < $layers->GetCount(); $i++)
    {
        $rtLayer = $layers->GetItem($i);
        $node = new TreeItem($rtLayer->GetName(), false, $rtLayer, $layersData->GetItem($i));
        $parentGroup = $rtLayer->GetGroup();
        if($parentGroup == null)
            array_push($tree, $node);
        else
        {
            $parentNode = $knownGroups[$parentGroup->GetName()];
            if($parentNode != null)
                $parentNode->Attach($node);
            else
                array_push($tree, $node); //should not happen. place layer in the root if parent is not known
        }
    }

    return $tree;
}

function BuildClientSideTree($tree, $parent, $parentName, $fulldata, $container, $resSrvc, $layerMap)
{
    global $intermediateVar, $output;

    // 2 passes: pass 1 adds layers to the tree, pass 2 adds groups
    //
    $treeIndex = 0;
    for($pass = 0; $pass < 2; $pass++)
    {
        for($i = 0; $i < count($tree); $i++)
        {
            $node = $tree[$i];
            if($node->isGroup)
            {
                if($pass == 1)
                {
                    $groupName = "grp" . ($intermediateVar ++);
                    if($node->children != null)
                    {
                        $arrChildName = "c" . ($intermediateVar ++);
                        $output = $output . sprintf("var %s = new Array();\n", $arrChildName);
                    }
                    else
                        $arrChildName = "null";

                    if($fulldata)
                    {
                        $output = $output . sprintf("var %s = new GroupItem(\"%s\", %s, %s, %s, %s, \"%s\", \"%s\", %s);\n",
                                                        $groupName,
                                                        StrEscape($node->rtObject->GetLegendLabel()),
                                                        $node->rtObject->GetExpandInLegend()? "true": "false",
                                                        $parentName,
                                                        $node->rtObject->GetVisible()? "true": "false",
                                                        $node->rtObject->GetDisplayInLegend()? "true": "false",
                                                        $node->rtObject->GetObjectId(),
                                                        StrEscape($node->rtObject->GetName()),
                                                        $node->rtObject->GetLayerGroupType() == MgLayerGroupType::BaseMap? "true": "false");
                    }
                    else
                    {
                        $output = $output . sprintf("var %s = new GroupSummary(\"%s\", \"%s\", %s, %s);\n",
                                                        $groupName,
                                                        StrEscape($node->rtObject->GetName()),
                                                        $node->rtObject->GetObjectid(),
                                                        $arrChildName,
                                                        $parentName);
                    }
                    $output = $output . sprintf("%s[%d] = %s;\n", $container, $treeIndex, $groupName);
                    ++ $treeIndex;

                    if($node->children != null)
                    {
                        BuildClientSideTree($node->children, $node, $groupName, $fulldata, $arrChildName, $resSrvc, null);
                        $output = $output . sprintf("%s.children = %s;\n", $groupName, $arrChildName);
                    }
                }
            }
            else
            {
                if($pass == 0)
                {
                    $rtLayer = $node->rtObject;
                    if($fulldata)
                    {
                        $resId = $node->rtObject->GetLayerDefinition();
                        $layerData = $node->layerData;
                        $layerName = "lyr" . ($intermediateVar ++);
                        $objectId = $node->rtObject->GetObjectId();
                        $output = $output . sprintf("var %s = new LayerItem(\"%s\", \"%s\", %s, %s, %s, %s, %s, \"%s\", \"%s\", %s);\n",
                                                        $layerName,
                                                        StrEscape($node->rtObject->GetLegendLabel()),
                                                        $node->rtObject->GetName(),
                                                        $node->rtObject->GetExpandInLegend()? "true": "false",
                                                        $parentName,
                                                        $node->rtObject->GetVisible()? "true": "false",
                                                        $node->rtObject->GetDisplayInLegend()? "true": "false",
                                                        $node->rtObject->GetSelectable()? "true": "false",
                                                        $resId->ToString(),
                                                        $objectId,
                                                        $node->rtObject->GetLayerType() == MgLayerType::BaseMap? "true": "false");
                        $output = $output . sprintf("%s[%d] = %s;\n",
                                                        $container, $treeIndex,
                                                        $layerName);
                        ++ $treeIndex;

                        if($layerMap == null || !isset($layerMap[$objectId]))
                            BuildLayerDefinitionData($layerData, $layerName, $output);
                    }
                    else
                    {
                        $output = $output . sprintf("%s[%d] = new LayerSummary(\"%s\", \"%s\", \"%s\");\n",
                                                            $container, $i,
                                                            StrEscape($rtLayer->GetName()),
                                                            $rtLayer->GetObjectId(),
                                                            $rtLayer->GetLayerDefinition()->ToString());
                    }
                }
            }
        }
    }
}

function BuildLayerDefinitionData($layerData, $layerVarName, &$output)
{
    global $intermediateVar;

    $xmldoc = DOMDocument::loadXML($layerData);

    $type = 0;
    $scaleRanges = $xmldoc->getElementsByTagName('VectorScaleRange');
    if($scaleRanges->length == 0) {
        $scaleRanges = $xmldoc->getElementsByTagName('GridScaleRange');
        if($scaleRanges->length == 0) {
            $scaleRanges = $xmldoc->getElementsByTagName('DrawingLayerDefinition');
            if($scaleRanges->length == 0)
                return;
            $type = 2;
        }
        else
            $type = 1;
    }

    $typeStyles = array("PointTypeStyle", "LineTypeStyle", "AreaTypeStyle", "CompositeTypeStyle");
    $ruleNames = array("PointRule", "LineRule", "AreaRule", "CompositeRule");

    for($sc = 0; $sc < $scaleRanges->length; $sc++)
    {
        $scaleRange = $scaleRanges->item($sc);
        $minElt = $scaleRange->getElementsByTagName('MinScale');
        $maxElt = $scaleRange->getElementsByTagName('MaxScale');
        $minScale = "0";
        $maxScale = "1000000000000.0";  // as MDF's VectorScaleRange::MAX_MAP_SCALE
        if($minElt->length > 0)
            $minScale = $minElt->item(0)->nodeValue;
        if($maxElt->length > 0)
            $maxScale = $maxElt->item(0)->nodeValue;
        $scaleRangeVarName = "sc" . ($intermediateVar ++);
        $output = $output . sprintf("var %s = new ScaleRangeItem(%s, %s, %s);\n",
                                    $scaleRangeVarName,
                                    $minScale, $maxScale,
                                    $layerVarName);
        $output = $output . sprintf("%s.children[%d] = %s;\n", $layerVarName, $sc, $scaleRangeVarName);

        if($type != 0)
            break;

        $styleIndex = 0;
        for($ts=0, $count = count($typeStyles); $ts < $count; $ts++)
        {
            $typeStyle = $scaleRange->getElementsByTagName($typeStyles[$ts]);
            $catIndex = 0;
            for($st = 0; $st < $typeStyle->length; $st++)
            {
                // We will check if this typestyle is going to be shown in the legend
                $showInLegend = $typeStyle->item($st)->getElementsByTagName("ShowInLegend");
                if($showInLegend->length > 0)
                    if($showInLegend->item(0)->nodeValue == "false")
                        continue;   // This typestyle does not need to be shown in the legend

                $rules = $typeStyle->item(0)->getElementsByTagName($ruleNames[$ts]);
                for($r = 0; $r < $rules->length; $r++)
                {
                    $rule = $rules->item($r);
                    $label = $rule->getElementsByTagName("LegendLabel");
                    $filter = $rule->getElementsByTagName("Filter");

                    $labelText = $label->length==1? $label->item(0)->nodeValue: "";
                    $filterText = $filter->length==1? $filter->item(0)->nodeValue: "";

                    $output = $output . sprintf("%s.children[%d] = new StyleItem(\"%s\", \"%s\", %d, %d);\n",
                                                $scaleRangeVarName,
                                                $styleIndex++,
                                                StrEscape(trim($labelText)),
                                                StrEscape(trim($filterText)),
                                                $ts+1,
                                                $catIndex++);
                }
            }
        }
    }
    $output = $output . sprintf("%s.lyrtype = %d;\n", $layerVarName, $type);
}

function ByteReaderToString($byteReader)
{
    $buffer = '';
    do
    {
        $data = str_pad("\0", 50000, "\0");
        $len = $byteReader->Read($data, 50000);
        if ($len > 0)
        {
            $buffer = $buffer . substr($data, 0, $len);
        }
    } while ($len > 0);

    return $buffer;
}

function BuildLayerMap($map)
{
    $layerMap = array();
    $layers = $map->GetLayers();
    for($i = 0; $i < $layers->GetCount(); $i++)
    {
        $rtLayer = $layers->GetItem($i);
        $layerMap[$rtLayer->GetObjectId()] = $rtLayer;
    }
    return $layerMap;
}

function GetParameters($params)
{
    global $mapName, $sessionId, $summary, $layerCount;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));
    if(isset($params['SUMMARY']))
        $summary = true;
    else
    {
        $layerCount = GetIntParameter($params, 'LC');
    }
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

function StrEscape($str, $single=false)
{
    $c = $single? "'" : "\"";
    if(strpos($str, $c) < 0)
        return $str;

    return str_replace($c, "\\" . $c , $str);
}

?>
