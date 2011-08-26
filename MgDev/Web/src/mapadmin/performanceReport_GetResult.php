<?php

try
{
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'performanceReport.php' );

    // Define Local values
    $confirmationMsg = "";
    $errorMsg = "";
    $mapProfileResult=new MapProfileResult();
    $displayManager= new DisplayProfileResultManager();

    function GetProfilingResults()
    {
        global $mapProfileResult;
        $resultSource=new DOMDocument();
        //As the profiling API is not finished, now we just use a temp xml file to simulate it.
        //we will change it in part 3.
        $resultSource->load("profilingmapxml/profileResults.xml");
        $mapProfileResult->ReadFromXML($resultSource);
        $mapProfileResult->GetBaseLayerCount();
    }

    GetProfilingResults();

    $displayManager->mapProfileResult=$mapProfileResult;

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
                        <td style="width:32%;">
                            <?php
                                $displayManager->OutputMapResourceNameWithToolTip($mapProfileResult->MapProfileData->MapResourceId);
                            ?>
                        </td>
                        <td style="width:18%;font-weight: bold;">
                            Data Extents:
                        </td>
                        <td style="width:27%;">
                            <?php
                                list($x1, $y1, $x2, $y2) = explode(",", $mapProfileResult->MapProfileData->DataExtents);
                                echo "X:" . $x1 . "&nbsp;&nbsp;&nbsp;Y:" . $y1;
                                echo "<br/>";
                                echo "X:" . $x2 . "&nbsp;&nbsp;&nbsp;Y:" . $y2;
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
                                list($x, $y) = explode("*", $_REQUEST["centerPoint"]);
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
                            <?php
                                echo $mapProfileResult->MapProfileData->CoordinateSystem;
                            ?>
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
                                echo "1:" . number_format($mapProfileResult->MapProfileData->Scale,0,"."," ");
                            ?>
                        </td>
                    </tr>
                </table>
            </div>
        </td>
    </tr>
    <tr>
        <td colspan="2" style="padding-top:20px; padding-left: 15px;">
            <?php
                echo "<span>";
                echo "Total Generation Time:&nbsp;" . $mapProfileResult->MapProfileData->TotalMapRenderTime . "&nbsp;ms";
                echo "</span>";
            ?>
        </td>
    </tr>
    <tr>
        <td colspan="2">
            <div>
                <table style="width:100%; padding: 0px; text-align: center;" cellspacing="0" cellpadding="0">
                    <?php
                        $displayManager->OutputMapRenderTimeGraph();
                    ?>
                    <tr style="height:35px;">
                        <td colspan="4" style=" text-align: center;">
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
            <td style="width:70%; padding-top: 15px;">
                <div id="layerHeader" class="layerResultsHeaderStyle" style="margin:0px; padding: 0px;">
                    <table id="layerHeaderTable">
                        <thead>
                            <!--the table head is in a different div so when the table scrolls, the head will be fixed -->
                            <!--Also, the table head columns are set with some customer attribute to help to sort the table data from the client site-->
                            <tr>
                                <th style="width:15%;border-left:1px solid #CCCCCC;" columnIndex="1" onClick="SortLayers.sortByColumn(this);">Layer</th>
                                <th style="width:20%;" columnIndex="2" onClick="SortLayers.sortByColumn(this);">Render Time</th>
                                <th style="width:25%;" columnIndex="3" onClick="SortLayers.sortByColumn(this);">Feature Class</th>
                                <th style="width:25%;" columnIndex="4" onClick="SortLayers.sortByColumn(this);">Coordinate System</th>
                                <th style="width:15%;border-right:1px solid #CCCCCC;" columnIndex="5" onClick="SortLayers.sortByColumn(this);">Type</th>
                            </tr>
                        </thead>
                    </table>
                </div>
                <div id="layerBody" class="layerResultsStyle">
                    <table style="width:100%; text-align: center;margin:0px; padding: 0px;" id="layerResultsTable">
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


