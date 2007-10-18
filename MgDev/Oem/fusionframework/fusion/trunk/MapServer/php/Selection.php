<?php
/**
 * Selection
 *
 * $Id: Selection.php 963 2007-10-16 15:37:30Z madair $
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
 * Purpose: Get all attribute informations for elements in the 
 * current selection
 *****************************************************************************/

/* set up the session */
include ("Common.php");
include ("Utilities.php");


if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
}


if (isset($_REQUEST['queryfile']) && $_REQUEST['queryfile'] != "") 
{
  $oMap->loadquery($_REQUEST['queryfile']);
}

header('Content-type: text/xml');

$nLayers = $oMap->numlayers;

$nSelectedLayers =0;
$nTotalElements = 0;
$totalminx =  0;
$totalminy =  0;
$totalmaxx =  0;
$totalmaxy =  0;
echo "<Selection>";
for ($i=0; $i<$nLayers; $i++)
{
    $oLayer = $oMap->GetLayer($i);
    $numResults = $oLayer->getNumResults();
    if ($numResults == 0)
      continue;

    $nLayerType = 0;
    if ($oLayer->type ==  MS_LAYER_POINT || $oLayer->type ==  MS_LAYER_ANNOTATION)
      $nLayerType = 0;
    if ($oLayer->type ==  MS_LAYER_LINE)
       $nLayerType = 1;   
    if ($oLayer->type ==  MS_LAYER_POLYGON)
       $nLayerType = 2;
    
    

    $oLayer->open();
    $nSelectedLayers++;
    echo "<Layer>";
    $name = $oLayer->name != "" ? $oLayer->name : "Layer_".$i; 
    echo "<Name>$name</Name>";
    
    /*get first shape to get the attributes*/
    $oRes = $oLayer->getResult(0);
    $oShape = $oLayer->getShape($oRes->tileindex,$oRes->shapeindex);
    $selFields = array();
    while ( list($key,$val) = each($oShape->values) ) 
      array_push($selFields, $key);

    echo "<PropertiesNumber>".count($selFields)."</PropertiesNumber>";
    echo "<PropertiesNames>".implode(",", $selFields)."</PropertiesNames>";
    //we do not know the types of the attributes in MS. Just outout 0
    $aTmp = array();
    for($iField=0; $iField < count($selFields); $iField++)
      $aTmp[$iField] = 0;
    echo "<PropertiesTypes>".implode(",", $aTmp)."</PropertiesTypes>";
    
    for ($iRes=0; $iRes < $numResults; $iRes++)
    {
        $oRes = $oLayer->getResult($iRes);
        $oShape = $oLayer->getShape($oRes->tileindex,$oRes->shapeindex);
        //TODO : area, length and distance are not set
        $minx =  $oShape->bounds->minx;
        $miny =  $oShape->bounds->miny;
        $maxx =  $oShape->bounds->maxx;
        $maxy =  $oShape->bounds->maxy;
        if ($nTotalElements == 0)
        {
            $totalminx =  $minx;
            $totalminy =  $miny;
            $totalmaxx =  $maxx;
            $totalmaxy =  $maxy;
        }
        else
        {
            if ($totalminx > $minx)
              $totalminx = $minx;
            if ($totalminy > $miny)
              $totalminy = $miny;
            if ($totalmaxx < $maxx)
              $totalmaxx = $maxx;
            if ($totalmaxy < $maxy)
              $totalmaxy = $maxy;
        }

        echo "<ValueCollection type=\"$nLayerType\" bbox=\"$minx,$miny,$maxx,$maxy\" center=\"0\" area=\"0\" distance=\"0\">";
        for($iField=0; $iField < count($selFields); $iField++)
        {
            echo "<v>". $oShape->values[$selFields[$iField]] . "</v>";
        }
        echo "</ValueCollection>";
    }
    $nTotalElements += $numResults;
    echo "<ElementsSelected>$numResults</ElementsSelected>"; 
    echo "</Layer>";
    
    $oLayer->close();
}

if ($nSelectedLayers > 0)
{
    echo "<NumberOfLayers>1</NumberOfLayers>"; 
    echo "<TotalElementsSelected>$nTotalElements</TotalElementsSelected>"; 
    echo "<minx>$totalminx</minx>"; 
    echo "<miny>$totalminy</miny>"; 
    echo "<maxx>$totalmaxx</maxx>"; 
    echo "<maxy>$totalmaxy</maxy>"; 

}
else
{
    echo "false";
}
echo "</Selection>";

?>
