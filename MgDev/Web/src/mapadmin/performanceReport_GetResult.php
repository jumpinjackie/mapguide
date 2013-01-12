<?php

try
{
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'performanceReport.php' );

    //sometimes the profiling map API needs a long time to response
    set_time_limit(900);

    // Define Local values
    $confirmationMsg = "";
    $errorMsg = "";
    $mapProfileResult = new MapProfileResult();
    $displayManager = new DisplayProfileResultManager();
    $mapResourceId = $_REQUEST["mapDefinition"];
    $centerPoint = $_REQUEST["centerPoint"];
    $scale = $_REQUEST["scale"];
    $clientWidth = $_REQUEST["imageWidth"];
    $clientHeigth = $_REQUEST["imageHeight"];

    function RemoveBaseLayers(& $map)
    {
        $layerCollection = $map->GetLayers();
        $n = 0;
        $bLayers = array();
        for ($i = 0; $i < $layerCollection->GetCount(); $i++)
        {
             $layer = $layerCollection->GetItem($i);
             $layerType = $layer->GetLayerType();

             if (MgLayerType::BaseMap == $layerType)
             {
                $bLayers[$n++] = $layer;
             }
        }

        for($j = 0; $j < count($bLayers); $j++)
        {
            $layerCollection->Remove($bLayers[$j]);
        }
    }

    function CheckMapExist()
    {
        global $mapResourceId;
        $mapNotExist = true;

        if(isset($mapResourceId))
        {
            try
            {
                global $userInfo;
                global $mapResources;
                $mapResourcesXml = "";

                // Enumerates all maps in the library
                $resourceID = new MgResourceIdentifier("Library://");
                //connect to the site and get a resource service instance
                $siteConn = new MgSiteConnection();
                $siteConn->Open($userInfo);
                $resourceService = $siteConn->CreateService(MgServiceType::ResourceService);

                $byteReader = $resourceService->EnumerateResources($resourceID, -1, "MapDefinition");

                $chunk = "";
                do
                {
                    $chunkSize = $byteReader->Read($chunk, 4096);
                    $mapResourcesXml = $mapResourcesXml . $chunk;
                } while ($chunkSize != 0);

                $resourceList = new DOMDocument();
                $resourceList->loadXML($mapResourcesXml);

                $resourceIdNodeList = $resourceList->documentElement->getElementsByTagName("ResourceId");

                for ($i = 0; $i < $resourceIdNodeList->length; $i++)
                {
                    $mapResourceID = $resourceIdNodeList->item($i)->nodeValue;

                    if(trim($mapResourceId) == trim($mapResourceID))
                    {
                        $mapNotExist = false;
                        break;
                    }
                }
            }
            catch (Exception $exc)
            {
                //true again
            }
        }

        if($mapNotExist)
        {
            echo "mapNotExist";
            exit(0);
        }
    }

    //get the background color of the given map resource id
    function GetBackGroundColor($resourceID)
    {
        $bgc;

        try
        {
            //site and userInfo are saved in the session
            global $userInfo;
            $mapResourceContent = "";

            //connect to the site and get a resource service instance
            $siteConn = new MgSiteConnection();
            $siteConn->Open($userInfo);
            $resourceService = $siteConn->CreateService(MgServiceType::ResourceService);
            //get the map resource content from the server
            $mgResourceID = new MgResourceIdentifier($resourceID);
            $byteReader = $resourceService->GetResourceContent($mgResourceID);

            //read the content into a string
            $chunk = "";
            do
            {
                $chunkSize = $byteReader->Read($chunk, 4096);
                $mapResourceContent = $mapResourceContent . $chunk;
            } while ($chunkSize != 0);

            //parse the xml data use DOMDocument
            $resourceContent = new DOMDocument();
            $resourceContent->loadXML($mapResourceContent);

            //get all the elements with the element name "BackgroundColor"
            $backgroundColor = $resourceContent->documentElement->getElementsByTagName("BackgroundColor");

            //if there's no "BackgroundColor" node
            if(0 == $backgroundColor->length)
            {
                $bgc = new MgColor(255, 255, 255, 255);
            }
            else
            {
                //the background color is saved as hex format string
                $tempBackgroundColor = $backgroundColor->item(0)->nodeValue;
                $red    = hexdec(substr($tempBackgroundColor, 0,2));
                $green  = hexdec(substr($tempBackgroundColor, 2,2));
                $blue   = hexdec(substr($tempBackgroundColor, 4,2));
                $alpha  = hexdec(substr($tempBackgroundColor, 6,2));

                $bgc = new MgColor($red, $green, $blue, $alpha);
            }
        }
        catch (Exception $exc)
        {
            $errorMsg = $exc->getMessage();
            $bgc = new MgColor(255, 255, 255, 255);
        }

        return $bgc;
    }

    function GetProfilingResults()
    {
        //profiling map parameters
        global $mapResourceId;
        global $mapProfileResult;
        global $scale;
        global $centerPoint;
        global $clientWidth;
        global $clientHeigth;

        list($x, $y) = explode("*", $centerPoint);
        $x = trim($x);
        $y = trim($y);

        $geometryFactory = new MgGeometryFactory();
        //[centerPoint]
        $coordNewCenter = $geometryFactory->CreateCoordinateXY($x,$y);

        //[backgroundColor]
        $bgc = GetBackGroundColor($mapResourceId);

        //[imageFormat]
        $imageFormat = "PNG";

        //userInfo are saved in the session
        global $userInfo;

        //connect to the site and get a resource service instance
        //create profiling service too
        $siteConn = new MgSiteConnection();
        $siteConn->Open($userInfo);

        $resourceService = $siteConn->CreateService(MgServiceType::ResourceService);
        $profilingService = $siteConn->CreateService(MgServiceType::ProfilingService);

        //the profiling result is saved as xml file, the file name is unique
        //the format is like this "YYYYMMDDHHMMSS10"
        $newXmlFileId = date("YmdHis") . rand(10, 99);

        $resourceID = new MgResourceIdentifier($mapResourceId);
        // Get a runtime map from a map definition
        $map = new MgMap();
        $map->Create($resourceService, $resourceID, $newXmlFileId);

        //The result will not show base layers
        RemoveBaseLayers($map);
        //get the profiling map result
        $byteReader = $profilingService->ProfileRenderMap($map, NULL, $coordNewCenter, $scale, $clientWidth, $clientHeigth, $bgc, $imageFormat, false);

        //read the content into a string
        $profilingResourceContent = "";
        $chunk = "";
        do
        {
            $chunkSize = $byteReader->Read($chunk, 4096);
            $profilingResourceContent = $profilingResourceContent . $chunk;
        } while ($chunkSize != 0);

        //save the file on the server
        $newXmlFileName = "profilingmapxml/".$newXmlFileId.".xml";
        $fp = fopen($newXmlFileName, "w");
        fwrite($fp, $profilingResourceContent);
        fclose($fp);

        //put the file name in the session
        $_SESSION["ProfilingResultFile"] = $newXmlFileName;

        //read the result into the DOM
        $resultSource = new DOMDocument();
        $resultSource->load($newXmlFileName);
        $mapProfileResult->ReadFromXML($resultSource);
        $mapProfileResult->GetBaseLayerCount();
    }

    CheckMapExist();
    GetProfilingResults();

    $displayManager->mapProfileResult = $mapProfileResult;

    $recentSettings = new RecentSettings();

    $recentSettings->SaveRecentSettings($mapResourceId,$centerPoint,$scale);
    
    //set cookie
    //save the recent setting in the cookie, so next time the user open 
    //the page will restore the last time setting
    //it will expire in 1 month
    setcookie("c_mapResourceId", $mapResourceId, time()+60*60*24*30);
    setcookie("c_centerPoint", $centerPoint, time()+60*60*24*30);
    setcookie("c_scale", $scale, time()+60*60*24*30);
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $errorMsg = $e->GetExceptionMessage();
}
catch ( Exception $e )
{
    $errorMsg = $e->getMessage();
}
?>

