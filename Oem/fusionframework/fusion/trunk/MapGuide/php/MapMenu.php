<?php
/**
 * MapMenu
 *
 * $Id: MapMenu.php 963 2007-10-16 15:37:30Z madair $
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
 * Purpose: a list of map definitions within a given repository folder
 *****************************************************************************/

include ("Common.php");

//Get the folder to search within
$root = (isset($_REQUEST['folder']))?$_REQUEST['folder']:'Library://';
$rootId = new MgResourceIdentifier($root);

//Enumerate elements of type MapDefinition
$maps = $resourceService->EnumerateResources($rootId, -1, 'MapDefinition');

//make a list of maps to query for names
$mapListXml = DOMDocument::loadXML(ByteReaderToString($maps));
//$aMapAssoc = Array();
$aMapIds = $mapListXml->getElementsByTagName('ResourceId');

//iterate over mapIds to retrieve names
for ( $i=0; $i < $aMapIds->length; $i++ ) { 
    $mapId = new MgResourceIdentifier($aMapIds->item($i)->nodeValue);
    $aPair['id'] = $aMapIds->item($i)->nodeValue;
    $aPair['name'] = $mapId->GetName(); 

    //Alternative - get the map description from the MapDefinition
    //$map = $resourceService->GetResourceContent($mapId);
    //$mapXml = DOMDocument::loadXML(ByteReaderToString($map));
    //$name = $mapXml->getElementsByTagName('Name')->item(0)->nodeValue;
    //$aPair['name'] = $name;
    
    $aMapAssoc[] = $aPair;
}
//output map list as xml
header('content-type: text/xml');
echo "<maps>";
for ( $i=0; $i < count($aMapAssoc); $i++ ){
    echo "<MapDefinition>";
    echo "<ResourceId>".$aMapAssoc[$i]['id']."</ResourceId>";
    echo "<Name>".$aMapAssoc[$i]['name']."</Name>";
    echo "</MapDefinition>";     
}
echo "</maps>";
exit;

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

?>
