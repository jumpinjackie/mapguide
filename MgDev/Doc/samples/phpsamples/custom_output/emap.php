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

include '../common/common.php';

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

  // Create an instance of ResourceService and use that to open the
  // current map instance stored in session state.

  $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
  $map = new MgMap();
  $map->Open($resourceService, $mapName);

  $dwfVersion = new MgDwfVersion("6.01", "1.2");

  $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);

  $mapAgentUri = 'http://' . $_SERVER['HTTP_HOST'] . '/mapguide/mapagent/mapagent.fcgi';
  $byteReader = $mappingService->GenerateMap($map, $mapAgentUri, $dwfVersion);

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
