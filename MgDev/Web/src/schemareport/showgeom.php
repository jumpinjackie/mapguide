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

            $includeDebugWatermark = true;
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
                
                $classDef = $featureSrvc->GetClassDefinition($featuresId, $schemaName, $className);
                $clsProps = $classDef->GetProperties();
                $geomProp = $clsProps->GetItem($geomName);

                //TODO: Worse case scenario is no provider support for Count() or SpatialExtents()
                //In such cases, we should piggy-back off of the same raw-spun feature reader to compile the count and MBR
                //instead of having to raw-spin twice (once to get the count, another to get the MBR via EXTENT())
                //
                //TODO: Have a way to do this purely mgserver-side so we don't have row transmission overhead by doing the counting
                //on the web tier

                $totalEntries = GetFeatureCount($featuresId, $schemaName, $className, $resourceSrvc, $featureSrvc);
                $featureName = $schemaName . ':' . $className;
                
                // Create a layer definition
                $layerfactory = new LayerDefinitionFactory();
                $layerDefinition = CreateLayerDef($layerfactory, $resName, $featureName, $geomName, $geomType);

                // Save the layer definition to a resource stored in the session repository
                $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
                $byteSource->SetMimeType(MgMimeType::Xml);
                $resName = 'Session:' . $sessionId . '//' . $className . '.LayerDefinition';
                $resId = new MgResourceIdentifier($resName);
                $resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);

                $mbr = GetFeatureClassMBR($featuresId, $className, $geomProp, $featureSrvc);
                if ($mbr->extentGeometry == null)
                    throw new Exception(ErrorMessages::NullExtent);
                
                // Get the coordinates
                $iterator = $mbr->extentGeometry->GetCoordinates();
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
                if ($includeDebugWatermark) {
                    $mapDefinition = CreateDebugMapDef($mapfactory, $resourceSrvc, $className, $sessionId, $resName, $mbr->coordinateSystem, $minX, $maxX, $minY, $maxY, $totalEntries);
                } else {
                    $mapDefinition = CreateMapDef($mapfactory, $className, $resName, $mbr->coordinateSystem, $minX, $maxX, $minY, $maxY);
                }
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
