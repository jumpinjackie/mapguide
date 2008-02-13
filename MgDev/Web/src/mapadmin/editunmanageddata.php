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
    CheckForCancel( 'unmanageddatamanagement.php' );

    // Local global vals.
    $menuCategory = UNMANAGEDDATA_MANAGEMENT_MENU_ITEM;
    $menuItem = EDIT_UNMANAGEDDATA_MENU_ITEM;
    $pageTitle = EDIT_UNMANAGEDDATA_TITLE;
    $helpPage = 'HelpDocs/adding_and_deleting_unmanageddata.htm';
    $pageName = 'EditUnmanagedDataPage';
    $formName = 'EditUnmanagedDataForm';
    $homePage = 'unmanageddatamanagement.php';

    $selectedMappingID = 'SelectedMapping';

    $mappingName = "";
    $mappingNameID = 'mappingName';
    $location = "";
    $locationID = 'location';

    $errorMsg = "";
    $confirmationMsg = "";

    $mappings = array();

    $siteServerAddress = "";
    $selectedServerID = "Server";
    $serverAdmin = 0;
    $confirmationMsg = "";
    $errorMsg = "";
    $pageEnabled = false;

    // Get Server
    $siteServerAddress = $site->GetCurrentSiteAddress();
    GetServerSelection( $selectedServerID, $selectedServer );
    $serverRec = GetDataForServer( $selectedServer );
    if ( $serverRec == NULL )
        throw new Exception( sprintf( $errNotFound, $selectedServer ) );
    if ( !$serverRec->poweredUp )
        throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

    // Get props values from server
    $serverAdmin = new MgServerAdmin();
    $serverAdmin->Open( $selectedServer, $userInfo );

    $mappings = GetUnmanagedDataMappings( $serverAdmin );

    // Is this our first time here?
    if ( FirstTimeHere( $pageName ) )
    {
        // Was the selected user passed in the URL?
        if ( array_key_exists( $selectedMappingID, $_GET ) )
        $selectedMapping = $_GET[ $selectedMappingID ];

        // Get info for selected user
        $mappingName = $selectedMapping;
        $location = $mappings[ $mappingName ];

        $serverAdmin->Close();
    }
    // The save button was hit.
    else
    {
        // Get submitted data
        if ( array_key_exists( $mappingNameID, $_POST ) )
            $mappingName = $_POST[ $mappingNameID ];
        if ( array_key_exists( $locationID, $_POST ) )
            $location = $_POST[ $locationID ];

        // Validate inputs.
        if ( empty( $mappingName ) )
            throw new Exception( $errMappingNameMissing );
        if ( empty( $location ) )
            throw new Exception( $errLocationMissing );

        // Update parameters of user.
        SetUnmanagedDataMapping( $serverAdmin, $mappingName, $location );
        if ($mappingName != $selectedMapping)
        {
            DeleteUnmanagedDataMapping( $serverAdmin, $selectedMapping );
        }
        $serverAdmin->Close();

        // Everything is OK.
        $confirmationMsg = sprintf( $confSuccessfulAddition, $mappingName );
        $selectedMapping = $mappingName;
        SaveSessionVars();
        header( 'Path to External File:  unmanageddatamanagement.php?'.strip_tags(SID) );
        exit();
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
                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="editUserInputLabel">Alias Name:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $mappingNameID?>" type="text" value="<?php echo $mappingName?>" size="60"></td>
                    </tr>
                    <tr>
                        <td class="editUserInputLabel">Path to External Folder:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $locationID?>" type="text" value="<?php echo $location?>" size="60"></td>
                    </tr>
                </table>
            <?php
            DisplaySaveCancelBar();
            EndContentArea( true, $formName, $mappingNameID );
            ?>
            <!-- End of Contents Area -->

        </tr>

    </table>
</body>

</html>
