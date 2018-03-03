<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<body>
<?php
    include "initwebtier.php";

    $code = "";
    $wkt = "";
    $errorMsg = "";
    $status = "";

    try
    {
        echo "<b>Coordinate System API: ConvertEpsgCodeToWkt</b><br><br>";

        $factory = new MgCoordinateSystemFactory();

        $code = $_GET['CODE'];
        $epsgcode = 0 + $code; // Convert to an integer
        $wkt = $factory->ConvertEpsgCodeToWkt($epsgcode);
        $status = "Pass";
    }
    catch ( MgException $e )
    {
        $errorMsg = $e->GetExceptionMessage();
        $status = "Fail";
    }
    catch ( Exception $e )
    {
        $errorMsg = $e->getMessage();
        $status = "Fail";
    }

    echo "<b>EPSG Code:</b><br>";
    echo htmlentities(strip_tags($code), ENT_NOQUOTES);
    echo "<br><br>";
    echo "<b>OGC WKT:</b><br>";
    echo "$wkt<br><br>";
    echo "<b>Status:</b><br>";
    echo "$status<br><br>";

    if($errorMsg != "")
    {
        echo "<b>Error:</b><br>";
        echo $errorMsg;
    }

?>

</body>
</html>
