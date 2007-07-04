<?php

function printPropertyCollection($propertyCollection)
{
    $propCount = $propertyCollection->GetCount();
    if ($propCount == 0)
    {
        echo "No properties<br>";
        return;
    }

    for($i=0;$i<$propCount;$i++)
    {
        $property = $propertyCollection->GetItem($i);

                $propertyType = $property->GetPropertyType();
                $propertyName = $property->GetName();
                $val = $property->GetValue();

                echo "<b>$propertyName: </b>$val<br>";
    }
}

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<body>
<?php
    include "initwebtier.php";

    $category = "";
    $errorMsg = "";
    $status = "";

    try
    {
        echo "<b>Coordinate System API: EnumerateCoordinateSystems</b><br><br>";

        // We have to use a factory because there is no direct access to the MgCoordinateSystem constructor
        $ll84 = "GEOGCS [ \"Longitude / Latitude (WGS 84)\", DATUM [\"WGS 84\", SPHEROID [\"WGS 84\", 6378137.000000, 298.257224]], PRIMEM [ \"Greenwich\", 0.000000 ], UNIT [\"Decimal Degree\", 0.01745329251994330]]";
        $factory = new MgCoordinateSystemFactory();
        $mgcoordinatesystem = $factory->Create($ll84);

        // Get the list of coordinate systems for the specified category
        $category = $_GET['CATEGORY'];
        $systems = $mgcoordinatesystem->EnumerateCoordinateSystems($category);
        $count = $systems->GetCount();
        echo "<b>Category: </b>$category<br>";
        echo "<b>Coordinate Systems: </b>$count<br><br>";
        for($i=1;$i<=$count;$i++)
        {
           $system = $systems->GetItem($i-1);
           echo "<b>Coordinate System $i</b><br>";
           printPropertyCollection($system);
           echo "<br>";
        }

        echo "<br>";
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
