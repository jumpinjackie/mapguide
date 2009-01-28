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

    function LoadResourcePackage( $packageName )
    {
        global $site;
        global $userInfo;

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );

        set_time_limit(0);
        $serverAdmin->LoadPackage( $packageName );

        $serverAdmin->Close();
    }

    // Define Local values
    $pageName = "LoadPackage";
    $formName = "LoadPackage";
    $pageTitle = "Load Package";
    $packageLoadErrorMsg = "";
    $packageLoadConfirmationMsg = "";

    if ( !FirstTimeHere( $pageName ) )
    {
        LoadResourcePackage( $selectedPackage );
        $packageLoadConfirmationMsg = sprintf( $confPackageLoadCompleted, $selectedPackage );
        $packageLoadErrorMsg = "";
        SaveSessionVars();
        header( 'Location:  packagemanagement.php?'.strip_tags(SID) );
        exit();
    }
    else
    {
        if ( empty( $selectedPackage ) )
            throw new Exception( $errNoPackageSelected );
    }
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $packageLoadErrorMsg = $e->GetMessage();
}
catch ( Exception $e )
{
    $packageLoadErrorMsg = $e->getMessage();
}

// If an error was encountered, return to the packages table and report the error.
if ( !empty( $packageLoadErrorMsg ) )
{
    SaveSessionVars();
    header( 'Location:  packagemanagement.php?'.strip_tags(SID) );
    exit();
}

?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <?php
    OutputHeader( $pageTitle );
    ?>

<body onLoad="document.body.style.cursor='wait'; window.setTimeout('document.<?php echo $formName ?>.submit();', 1);" >
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, false, false, $formName, NULL ); ?>
        </tr>
        <tr>
            <!-- Contents Area -->
                <?php
                BeginContentArea( $formName );
                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, NULL );
                ?>

                The package is currently loading which may take some time.  If you wish to continue with Site Administration work, you may open another browser and login to another instance of the Site Administrator.

                <?php
                EndContentArea( true, $formName, "" );
                ?>
            <!-- End of Contents Area -->

        </tr>
    </table>

</body>
</html>
