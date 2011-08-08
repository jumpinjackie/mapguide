<?php
    $Date = date("Y-m-d_His");
    $Filename = $Date.".csv";

    header("Content-type:application/vnd.ms-excel");
    header("Content-Disposition:filename=".$Filename);
    echo "Layer,Render Time,Feature Class,Coordinate System,TypeLF";
    //TODO: The real export data will be implemented in part2
?>