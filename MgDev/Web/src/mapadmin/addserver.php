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
    CheckForCancel( 'servermanagement.php' );

    // Define the property sections
    define( 'GENERAL_PROPS', "GENERAL" );
    define( 'ADMIN_CONN_PROPS', "ADMIN_CONN" );
    define( 'CLIENT_CONN_PROPS', "CLIENT_CONN" );
    define( 'SITE_CONN_PROPS', "SITE_CONN" );

    // Define local vars
    $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
    $menuItem = ADD_SERVER_MENU_ITEM;
    $pageTitle = ADD_SERVER_TITLE;
    $helpPage = 'HelpDocs/adding_and_removing_servers.htm';
    $pageName = 'AddServerPage';
    $formName = 'AddServerForm';
    $homePage = 'servermanagement.php';
    $serverName = "";
    $serverNameID = 'serverName';
    $serverAddress = "";
    $serverAddressID = 'serverAddress';
    $serverDescription = "";
    $serverDescriptionID = 'serverDescription';
    $confirmationMsg = "";
    $errorMsg = "";

    // Save button must have been hit.
    if ( !FirstTimeHere( $pageName ) )
    {
        // Get submitted data
        if ( array_key_exists( $serverNameID, $_POST ) )
        $serverName = $_POST[ $serverNameID ];
        if ( array_key_exists( $serverAddressID, $_POST ) )
        $serverAddress = $_POST[ $serverAddressID ];
        if ( array_key_exists( $serverDescriptionID, $_POST ) )
        $serverDescription = $_POST[ $serverDescriptionID ];

        // Validate inputs.
        if ( empty( $serverName ) )
        throw new Exception( $errServerNameMissing );
        if ( empty( $serverAddress ) )
        throw new Exception( $errServerAddressMissing );

        // Create new server
        $site->AddServer( $serverName, $serverDescription, $serverAddress );
        $selectedServer = $serverAddress;

        $confirmationMsg = sprintf( $confSuccessfulAddition, $serverName );
        SaveSessionVars();
        header( 'Location:  servermanagement.php?'.strip_tags(SID) );
        exit();
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
            <?php DisplaySiteHeader( false, true, true, true, $formName, $homePage ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuItem ); ?>

            <!-- Contents Area -->
             <?php BeginContentArea( $formName ); ?>

                <?php
                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, $helpPage );
                DisplaySaveCancelBar();
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );
                ?>

                <!-- Input Area -->
                <br>
                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="inputLabel" valign="top">Name:</td>
                        <td class="inputFormValue">
                            <input class="inputFormValue" name="<?php echo $serverNameID?>" type="text" size="60" value="<?php echo $serverName?>"></td>
                        </td>
                    </tr>
                    <tr>
                        <td class="inputLabel" valign="top">IP address:</td>
                        <td class="inputFormValue">
                            <input class="inputFormValue" name="<?php echo $serverAddressID?>" type="text" size="60" value="<?php echo $serverAddress?>"></td>
                        </td>
                    </tr>
                    <tr>
                        <td class="inputLabel" valign="top">Description:</td>
                        <td class="inputFormValue">
                            <textarea  class="inputFormValue" name="<?php echo $serverDescriptionID?>" cols="60"><?php echo $serverDescription; ?></textarea>
                        </td>
                    </tr>
                </table>
                <br>

                <?php
                DisplaySaveCancelBar();
                ?>
                <?php EndContentArea( true, $formName, $serverNameID ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>

</html>
