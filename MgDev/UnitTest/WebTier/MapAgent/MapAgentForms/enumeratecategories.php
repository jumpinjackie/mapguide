<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<body>
<?php
    include "initwebtier.php";

    $errorMsg = "";
    $status = "";

    try
    {
        echo "<b>Coordinate System API: EnumerateCategories</b><br><br>";

        $factory = new MgCoordinateSystemFactory();

        // Get the list of categories
        $categories = $factory->EnumerateCategories();
        $count = $categories->GetCount();
        echo "<b>Coordinate System Categories: </b>$count<br>";
        for($i=1;$i<=$count;$i++)
        {
           $category = $categories->GetItem($i-1);
           echo "<b>$i: </b>$category<br>";
        }

        echo "<br>";
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
