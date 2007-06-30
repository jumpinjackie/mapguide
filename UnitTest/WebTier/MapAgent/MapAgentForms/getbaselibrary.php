<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<body>
<?php
    include "initwebtier.php";

    $library = "";
    $errorMsg = "";
    $status = "";

    try
    {
        echo "<b>Coordinate System API: GetBaseLibrary</b><br><br>";

        // We have to use a factory because there is no direct access to the MgCoordinateSystem constructor
        $ll84 = "GEOGCS [ \"Longitude / Latitude (WGS 84)\", DATUM [\"WGS 84\", SPHEROID [\"WGS 84\", 6378137.000000, 298.257224]], PRIMEM [ \"Greenwich\", 0.000000 ], UNIT [\"Decimal Degree\", 0.01745329251994330]]";
        $factory = new MgCoordinateSystemFactory();
        $mgcoordinatesystem = $factory->Create($ll84);
        $library = $mgcoordinatesystem->GetBaseLibrary();
        $status = "Pass";
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

    echo "<b>Library:</b><br>";
    echo "$library<br><br>";
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