<table style="width: 70%;">
    <tr>
        <td colspan="2" style=" padding-left: 14px;">
            <br>
            <span style=" font-size: 18px; font-weight: bold; ">Results</span>
        </td>
    </tr>
    <tr>
        <td style=" padding-left: 15px;">
            <span><?php echo date("F d ,Y h:i:s A"); ?></span>
        </td>
        <td style=" text-align: right;">
            <input type="button" value="Export" style="width:80px; font-weight: bold;height: 28px;" onclick="ExportCSV();" />
        </td>
    </tr>
    <tr>
        <td colspan="2">
            <div style=" background-color: #EEEEEE; padding: 15px;">
                <table style="width:100%;" class="mapDefinitionResultTableStyle">
                    <tr>
                        <td style="width:22%; font-weight: bold;">Resource Name:</td>
                        <td style="width:32%; font-weight: bold;">
                            <?php
                                $displayManager->OutputMapResourceNameWithToolTip($mapProfileResult->MapProfileData->MapResourceId,false);
                            ?>
                        </td>
                        <td style="width:18%;font-weight: bold;">
                            Data Extents:
                        </td>
                        <td style="width:27%;">
                            <?php
                                //Data extents can be displayed in many different ways depending on the coordinate system used.
                                //Scientific E Notation is used to create a shorter and more managable string length.
                                function FormatDataExtents($value)
                                {
                                    if(strlen($value) > 10)
                                    {
                                        $value = sprintf("%+F",$value);
                                        $pos = strpos($value, ".");

                                        if ($pos >= 6 )
                                        {
                                           $value = sprintf("%0.4E", $value);
                                        }
                                        else
                                        {
                                            $value = number_format($value, 4,'.',' ');
                                        }
                                    }
                                    else
                                    {
                                        $value = number_format($value, 4,'.',' ');
                                    }

                                    return $value;
                                }

                                list($x1, $y1, $x2, $y2) = explode(",", $mapProfileResult->MapProfileData->DataExtents);

                                echo "X:" . FormatDataExtents($x1) . "&nbsp;&nbsp;&nbsp;Y:" . FormatDataExtents($y1);
                                echo "<br/>";
                                echo "X:" . FormatDataExtents($x2) . "&nbsp;&nbsp;&nbsp;Y:" . FormatDataExtents($y2);
                            ?>
                        </td>
                    </tr>
                    <tr>
                        <td style=" font-weight: bold;">Base Layers:</td>
                        <td>
                            <?php
                                echo $mapProfileResult->MapProfileData->BaseLayerCount;
                            ?>
                        </td>
                        <td style=" font-weight: bold;">Image Format:</td>
                        <td>
                            <?php
                            echo $mapProfileResult->MapProfileData->ImageFormat;
                            ?>
                        </td>
                    </tr>
                    <tr>
                        <td style=" font-weight: bold;">Center Point:</td>
                        <td>
                            <?php
                                list($x, $y) = explode("*", $centerPoint);
                                $x = trim($x);
                                $y = trim($y);
                                echo "X:&nbsp;" . $x;
                                echo "<br/>";
                                echo "Y:&nbsp;" . $y;
                            ?>
                        </td>
                        <td style=" font-weight: bold;">Layers:</td>
                        <td>
                            <?php
                                echo $mapProfileResult->MapProfileData->LayerCount;
                            ?>
                        </td>
                    </tr>
                    <tr>
                        <td style=" font-weight: bold;">Coordinate System:</td>
                        <td>
                            <span style="width:50px; overflow:hidden;">
                            <?php                         
                                echo $mapProfileResult->MapProfileData->CoordinateSystem;
                            ?>
                            </span>
                        </td>
                        <td  style=" font-weight: bold;">Renderer Type:</td>
                        <td>
                            <?php
                                echo $mapProfileResult->MapProfileData->RenderType;
                            ?>
                        </td>
                    </tr>
                    <tr>
                        <td></td>
                        <td></td>
                        <td style=" font-weight: bold;">Scale:</td>
                        <td>
                            <?php
                                echo "1:" . number_format($mapProfileResult->MapProfileData->Scale,4,"."," ");
                            ?>
                        </td>
                    </tr>
                </table>
            </div>
        </td>
    </tr>
    <tr>
        <td colspan="2">
            <?php
                $displayManager->OutputErrorMessage();
            ?>
            
        </td>
    </tr>
    <tr>
        <td colspan="2" style="padding-top:20px; padding-left: 15px;">
            <?php
                echo "<span>";
                echo "Total Generation Time:&nbsp;" . number_format($mapProfileResult->MapProfileData->TotalMapRenderTime,2) . "&nbsp;ms";
                echo "</span>";
            ?>
        </td>
    </tr>
    <tr>
        <td colspan="2">
            <div>
                <table style="width:100%; padding: 0px; text-align: center; table-layout: fixed;" cellspacing="0" cellpadding="0">
                    <?php
                        $displayManager->OutputMapRenderTimeGraph();
                    ?>
                    <tr style="height:35px;">
                        <td colspan="5" style=" text-align: center;">
                            <span style="width: 11px; height: 11px; background-color:#E4C7AE; margin-right: 5px;">
                                &nbsp;&nbsp;&nbsp;&nbsp;</span>
                            <span style=" font-size: small; margin-right: 20px;">Layers</span>
                            <span style="width: 11px; height: 11px; background-color:#AECBE4;margin-right: 5px;">
                                &nbsp;&nbsp;&nbsp;&nbsp;</span>
                            <span style=" font-size: small; margin-right: 20px;">Labels</span>
                            <span style="width: 11px; height: 11px; background-color:#E79661; margin-right: 5px;">
                                &nbsp;&nbsp;&nbsp;&nbsp;</span>
                            <span style=" font-size: small; margin-right: 20px;">Watermarks</span>
                            <span style="width: 11px; height: 11px; background-color:#BE76EE; margin-right: 5px;">
                                &nbsp;&nbsp;&nbsp;&nbsp;</span>
                            <span style=" font-size: small; margin-right: 20px;">Images</span>
                            <span style="width: 11px; height: 11px; background-color:#999999; margin-right: 5px;">
                                &nbsp;&nbsp;&nbsp;&nbsp;</span>
                            <span style=" font-size: small;">Other</span>
                        </td>
                    </tr>
                </table>
            </div>
        </td>
    </tr>
