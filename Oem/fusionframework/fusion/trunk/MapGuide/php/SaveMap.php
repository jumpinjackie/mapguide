<?php
/**
 * SaveMap
 *
 * $Id: SaveMap.php 1396 2008-05-08 15:34:30Z madair $
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
$format     = isset($_REQUEST['format']) ? $_REQUEST['format'] : 'png';
$layout     = isset($_REQUEST['layout']) ? $_REQUEST['layout'] : null;
$scale      = isset($_REQUEST['scale']) ? $_REQUEST['scale'] : null;
$imgWidth   = isset($_REQUEST['width']) ? $_REQUEST['width'] : null;
$imgHeight  = isset($_REQUEST['height']) ? $_REQUEST['height'] : null;
$pageHeight = isset($_REQUEST['pageheight']) ? $_REQUEST['pageheight'] : 11;
$pageWidth  = isset($_REQUEST['pagewidth']) ? $_REQUEST['pagewidth'] : 8.5;
$aMargins = isset($_REQUEST['margins']) ? explode(',',$_REQUEST['margins']) : array(0,0,0,0);

try
{
    $mappingService = $siteConnection->CreateService(MgServiceType::MappingService);
    $renderingService = $siteConnection->CreateService(MgServiceType::RenderingService);
    $map = new MgMap();
    $map->Open($resourceService, $mapName);
    
    $selection = new MgSelection($map);
    $selection->Open($resourceService, $mapName);
    
    //compute center
    $extent = $map->GetMapExtent();
    $centerX = $extent->GetLowerLeftCoordinate()->GetX() + ($extent->GetWidth())/2;
    $centerY = $extent->GetLowerLeftCoordinate()->GetY() + ($extent->GetHeight())/2;
    $geomFactory = new MgGeometryFactory();
    $center = $geomFactory->CreateCoordinateXY($centerX, $centerY);

    if ($format == 'DWF') {
        $oLayout = null;
        if ($layout) {
            $layoutId = new MgResourceIdentifier($layout);
            $layoutId->Validate();
            $oLayout = new MgLayout($layoutId,'Map', 'meters');
        };
        $oPlotSpec = new MgPlotSpecification($pageWidth,$pageHeight,MgPageUnitsType::Inches,
                                            $aMargins[0],
                                            $aMargins[1],
                                            $aMargins[2],
                                            $aMargins[3]
                                            );
        
        $dwfVersion = new MgDwfVersion('6.01','1.2');
        
        if ($scale) {
            //plot with the passed scale
            
            $coordSysFactory = new MgCoordinateSystemFactory();
            $coordSystem = $coordSysFactory->Create($map->GetMapSRS());
            $metersPerUnit = $coordSystem->ConvertCoordinateSystemUnitsToMeters(1.0);
            $metersPerPixel = 1.0/(100.0 / 2.54 * $map->GetDisplayDpi());

            $height = $map->GetDisplayHeight();
            $width = $map->GetDisplayWidth();
            $mapWidth = $scale * $width * $metersPerPixel/$metersPerUnit;
            $mapHeight = $scale * $height * $metersPerPixel/$metersPerUnit;
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
        //render as an image
        if (isset($imgHeight) && isset($imgWidth)) {
            $scale = $map->GetViewScale();
            $oImg = $renderingService->RenderMap($map, $selection,
                                                 $center, $scale,
                                                 $imgWidth, $imgHeight,
                                                 new MgColor(255,255,255),
                                                 $format);
        }else{
            $oImg = $renderingService->RenderMap($map, $selection, $format);
        };
    };
}
catch (MgException $e)
{
  echo "ERROR: " . $e->GetMessage() . "\n";
  echo $e->GetDetails() . "\n";
  echo $e->GetStackTrace() . "\n";
  exit;
}

header("Cache-Control: no-store, no-cache, must-revalidate");  // HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
header( "Content-type: image/$format" );
header( "Content-disposition: attachment; filename=$mapName.$format" );

$buffer = '';
while ($oImg->Read($buffer, 4096) > 0) {
    echo $buffer;
}
?>
