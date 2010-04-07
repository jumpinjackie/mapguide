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

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'wfsproperties.php' );

    // Define local vars
    $menuItem = CONFIGURE_WFS_MENU_ITEM;
    $pageTitle = CONFIGURE_WFS_TITLE;
    $helpPage = 'HelpDocs/configuring_web_services.htm';
    $pageName = 'WFSPropertiesPage';
    $formName = 'WFSPropertiesForm';
    $WFSProps = new WFSPropsRecord();
    $confirmationMsg = "";
    $errorMsg = "";

    if ( FirstTimeHere( $pageName ) )
        $WFSProps->GetProps();
    else
    {
        GetWFSServicePropVals( $_POST, $WFSProps );
        GetWFSServiceContactPropVals( $_POST, $WFSProps );

        // Validate inputs.
        $WFSProps->ValidateProps();

        // Set new properties
        $WFSProps->SetProps();

        $confirmationMsg = $confSuccessfulWFSUpdate;
    }
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $errorMsg = $e->GetExceptionMessage();
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
                DisplayWFSServicePropVals( $WFSProps );
                DisplayWFSServiceContactPropVals( $WFSProps );
                echo "<br>\n";

                DisplaySaveCancelBar();

                EndContentArea( true, $formName, WFS_SERVICE_TITLE_ID);
            ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
