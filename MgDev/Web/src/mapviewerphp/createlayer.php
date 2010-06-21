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

include("common.php");
include("constants.php");
require_once("layerdefinitionfactory.php");

InitializeWebTier();


//Area Rules
$areaRule = LayerDefinitionFactory::CreateAreaRule("TestArea1", 'SQFT &gt;= 0 AND SQFT &lt; 500', 'FF808080');
$areaRule .= LayerDefinitionFactory::CreateAreaRule("TestArea2", 'SQFT &gt;= 500 AND SQFT &lt; 1500', 'FF800080');
$areaRule .= LayerDefinitionFactory::CreateAreaRule("TestArea3", 'SQFT &gt;= 1500 AND SQFT &lt; 6000', 'FF808000');

$areaTypeStyle = LayerDefinitionFactory::CreateAreaTypeStyle($areaRule);

$scaleRange = LayerDefinitionFactory::CreateScaleRange('0', '1000000000000', $areaTypeStyle);


/*
//Line Rules
$lineRule = LayerDefinitionFactory::CreateLineRule("TestLine1", "", 'FF808000');

$lineTypeStyle = LayerDefinitionFactory::CreateLineTypeStyle($lineRule);

$scaleRange = LayerDefinitionFactory::CreateScaleRange('0', '1000000000000', $lineTypeStyle);
*/

/*
//Point Rules
$markSymbol = LayerDefinitionFactory::CreateMarkSymbol("", "", 5, 5, "FF800000");
$textSymbol = LayerDefinitionFactory::CreateTextSymbol("", "10", "FF808000");

$pointRule = LayerDefinitionFactory::CreatePointRule("TestPoint", "", $textSymbol, $markSymbol);
$pointStyle = LayerDefinitionFactory::CreatePointTypeStyle($pointRule);

$scaleRange = LayerDefinitionFactory::CreateScaleRange('0', '1000000000000', $pointStyle);
*/

//Create layer definition
$layerDef = LayerDefinitionFactory::CreateLayerDefinition('Library://NewParcels.LayerDefinition','Parcels','SHPGEOM', $scaleRange);

//Uncomment to check out the generated xml
//file_put_contents('test.xml',$layerDef);

$byteSource = new MgByteSource($layerDef, strlen($layerDef));
$byteReader = $byteSource->GetReader();

$cred = new MgUserInformation();
$cred->SetMgUsernamePassword('Anonymous', '');
$cred->SetClientIp(GetClientIp());
$cred->SetClientAgent(GetClientAgent());

$siteConn = new MgSiteConnection();
$siteConn->Open($cred);
$svc = $siteConn->CreateService(MgServiceType::ResourceService);
$resId = new MgResourceIdentifier('Library://Test/NewLayer.LayerDefinition');

try
{
    $svc->SetResource($resId,$byteReader,null);
}
catch (MgException $e)
{
    $errorMsg = EscapeForHtml($e->GetExceptionMessage());
    echo $errorMsg;
}
?>
