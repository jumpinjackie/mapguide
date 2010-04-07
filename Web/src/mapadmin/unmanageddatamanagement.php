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

    // Define Local values
    $menuCategory = UNMANAGEDDATA_MANAGEMENT_MENU_ITEM;
    $pageTitle = UNMANAGEDDATA_MANAGEMENT_MENU_ITEM;

    $helpPage = 'HelpDocs/about_external_files.htm';
    $pageName = 'UnmanagedDataManagementPage';
    $formName = 'UnmanagedDataManagementForm';
    $homePage = NULL;

    $selectedMappingID = 'SelectedMapping';
    $mappingTable = array();

    $pageSize = 25;
    $firstPageIndex = -1;
    $lastPageIndex = -1;

    $mappings = array();

    $numMappings = 0;
    $currPage = 1;
    $sortDirection= DEFAULT_SORT_DIRECTION;
    $sortDirectionID = SORT_DIRECTION_ID;
    $sortColumn = NAME_SORT_COLUMN;
    $sortColumnID = SORT_COLUMN_ID;


    $siteServerAddress = "";
    $selectedServerID = "Server";
    $serverAdmin = 0;
    $refreshRequested = false;
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

    // Get input values
    if ( array_key_exists( $selectedMappingID, $_GET ) )
        $selectedMapping = $_GET[$selectedMappingID];
    else
    if ( array_key_exists( $selectedMappingID, $_POST ) )
        $selectedMapping = $_POST[$selectedMappingID];

    if ( array_key_exists( $sortDirectionID, $_GET ) )
        $sortDirection = $_GET[$sortDirectionID];
    else
    if ( array_key_exists( $sortDirectionID, $_POST ) )
        $sortDirection = $_POST[$sortDirectionID];
    SetUserSortDirection( $sortDirection );

    if ( array_key_exists( $sortColumnID, $_GET ) )
        $sortColumn = $_GET[$sortColumnID];
    else
    if ( array_key_exists( $sortColumnID, $_POST ) )
        $sortColumn = $_POST[$sortColumnID];
    SetUserSortColumn( $sortColumn );

    // Are we switching to another page?
    CheckForPageSwitch();

    // Are we deleting the selected mapping?
    //if ( CheckForDeleteSelection() && DeleteUnmanagedDataMapping( $serverAdmin, $mappings, $selectedMapping ) )
    if ( CheckForDeleteSelection() && DeleteUnmanagedDataMapping( $serverAdmin, $selectedMapping ) )
    {
        $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedMapping );
        $mappings = GetUnmanagedDataMappings( $serverAdmin );
    }
    $serverAdmin->Close();

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

