<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

// This script plots the current view of the map, optionally
// using the Scale parameter if Scale > 0.

include '../utilityfunctions.php';

$mgSessionId = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['SESSION']: $_GET['SESSION'];

try
{
    // Initialize the Web Extensions and connect to the erver using
    // the Web Extensions session identifier stored in PHP session state.

    MgInitializeWebTier ($configFilePath);

    $userInfo = new MgUserInformation($mgSessionId);
    $siteConnection = new MgSiteConnection();
    $siteConnection->Open($userInfo);

    $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);

    // Create an instance of ResourceService and use that to open the
    // current map instance stored in session state.

    $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
    $map = new MgMap();
    $map->Open($resourceService, 'Sheboygan');

    // Now create an instance of MappingService and use it to plot the
    // current view of the map.

    $dwfVersion = new MgDwfVersion("6.01", "1.2");
    $plotSpec = new MgPlotSpecification(8.5, 11, MgPageUnitsType::Inches);
    $plotSpec->SetMargins(0.5, 0.5, 0.5, 0.5);

    $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);

    $layout = null;
    if (array_key_exists('UseLayout', $_GET) && $_GET['UseLayout'] == 'true')
    {
        $layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
        $layout =  new MgLayout($layoutRes, "City of Sheboygan", MgPageUnitsType::Inches);
    }

    $scale = doubleval($_GET['Scale']);
    if ($scale <= 0)
    {
        $byteReader = $mappingService->GeneratePlot($map, $plotSpec, $layout, $dwfVersion);
    }
    else
    {
        $mapCenter = $map->GetViewCenter()->GetCoordinate();
        $byteReader = $mappingService->GeneratePlot($map, $mapCenter, $scale, $plotSpec, $layout, $dwfVersion);
    }

    // Now output the resulting DWF.

    OutputReaderContent($byteReader);
}
catch (MgException $e)
{
    $result = $e->GetExceptionMessage() . "<br><br>" . $e->GetDetails();

    header('Content-Type: text/html');
    header('Content-Length: ' . strlen($result));
    echo $result;
}

?>
