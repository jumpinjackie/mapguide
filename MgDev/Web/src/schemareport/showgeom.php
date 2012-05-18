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

    include '../mapadmin/constants.php';
    include 'stringconstants.php';
    include 'displayschemafunctions.php';
    include 'layerdefinitionfactory.php';
    include 'mapdefinitionfactory.php';
    include 'weblayoutfactory.php';
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

    <head>
        <title><?php echo HtmlTitles::DisplaySchema ?></title>
        <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
        <link href="displayschema.css" rel="stylesheet" type="text/css">
    </head>

    <body onLoad="Confirmation()">

        <?php

            $sessionId = $_GET['sessionId'];
            $resName = $_GET['resId'];
            $schemaName = $_GET['schemaName'];
            $className = $_GET['className'];
            $geomName = $_GET['geomname'];
            $geomType = $_GET['geomtype'];
            $totalEntries = 0;
            $firstTime = true;
            $validSession = 1;
            $useBasicViewer = ($_GET['viewer'] == 'basic');
            $viewerRequest = '../schemareport/blank.php';
            
            try
            {
                $thisFile = __FILE__;
                $pos = strrpos($thisFile, '\\');
                if ($pos == false)
                    $pos = strrpos($thisFile, '/');
                $configFilePath = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
                MgInitializeWebTier ($configFilePath);

                $userInfo = new MgUserInformation($sessionId);
                $userInfo->SetClientIp(GetClientIp());
                $userInfo->SetClientAgent(GetClientAgent());
                $site = new MgSiteConnection();
                $site->Open($userInfo);

                $featureSrvc = $site->CreateService(MgServiceType::FeatureService);
                $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);
                $featuresId = new MgResourceIdentifier($resName);

                $schemaName = substr(strrchr($schemaName, "/"), 1);
                $featureName = $schemaName . ':' . $className;

                $classDef = $featureSrvc->GetClassDefinition($featuresId, $schemaName, $className);
                $geomProp = $classDef->GetProperties()->GetItem($geomName);
                $spatialContext = $geomProp->GetSpatialContextAssociation();

                //Try the SelectAggregate shortcut. This is faster than raw spinning a feature reader
                //
                //NOTE: If MapGuide supported scrollable readers like FDO, we'd have also tried 
                //that as well.
                $canCount = false;
                $gotCount = false;
                $fsBr = $resourceSrvc->GetResourceContent($featuresId);
                $fsXml = $fsBr->ToString();
                $fsDoc = DOMDocument::loadXML($fsXml);
                $providerNodeList = $fsDoc->getElementsByTagName("Provider");
                $providerName = $providerNodeList->item(0)->nodeValue;
                $capsBr = $featureSrvc->GetCapabilities($providerName);
                $capsXml = $capsBr->ToString();
                //This should be good enough to find out if Count() is supported
                $canCount = !(strstr($capsXml, "<Name>Count</Name>") === false);
                
                if ($canCount) {
                    $clsDef = $featureSrvc->GetClassDefinition($featuresId, $schemaName, $className);
                    $idProps = $clsDef->GetIdentityProperties();
                    if ($idProps->GetCount() > 0)
                    {
                        $pd = $idProps->GetItem(0);
                        $expr = "COUNT(" .$pd->GetName(). ")";
                        $query = new MgFeatureAggregateOptions();
                        $query->AddComputedProperty("TotalCount", $expr);
                        try 
                        {
                            $dataReader = $featureSrvc->SelectAggregate($featuresId, $featureName, $query);
                            if ($dataReader->ReadNext())
                            {
                                // When there is no data, the property will be null.
                                if($dataReader->IsNull("TotalCount"))
                                {
                                    $totalEntries = 0;
                                    $gotCount = true;
                                }
                                else
                                {
                                    $ptype = $dataReader->GetPropertyType("TotalCount");
                                    switch ($ptype)
                                    {
                                        case MgPropertyType::Int32:
                                            $totalEntries = $dataReader->GetInt32("TotalCount");
                                            $gotCount = true;
                                            break;
                                        case MgPropertyType::Int64:
                                            $totalEntries = $dataReader->GetInt64("TotalCount");
                                            $gotCount = true;
                                            break;
                                    }
                                    $dataReader->Close();
                                }
                            }
                        }
                        catch (MgException $ex) //Some providers like OGR can lie
                        {
                            $gotCount = false;
                        }
                    }
                }
                
                if ($gotCount == false)
                {
                    $featureReader = null;
                    try 
                    {
                        $featureReader = $featureSrvc->SelectFeatures($featuresId, $featureName, null);
                    }
                    catch (MgException $ex)
                    {
                        $totalEntries = -1; //Can't Count() or raw spin? Oh dear!
                    }
                    
                    if ($featureReader != null)
                    {
                        while($featureReader->ReadNext())
                            $totalEntries++;
                        $featureReader->Close();
                    }
                }
                
                // Create a layer definition
                $layerfactory = new LayerDefinitionFactory();
                $layerDefinition = CreateLayerDef($layerfactory, $resName, $featureName, $geomName, $geomType);

                // Save the layer definition to a resource stored in the session repository
                $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
                $byteSource->SetMimeType(MgMimeType::Xml);
                $resName = 'Session:' . $sessionId . '//' . $className . '.LayerDefinition';
                $resId = new MgResourceIdentifier($resName);
                $resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);

                $extentGeometryAgg = null;
                $extentGeometrySc = null;

                // Finds the coordinate system
                $agfReaderWriter = new MgAgfReaderWriter();
                $spatialcontextReader = $featureSrvc->GetSpatialContexts($featuresId, false);
                while ($spatialcontextReader->ReadNext())
                {
                    if ($spatialcontextReader->GetName() == $spatialContext)
                    {
                        $coordinate = $spatialcontextReader->GetCoordinateSystemWkt();

                        // Finds the extent
                        $extentByteReader = $spatialcontextReader->GetExtent();
                        break;
                    }
                }
                $spatialcontextReader->Close();
                if ($extentByteReader == null)
                {
                    // Get the extent geometry from the spatial context
                    $extentGeometrySc = $agfReaderWriter->Read($extentByteReader);
                }

                // Try to get the extents using the selectaggregate as sometimes the spatial context
                // information is not set
                $aggregateOptions = new MgFeatureAggregateOptions();
                $featureProp = 'SPATIALEXTENTS("' . $geomName . '")';
                $aggregateOptions->AddComputedProperty('EXTENTS', $featureProp);

                try
                {
                    $dataReader = $featureSrvc->SelectAggregate($featuresId, $className, $aggregateOptions);
                    if($dataReader->ReadNext())
                    {
                        // Get the extents information
                        $byteReader = $dataReader->GetGeometry('EXTENTS');
                        $extentGeometryAgg = $agfReaderWriter->Read($byteReader);
                    }
                    $dataReader->Close();
                }
                catch (MgException $e)
                {
                
                }
                
                $extentGeometry = null;
                // Prefer SpatialExtents() extent over spatial context extent
                if ($extentGeometryAgg != null)
                    $extentGeometry = $extentGeometryAgg;
                if ($extentGeometry == null) { //Stil null? Now try spatial context
                    if ($extentGeometrySc != null)
                        $extentGeometry = $extentGeometrySc;
                    else
                        throw new Exception(ErrorMessages::NullExtent);
                }
                // Get the coordinates
                $iterator = $extentGeometry->GetCoordinates();
                while($iterator->MoveNext())
                {
                    $x = $iterator->GetCurrent()->GetX();
                    $y = $iterator->GetCurrent()->GetY();
                    if($firstTime)
                    {
                        $maxX = $x;
                        $minX = $x;
                        $maxY = $y;
                        $minY = $y;
                        $firstTime = false;
                    }
                    if($maxX<$x)
                        $maxX = $x;
                    if($minX>$x||$minX==0)
                        $minX = $x;
                    if($maxY<$y)
                        $maxY = $y;
                    if($minY>$y||$minY==0)
                        $minY = $y;
                }

                // Create a map definition
                $mapfactory = new MapDefinitionFactory();
                $mapDefinition = CreateMapDef($mapfactory, $className, $resName, $coordinate, $minX, $maxX, $minY, $maxY);

                // Save the map definition to a resource stored in the session repository
                $byteSource = new MgByteSource($mapDefinition, strlen($mapDefinition));
                $byteSource->SetMimeType(MgMimeType::Xml);
                $resName = 'Session:' . $sessionId . '//' . $className . '.MapDefinition';
                $resId = new MgResourceIdentifier($resName);
                $resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);

                // Create a web layout
                $webfactory = new WebLayoutFactory();
                $webLayout = CreateWebLay($webfactory, $resName, $useBasicViewer);

                // Save the web layout to a resource stored in the session repository
                $byteSource = new MgByteSource($webLayout, strlen($webLayout));
                $byteSource->SetMimeType(MgMimeType::Xml);
                
                if($useBasicViewer)
                {
                    $resName = 'Session:' . $sessionId . '//' . $className . '.WebLayout';
                    $viewerRequest = '../mapviewerajax/?SESSION=' . $sessionId . '&WEBLAYOUT=' . $resName;
                }
                else
                {
                    $resName = 'Session:' . $sessionId . '//' . $className . '.ApplicationDefinition';
                    $viewerRequest = '../fusion/templates/mapguide/preview/indexNoLegend.html?SESSION=' . $sessionId . '&APPLICATIONDEFINITION=' . $resName;
                }
                $resId = new MgResourceIdentifier($resName);
                $resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);
            }
            catch (MgSessionExpiredException $s)
            {
                $validSession = 0;
                echo ErrorMessages::SessionExpired;
            }
            catch (MgException $mge)
            {
                $validSession = 0;
                echo $mge->GetExceptionMessage();
                echo $mge->GetStackTrace();
            }
            catch (Exception $e)
            {
                $validSession = 0;
                echo $e->GetMessage();
            }

        ?>

    </body>

    <script language="JavaScript">
        function Confirmation()
        {
            // checks for valid session
            if(<?php echo $validSession ?> > 0)
            {
                if(<?php echo $totalEntries ?> > <?php echo Constants::MaxFeatureBeforeConfirmation ?>)
                {
                    var answer = confirm("<?php echo sprintf(ConfirmationDialog::Preview, $totalEntries)?>");
                    if (answer)
                        location = '<?php echo $viewerRequest ?>';
                    else
                        location = '../schemareport/blank.php';
                }
                else
                    location = '<?php echo $viewerRequest ?>';
            }
        }
    </script>

</html>
