<?php

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

        $serverToMonitorID = 'serverToMonitor';

        // Define Local values
        if ( array_key_exists( $serverToMonitorID, $_GET ) )
            $selectedServer = $_GET[ $serverToMonitorID ];
        $selectedServerName = GetServerName( $selectedServer );
        $pageTitle = $selectedServerName.' - Status';
        $confirmationMsg = "";
        $errorMsg = "";
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

<frameset rows="180,*" cols="*" frameborder="0" framespacing="0">

    <frame name="ViewServerStatusTop"    scrolling="auto"   frameborder="0" src="viewserverstatustop.php" >
    <frame name="ViewServerStatusBottom" scrolling="auto" frameborder="0">


</frameset><noframes></noframes>

</html>
