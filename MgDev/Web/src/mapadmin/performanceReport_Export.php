<?php
   include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'performanceReport.php' );

    // Define Local values
    $mapProfileResult = new MapProfileResult();

    function ReplaceCSVSpecialChar($value)
    {
        $newValue = trim($value);
        $newValue = str_replace("\"", "\"\"", $newValue);
        return "\"".$newValue."\"";
    }

    function GetProfilingResults()
    {
        $profilingResultFile = $_SESSION["ProfilingResultFile"];

        if (isset($profilingResultFile))
        {
            global $mapProfileResult;
            $resultSource = new DOMDocument();
            $resultSource->load($profilingResultFile);
            $mapProfileResult->ReadFromXML($resultSource);
            $mapProfileResult->GetBaseLayerCount();
        }
        else
        {
            throw new Exception("The temp result file may be deleted or corrupted, Please regenerate the performance report and try again!");
        }
    }

    try
    {
        GetProfilingResults();
    }
    catch (Exception $e)
    {
        echo $e->getMessage();
        header("Content-type:text/plain");
        exit(0);
    }

    //construct a readable file name
    $tempMapName = strrchr($mapProfileResult->MapProfileData->MapResourceId, '/');
    $tempMapName = substr($tempMapName, 1, strlen($tempMapName) - 15);
    $Date = date("Y-m-d_His");
    $Filename = $Date."_".$tempMapName.".csv";

    header("Content-type:application/vnd.ms-excel");
    header("Content-Disposition:filename=".$Filename);

    //add one line gap
    $outSCVContent = "\n";

    //output the basic information of the map resource
    //get the extent display string
    list($x1, $y1, $x2, $y2) = explode(",", $mapProfileResult->MapProfileData->DataExtents);
    $extents= '"MinX:' . $x1 . '    MinY:' . $y1.'    MaxX:' . $x2 . '    MaxY:' . $y2.'"';

    //get the scale display string
    $scale= "[1: " .number_format($mapProfileResult->MapProfileData->Scale,4,"."," ")."]";

    //get the center point display string
    list($centerPointX, $centerPointY) = explode("*", $_REQUEST["centerPoint"]);
    $centerPoint= '"X:'.trim($centerPointX).'    Y:'.trim($centerPointY).'"';

    //export the map information
    $outSCVContent .=  '"Resource Name",'.$mapProfileResult->MapProfileData->MapResourceId;
    $outSCVContent .= ',"Data Extents",'.$extents."\n";
    $outSCVContent .= '"Base Layers",'.$mapProfileResult->MapProfileData->BaseLayerCount.',"Image Format",';
    $outSCVContent .= $mapProfileResult->MapProfileData->ImageFormat."\n";
    $outSCVContent .= '"Center Point",'.$centerPoint.',Layers,';
    $outSCVContent .= $mapProfileResult->MapProfileData->LayerCount."\n";
    $outSCVContent .= '"Coordinate System",'.ReplaceCSVSpecialChar($mapProfileResult->MapProfileData->CoordinateSystem);
    $outSCVContent .= ',"Render Type",'.$mapProfileResult->MapProfileData->RenderType."\n";
    $outSCVContent .= 'Scale,'.$scale."\n";

    //add two lines gap to divide the information into different sections
    $outSCVContent .= "\n";
    $outSCVContent .= "\n";

    //output the render time information
    $outSCVContent .= '"Total Generation Time:","'.$mapProfileResult->MapProfileData->TotalMapRenderTime . ' ms"'."\n";

    $outSCVContent .= '"Layers ","'.$mapProfileResult->MapProfileData->TotalLayerRenderTime . ' ms",';
    $outSCVContent .= $mapProfileResult->MapProfileData->GetLayerRenderPercent() .'%'."\n";

    $outSCVContent .= '"Labels ","'.$mapProfileResult->MapProfileData->TotalLabelRenderTime . ' ms",';
    $outSCVContent .= $mapProfileResult->MapProfileData->GetLabelRenderPercent() .'%'."\n";

    $outSCVContent .= '"Watermarks ","'.$mapProfileResult->MapProfileData->TotalWatermarkRenderTime . ' ms",';
    $outSCVContent .= $mapProfileResult->MapProfileData->GetWartermarkRenderPercent() .'%'."\n";

    $outSCVContent .= '"Images ","'.$mapProfileResult->MapProfileData->TotalImageRenderTime . ' ms",';
    $outSCVContent .= $mapProfileResult->MapProfileData->GetImageRenderPercent() .'%'."\n";

    $outSCVContent .= '"Other ","'.$mapProfileResult->MapProfileData->GetOtherRenderTime() . ' ms",';
    $outSCVContent .= $mapProfileResult->MapProfileData->GetOthersRenderPercent() .'%'."\n";

    //add two lines gap to divide the information into different sections
    $outSCVContent .= "\n";
    $outSCVContent .= "\n";

    if(isset($mapProfileResult->MapProfileData->MapErrorMessage))
    {
        $outSCVContent .= "\"Errors occurred while rendering the map:\",";
        $outSCVContent .= ReplaceCSVSpecialChar($mapProfileResult->MapProfileData->MapErrorMessage)."\n";
    }

    if(isset($mapProfileResult->MapProfileData->WatermarksErrorMessage))
    {
        $outSCVContent .= "\"Errors occurred while rendering watermarks:\",";

        $outSCVContent .= ReplaceCSVSpecialChar(trim($mapProfileResult->MapProfileData->WatermarksErrorMessage,'\n'))."\n";
    }

    if(isset($mapProfileResult->MapProfileData->LabelsErrorMessage))
    {
        $outSCVContent .= "\"Errors occurred while rendering Labels:\",";
        $outSCVContent .= ReplaceCSVSpecialChar($mapProfileResult->MapProfileData->LabelsErrorMessage)."\n";
    }

    //add two lines gap to divide the information into different sections
    $outSCVContent .= "\n";
    $outSCVContent .= "\n";

    //output the detailed information of each layer
    $outSCVContent .= 'Layer,"Render Time","Feature Class","Coordinate System",Type,Filter,"Scale Range","Inner Exception Message"'."\n";

    foreach ($mapProfileResult->LayerProfileData->LayerProfileDataCollection as $layerProfileData)
    {
        $outSCVContent .= '"'.$layerProfileData->LayerName.'",' ;
        $outSCVContent .= '"'.$layerProfileData->TotalRenderTime.' ms",' ;
        $outSCVContent .= ReplaceCSVSpecialChar($layerProfileData->FeatureClass) .',';
        $outSCVContent .= ReplaceCSVSpecialChar($layerProfileData->CoordinateSystem) .',';
        $outSCVContent .= '"'.$layerProfileData->LayerType.'",' ;
        $outSCVContent .= ReplaceCSVSpecialChar($layerProfileData->Filters).',' ;
        $outSCVContent .= '"'.$layerProfileData->ScaleRange.'",' ;
        $outSCVContent .= ReplaceCSVSpecialChar($layerProfileData->ErrorMessage) ;
        $outSCVContent .= "\n";
    }

    print $outSCVContent;
?>