</table>
<span id="layerDetailsJsArray" style="display:none;">
    <?php
        $displayManager->OutputLayerDetailData();
    ?>
</span>
<div id="LayersResult">
    <table style=" width:100%;">
        <tr>
            <td style="width:70%; padding-top: 15px; vertical-align: top;">
                <div id="layerHeader" class="layerResultsHeaderStyle" style="margin:0px; padding: 0px;">
                    <table id="layerHeaderTable">
                        <thead>
                            <!--the table head is in a different div so when the table scrolls, the head will be fixed -->
                            <!--Also, the table head columns are set with some customer attribute to help to sort the table data from the client site-->
                            <tr>
                                <th style="width:20%;border-left:1px solid #CCCCCC;" columnIndex="1" onClick="SortLayers.sortByColumn(this);">Layer</th>
                                <th style="width:17%;" columnIndex="2" onClick="SortLayers.sortByColumn(this);">Render Time</th>
                                <th style="width:30%;" columnIndex="3" onClick="SortLayers.sortByColumn(this);">Feature Class</th>
                                <th style="width:21%;" columnIndex="4" onClick="SortLayers.sortByColumn(this);">Coordinate System</th>
                                <th style="width:12%;border-right:1px solid #CCCCCC;" columnIndex="5" onClick="SortLayers.sortByColumn(this);">Type</th>
                            </tr>
                        </thead>
                    </table>
                </div>
                <div id="layerBody" class="layerResultsStyle">
                    <table style="width:100%; text-align: center;margin:0px; padding: 0px; table-layout: fixed;" id="layerResultsTable">
                        <tbody>
                            <?php
                                $displayManager->OutputLayerDefinitionData();
                            ?>
                        </tbody>
                    </table>
                </div>
            </td>
            <td style="width:30%; padding-left:15px; vertical-align: top; padding-top: 15px;">
                <div id="layerInfoDetail">         
                </div>
            </td>
        </tr>
    </table>
</div>
<div id="tempRecentSettingDiv" style="display:none;">
    <?php
        $recentSettings->GetRecentSettings();
        $displayManager->OutputRecentSettings($recentSettings->recentSettings);
    ?>
</div>
<span id="tempRecentSettingsJsArray" style="display:none;">
    <?php
        $displayManager->OutputSettingsJsArray($recentSettings->recentSettings);
    ?>
</span>

