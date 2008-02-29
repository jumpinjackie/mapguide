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

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'wmsproperties.php' );

    // Define local vars
    $menuItem = CONFIGURE_WMS_MENU_ITEM;
    $pageTitle = CONFIGURE_WMS_TITLE;
    $helpPage = 'HelpDocs/configuring_web_services.htm';
    $pageName = 'WmsPropertiesPage';
    $formName = 'WmsPropertiesForm';
    $wmsProps = new WMSPropsRecord();
    $confirmationMsg = "";
    $errorMsg = "";

    if ( FirstTimeHere( $pageName ) )
        $wmsProps->GetProps();
    else
    {
        GetWmsServicePropVals( $_POST, $wmsProps );
        GetWmsServiceContactPropVals( $_POST, $wmsProps );

        // Validate inputs.
        $wmsProps->ValidateProps();

        // Set new properties
        $wmsProps->SetProps();

        $confirmationMsg = $confSuccessfulWMSUpdate;
    }
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $errorMsg = $e->GetMessage();
}
catch ( Exception $e )
{
    $errorMsg = $e->getMessage();
}
?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

    <?php OutputHeader( $pageTitle ); ?>

<body>
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, true, true, $formName, NULL ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuItem ); ?>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName );

                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, $helpPage );
                DisplaySaveCancelBar();
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );

                // Input Area
                DisplayWmsServicePropVals( $wmsProps );
                DisplayWmsServiceContactPropVals( $wmsProps );
                echo "<br>\n";

                DisplaySaveCancelBar();

                EndContentArea( true, $formName, WMS_SERVICE_TITLE_ID);
            ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
