<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<body>
<?php
    include "initwebtier.php";

    $wkt = "";
    $errorMsg = "";
    $status = "";

    try
    {
        echo "<b>Coordinate System API: IsValid</b><br><br>";

        $factory = new MgCoordinateSystemFactory();
        $wkt = $_GET['WKT'];
        $result = $factory->IsValid($wkt);
        if($result == true)
        {
            $status = "Pass";
        }
        else
        {
            $status = "Fail";
        }
    }
    catch ( MgException $e )
    {
        $errorMsg = $e->GetMessage();
        $status = "Fail";
    }
    catch ( Exception $e )
    {
        $errorMsg = $e->getMessage();
        $status = "Fail";
    }

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
