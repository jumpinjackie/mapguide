<?php

//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

        // Register this popup
        SavePopup( 'AdminAboutBox' );

        // Define Local values
        $pageTitle = "About";
        $serverVersion = "?.?";
        $appVersion = SITE_ADMINISTRATOR_VERSION;
        $errorMsg = "";

        // Get Server
        $selectedServerID = "Server";
        $site = new MgSite();
        $site->Open($userInfo);
        $siteServerAddress = $site->GetCurrentSiteAddress();
        GetServerSelection( $selectedServerID, $selectedServer );
        $serverRec = GetDataForServer( $selectedServer );
        if ( $serverRec == NULL )
            throw new Exception( sprintf( $errNotFound, $selectedServer ) );
        if ( !$serverRec->poweredUp )
            throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

        $serverVersion = $serverRec->version;
            
    }
    catch ( MgException $e )
    {
        $errorMsg = preg_replace("/%20/", " ", $errFatalError);
    }
    catch ( Exception $e )
    {
        $errorMsg = $e->getMessage();
    }
?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <head>
        <title>About MapGuide Site Administrator</title>

        <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
        <meta http-equiv="content-script-type" content="text/javascript" />
        <meta http-equiv="content-style-type" content="text/css" />

        <link href="css/globalstyles.css" rel="stylesheet" type="text/css">
        <link href="css/resizablestyles.css" rel="stylesheet" type="text/css">

        <script type="text/javascript" src="js/scripts.js"></script>
    </head>

<body class="aboutBox">
    <table cellspacing="0" cellpadding="5" border="0"  class="aboutBox" align="center">
        <tr>
            <td>
                <table class="titleBar">
                    <tr>
                        <td class="pageTitle">About MapGuide Site Administrator</td>
                        <td class="resizableButton">
                            <div align="right"><input class="resizableButton" type="button" value="Close" onClick="self.close();"></div>
                        </td>
                    </tr>
                    <tr>
                        <td class="titleBar" colspan="2"><hr noshade class="titleBar"></td>
                    </tr>
                </table>
            </td>
        </tr>
        <tr>
            <td class="aboutBoxText">
                <?php
                    DisplayErrorMsg( $errorMsg );
                    echo SERVER_TITLE.' (Version '.$serverVersion.')<br>';
                    echo APP_TITLE.' (Version '.$appVersion.')<br>';
                ?>
            </td>
        </tr>
        <tr>
            <td class="aboutBoxText">
                <?php echo ABOUT_TEXT ?>
            </td>
        </tr>
    </table>

</body>

</html>

<?php SaveSessionVars(); ?>
