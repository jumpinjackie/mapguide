<?php
/**
 * SaveMap
 *
 * $Id: SaveMap.php 963 2007-10-16 15:37:30Z madair $
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
 * Purpose: get map initial information
 *****************************************************************************/

include('Common.php');
$format = isset($_REQUEST['format']) ? $_REQUEST['format'] : 'png';
$layout = isset($_REQUEST['layout']) ? $_REQUEST['layout'] : null;
$scale  = isset($_REQUEST['scale']) ? $_REQUEST['scale'] : null;

try
{
    $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);
    $renderingService = $siteConnection->CreateService(MgServiceType::RenderingService);
    $map = new MgMap();
    $map->Open($resourceService, $mapName);
    
    $selection = new MgSelection($map);
    $selection->Open($resourceService, $mapName);
    
    if ($format == 'DWF') {
        $extent = $map->GetMapExtent();
        $oLayout = null;
        if ($layout) {
            $layoutId = new MgResourceIdentifier($layout);
            $layoutId->Validate();
            $oLayout = new MgLayout($layoutId,'Map', 'meters');
        }
        $oPlotSpec = new MgPlotSpecification(8.5,11,MgPageUnitsType::Inches);
        
        $dwfVersion = new MgDwfVersion('6.01','1.2');
        
        if ($scale) {
            //compute center point and plot with the passed scale
        
            $centerX = $extent->GetLowerLeftCoordinate()->GetX() + ($extent->GetWidth())/2;
            $centerY = $extent->GetLowerLeftCoordinate()->GetY() + ($extent->GetHeight())/2;
            $geomFactory = new MgGeometryFactory();
            $center = $geomFactory->CreateCoordinateXY($centerX, $centerY);
            
            //echo $centerX.", ".$centerY;exit;
            //$metersPerUnit = $map->GetMetersPerUnit();
            $coordSysFactory = new MgCoordinateSystemFactory();
            $coordSystem = $coordSysFactory->Create($map->GetMapSRS());
            $metersPerUnit = $coordSystem->ConvertCoordinateSystemUnitsToMeters(1.0);
            $metersPerPixel = 1.0/(100.0 / 2.54 * $map->GetDisplayDpi());
            //echo $metersPerPixel; exit;

            $height = $map->GetDisplayHeight();
            $width = $map->GetDisplayWidth();
            $mapWidth = $scale * $width * $metersPerPixel/$metersPerUnit;
            $mapHeight = $scale * $height * $metersPerPixel/$metersPerUnit;
            //echo $mapWidth.", ".$mapHeight;exit;
            
            // $lowerLeft = $geomFactory->CreateCoordinateXY($center->GetX() - 0.5*$mapWidth,
            //                                               $center->GetY() - 0.5*$mapHeight);
            // $topRight = $geomFactory->CreateCoordinateXY($center->GetX() + 0.5*$mapWidth,
            //                                               $center->GetY() + 0.5*$mapHeight);
            $extent = new MgEnvelope( 
                                      $center->GetX() - 0.5*$mapWidth,
                                      $center->GetY() - 0.5*$mapHeight,
                                      $center->GetX() + 0.5*$mapWidth,
                                      $center->GetY() + 0.5*$mapHeight);
            //this is broken because the swig api has an error
            //$center = $map->GetViewCenter();
            // echo $oPlotSpec; exit;
            //echo $map->GetName()." ".$center->GetX().", ".
            //                        $center->GetY().", ".
            //                        $scale."  paper height: ".
            //$oPlotSpec->GetPaperHeight().", layout: ".$oLayout->GetTitle().", dwfVersion ".$dwfVersion->GetFileVersion()."\n";
            // $oImg = $mappingService->GeneratePlot($map,
            //                                   $center,
            //                                   $scale,
            //                                   $oPlotSpec,
            //                                   $oLayout,
            //                                   $dwfVersion);
            
            $oImg = $mappingService->GeneratePlot($map, $extent, true,
                                              $oPlotSpec,
                                              $oLayout,
                                              $dwfVersion);
            
        } else {
            //use current extents for plot
            $oImg = $mappingService->GeneratePlot($map, $extent, false,
                                              $oPlotSpec,
                                              $oLayout,
                                              $dwfVersion);
        }
    } else {
        $oImg = $renderingService->RenderMap($map, $selection, $format);
    }    
}
catch (MgException $e)
{
  echo "ERROR: " . $e->GetMessage() . "\n";
  echo $e->GetDetails() . "\n";
  echo $e->GetStackTrace() . "\n";
  exit;
}

/*
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
                                             // always modified
header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          // HTTP/1.0
header( "Content-type: application/octet-stream" );
header( "Content-Disposition: attachment; filename=$mapName.png" );
*/
header( "Content-type: image/$format" );
header( "Content-disposition: attachment; filename=$mapName.$format" );

$buffer = '';
while ($oImg->Read($buffer, 4096) > 0) {
    echo $buffer;
}
?>
