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

        $factory = new MgCoordinateSystemFactory();
        $library = $factory->GetBaseLibrary();
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