// Load display data
try
{
    // sort mappings
    if ($sortColumn == NAME_SORT_COLUMN)
        if ($sortDirection == ASCENDING_SORT_DIRECTION)
            ksort($mappings);
        else
            krsort($mappings);
    else
        if ($sortDirection == ASCENDING_SORT_DIRECTION)
            asort($mappings);
        else
            arsort($mappings);

    $numMappings = sizeof( $mappings );
    $mappingsKeys = array_keys( $mappings );

    $currPage = GetPageNumber( $mappingsKeys, $selectedMapping, $pageSize );
    GetPageRange( $currPage, $numMappings, $pageSize, $firstPageIndex, $lastPageIndex );

    LoadMappingTable( $mappingTable, $mappings, $firstPageIndex, $lastPageIndex );

    if ( !array_key_exists( $selectedMapping, $mappingTable ) && $numMappings > 0 )
    {
        $selectedMapping = $mappingsKeys[ $firstPageIndex ];
    }
    else if ( $numMappings == 0 )
    {
        $selectedMapping = '';
    }
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    if ( empty( $errorMsg ) )
        $errorMsg = $e->GetExceptionMessage();
}
catch ( Exception $e )
{
    if ( empty( $errorMsg ) )
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
            <?php DisplayLefthandSash( $menuCategory ); ?>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName ); ?>

                <?php
                    DeclareHiddenVars( $pageName );
                    DisplayTitleBar( $pageTitle, $helpPage );
                    DisplayConfirmationMsg( $confirmationMsg );
                    DisplayErrorMsg( $errorMsg );
                ?>

                <div class="textMsg">
                    <br>
                    Configure aliases to external folders containing spatial files such as Raster, SDF, and SHP.
                    You can then connect directly to these external files, instead of loading them into the MapGuide Library.
                    Restart the MapGuide Server process to apply the changes.
                    <br><br>
                </div>


                    <input type="hidden" name="<?php echo TABLE_PAGE_ID?>" value="<?php echo $currPage?>">
                    <input type="hidden" name="<?php echo $sortColumnID?>" value="<?php echo $sortColumn?>">
                    <input type="hidden" name="<?php echo $sortDirectionID?>" value="<?php echo $sortDirection?>">
                    <input type="hidden" name="<?php echo $selectedMappingID?>" value="<?php echo $selectedMapping?>">

                <?php
                    DisplayPaginationControl( $currPage, $numMappings, $pageSize, 'unmanageddatamanagement.php', $sortColumn, $sortDirection );

                    // Toolbar
                    $buttons = array();
                    $button = new ToolbarButtonRecord();
                        $button->label = "Add Alias";
                        $button->icon = "images/new.gif";
                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'addunmanageddata.php');";
                    $buttons[0] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = "Edit Alias";
                        $button->icon = "images/edit.gif";
                        $button->disabled = !empty( $selectedMapping ) ? false : true;
                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'editunmanageddata.php');";
                    $buttons[1] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = "Delete Alias";
                        $button->icon = "images/delete.gif";
                        $button->submitForm = false;
                        $button->disabled = !empty( $selectedMapping ) ? false : true;
                        $button->action = "ConditionalSubmitForm( '".$formName."', 'Are you sure you want to delete the selected alias?', '".DELETE_SELECTION_ID."', true );";
                    $buttons[2] = $button;
                    DisplayToolbar( $buttons, $formName );
                ?>

                <!-- Unmanaged Data Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <?php
                            $mappingNameSortIconStr    = ( $sortColumn === NAME_SORT_COLUMN ) ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                            $locationSortIconStr       = ( $sortColumn === ID_SORT_COLUMN )   ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                            $sortDirectionStr          = "SetElementValue( '".$sortDirectionID."', '".GetSortDirection( $sortDirection )."' );";
                            $sortByMappingNameStr      = "SetElementValue( '".$sortColumnID."', '".NAME_SORT_COLUMN."');";
                            $sortByLocationStr         = "SetElementValue( '".$sortColumnID."', '".ID_SORT_COLUMN."');";
                        ?>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByMappingNameStr?>SubmitForm('<?php echo $formName?>');">Alias Name<?php echo $mappingNameSortIconStr ?></a> </td>
                        <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByLocationStr?>SubmitForm('<?php echo $formName?>');">Path to External Folder<?php echo $locationSortIconStr ?></a> </td>
                    </tr>

                    <?php
                        $initOkToDeleteVal = 'true';

                        foreach ( $mappingTable as $key => $val )
                        {
                            if ( $key == $selectedMapping )
                                $checkedStr = " checked ";
                            else
                                $checkedStr = "";

                            $mappingSelectionOnClickStr = "SetElementValue( '".$selectedMappingID."', '".$key."');";

                    ?>
                            <tr>
                                <td class="dataCell"><input name="mappingSelector" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> onClick="<?php echo $mappingSelectionOnClickStr ?>" ></td>
                                <td class="dataCell"><a href="editunmanageddata.php?<?php echo $selectedMappingID?>=<?php echo $key?>"><?php echo $key ?></a></td>
                                <td class="dataCell"><?php echo $val ?></td>
                            </tr>
                    <?php
                        }
                    ?>
                </table>
                <?php DisplayPaginationControl( $currPage, $numMappings, $pageSize, 'unmanageddatamanagement.php', $sortColumn, $sortDirection ); ?>

            <?php EndContentArea( true, $formName, NULL ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
