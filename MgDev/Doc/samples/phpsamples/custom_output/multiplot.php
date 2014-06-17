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

require_once '../common/common.php';

$args = ($_SERVER['REQUEST_METHOD'] == 'POST') ? $_POST : $_GET;
$sessionId = $args['SESSION'];
$mapName = $args['MAPNAME'];

try
{
  // Initialize the Web Extensions and connect to the server using
  // the Web Extensions session identifier stored in PHP session state.

  MgInitializeWebTier ($webconfigFilePath);

  $userInfo = new MgUserInformation($sessionId);
  $siteConnection = new MgSiteConnection();
  $siteConnection->Open($userInfo);

  $map = new MgMap($siteConnection);
  $map->Open($mapName);

  $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);

  $dwfVersion = new MgDwfVersion("6.01", "1.2");

  $plotSpec = new MgPlotSpecification(8.5, 11, MgPageUnitsType::Inches);
  $plotSpec->SetMargins(0.5, 0.5, 0.5, 0.5);

  $layout = null;
  $layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
  $layout =  new MgLayout($layoutRes, "City of Sheboygan", MgPageUnitsType::Inches);

  $plotCollection = new MgMapPlotCollection();

  $plot1 = new MgMapPlot($map, $plotSpec, $layout);
  $plot1->SetCenterAndScale($map->GetViewCenter()->GetCoordinate(), $map->GetViewScale() * 2);
  $plotCollection->Add($plot1);

  // Create a second map for the second sheet in the DWF. This second sheet uses the print layout
  // to display a page title and legend.

  $map2 = new MgMap($siteConnection);
  $map2->Create($map->GetMapDefinition(), 'Sheet 2');
  $plot2 = new MgMapPlot($map2, $plotSpec, $layout);
  $plot2->SetCenterAndScale($map->GetViewCenter()->GetCoordinate(), $map->GetViewScale());
  // $plot2 = new MgMapPlot($map2, $map->GetViewCenter()->GetCoordinate(), $map->GetViewScale(), $plotSpec, $layout);
  $plotCollection->Add($plot2);

  $byteReader = $mappingService->GenerateMultiPlot($plotCollection, $dwfVersion);

  // Now output the resulting DWF.

  $outputBuffer = '';
  $buffer = '';
  while ($byteReader->Read($buffer, 50000) != 0)
  {
    $outputBuffer .= $buffer;
  }

  header('Content-Type: ' . $byteReader->GetMimeType());
  header('Content-Length: ' . strlen($outputBuffer));
  echo $outputBuffer;

}
catch (MgException $e)
{
  $result = $e->GetExceptionMessage() . "<br><br>" . $e->GetDetails();

  header('Content-Type: text/html');
  header('Content-Length: ' . strlen($result));
  echo $result;
}

?>
