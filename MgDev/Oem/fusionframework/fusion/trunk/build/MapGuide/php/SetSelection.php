<?php
/**
 * SetSelection
 *
 * $Id: $
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

include('Common.php');
include('Utilities.php');
include('../../common/php/Utilities.php');

    $mapName = "";
    $sessionId = "";
    $selText = "";
    $queryInfo = false;

    GetRequestParameters();

    try
    {
        //load the map runtime state
        //
        $map = new MgMap();
        $map->Open($resourceService, $mapName);

        // Create the selection set and save it
        $sel = new MgSelection($map);
        if($selText != "")
            $sel->FromXml($selText);
        $sel->Save($resourceService, $mapName);

        if($queryInfo) {
            //Query feature info for the feature in the selection set. This will return the current set
            //along with property info
            //There must be only one feature in the feature set
            $layers = $sel->GetLayers();
            if($layers == null || $layers->GetCount() != 1) {
                echo "Error: There must be exactly one feature in the set."; ///NOXLATE dbg report only
                return;
            }
            $layer = $layers->GetItem(0);
            $featureClassName = $layer->GetFeatureClassName();
            $filter = $sel->GenerateFilter($layer, $featureClassName);
            $featureSrvc = $siteConnection->CreateService(MgServiceType::FeatureService);
            $query = new MgFeatureQueryOptions();
            $query->SetFilter($filter);
            $featureSource = new MgResourceIdentifier($layer->GetFeatureSourceId());
            $features = $featureSrvc->SelectFeatures($featureSource, $featureClassName, $query);
            $featCount = 0;
            while($features->ReadNext()) 
            {
                if($featCount++ == 1)
                    break;
            }
            if($featCount != 1) {
                echo "Error: There must be exactly one feature in the set."; ///NOXLATE dbg report only
                return;
            }
            $renderingSrvc = $siteConnection->CreateService(MgServiceType::RenderingService);
            $layerNames = new MgStringCollection();
            $layerNames->Add($layer->GetName());
            $featInfo = $renderingSrvc->QueryFeatures($map, $layerNames, NULL, MgFeatureSpatialOperations::Intersects, $selText, 1, 2);
            //header('Content-Type: text/xml; charset: UTF-8');
            //echo $featInfo->ToXml()->ToString();
            header('Content-type: text/x-json');
            header('X-JSON: true');
            echo xml2json($featInfo->ToXml());
        }


    } catch(MgException $e) {
        echo "ClearSelection Exception: " . $e->GetDetails();
    }

function GetParameters($params)
{
    global $mapName, $sessionId, $selText, $queryInfo;

    $mapName = $params['mapname'];
    $sessionId = $params['session'];
    $selText = UnescapeMagicQuotes($params['selection']);
    if(isset($params['queryinfo']))
        $queryInfo = $params['queryinfo'] == "1";
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
