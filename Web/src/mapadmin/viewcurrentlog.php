<?php

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

    try
    {
        include 'resizableadmin.php';

        LoadSessionVars();

        // Define Local values
        $pageTitle = 'View Current Log';
        $logToViewID = 'logToView';
        $logToView = "";
        $confirmationMsg = "";
        $errorMsg = "";

        if ( array_key_exists( $logToViewID, $_GET ) )
            $logToView = $_GET[ $logToViewID ];
        if ( empty( $logToView ) )
            $logToView = ACCESS_LOG;

        $pageTitle = 'View Current '.$logLabels[ $logToView ].' Log ';
    }
    catch ( MgException $e )
    {
        CheckForFatalMgException( $e );
        if ( empty( $errorMsg ) )
            $errorMsg = $e->GetMessage();
    }
    catch ( Exception $e )
    {
        if ( empty( $errorMsg ) )
            $errorMsg = $e->getMessage();
    }

    SaveSessionVars();
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <?php
        OutputHeader( $pageTitle );
    ?>

<frameset rows="215,*" cols="*" frameborder="0" framespacing="0">

    <frame name="ViewCurrentLogTop"    scrolling="no"   frameborder="0" src="viewcurrentlogtop.php?<?php echo $logToViewID?>=<?php echo $logToView?>" >
    <frame name="ViewCurrentLogBottom" scrolling="auto" frameborder="0">


</frameset><noframes></noframes>

</html>
