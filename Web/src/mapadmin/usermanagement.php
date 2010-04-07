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

    function OkToDeleteUser( $user )
    {
        // Built-in users may not be deleted from the database
        switch( $user )
        {
            case MgUser::Administrator:
            case MgUser::Author:
            case MgUser::Anonymous:
            case MgUser::WmsUser:
            case MgUser::WfsUser:
                return 'false';
                break;
            default:
                return 'true';
                break;
        }
    }

    try
    {
        include 'resizableadmin.php';

        LoadSessionVars();

        // Did the user logout?
        CheckForLogout();
        // Define Local values
        $menuCategory = USER_MANAGEMENT_MENU_ITEM;
        $pageTitle = USER_MANAGEMENT_MENU_ITEM;
        $helpPage = 'HelpDocs/managing_users.htm';
        $pageName = 'UserManagementPage';
        $formName = 'UserManagementForm';
        $homePage = NULL;
        $selectedUserID = 'SelectedUser';
        $userTable = array();
        $pageSize = 25;
        $firstPageIndex = -1;
        $lastPageIndex = -1;
        $okToDeleteID = 'okToDelete';
        $okToDeleteVal = 'true';
        $users = array();
        $numUsers = 0;
        $currPage = 1;
        $sortDirection= DEFAULT_SORT_DIRECTION;
        $sortDirectionID = SORT_DIRECTION_ID;
        $sortColumn = NAME_SORT_COLUMN;
        $sortColumnID = SORT_COLUMN_ID;
        $userRolesTable = array();

        // Get input values
        if ( array_key_exists( $selectedUserID, $_GET ) )
            $selectedUser = $_GET[$selectedUserID];
        else
        if ( array_key_exists( $selectedUserID, $_POST ) )
            $selectedUser = $_POST[$selectedUserID];

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

        // Are we deleting the selected user?
        if ( CheckForDeleteSelection() && DeleteUser( $selectedUser ) )
            $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedUser );
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
        $users = GetUsers();
        $numUsers = sizeof( $users );
        $currPage = GetPageNumber( $users, $selectedUser, $pageSize );
        GetPageRange( $currPage, $numUsers, $pageSize, $firstPageIndex, $lastPageIndex );
        LoadUserTable( $userTable, $firstPageIndex, $lastPageIndex, true );
        if ( !array_key_exists( $selectedUser, $userTable ) && $numUsers > 0 )
            $selectedUser = $users[ $firstPageIndex ];
        LoadUserRolesTable( $userRolesTable, $firstPageIndex, $lastPageIndex );
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
                    <input type="hidden" name="<?php echo TABLE_PAGE_ID?>" value="<?php echo $currPage?>">
                    <input type="hidden" name="<?php echo $sortColumnID?>" value="<?php echo $sortColumn?>">
                    <input type="hidden" name="<?php echo $sortDirectionID?>" value="<?php echo $sortDirection?>">
                    <input type="hidden" name="<?php echo $selectedUserID?>" value="<?php echo $selectedUser?>">

                <?php
                    DisplayPaginationControl( $currPage, $numUsers, $pageSize, 'usermanagement.php', $sortColumn, $sortDirection );

                    // Toolbar
                    $buttons = array();
                    $button = new ToolbarButtonRecord();
                        $button->label = "Add User";
                        $button->icon = "images/new.gif";
                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'adduser.php');";
                    $buttons[0] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = "Edit User";
                        $button->icon = "images/edit.gif";
                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'edituser.php');";
                    $buttons[1] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = "Delete User";
                        $button->icon = "images/delete.gif";
                        $button->submitForm = false;
                        $button->action =
                            "ServiceConditionalDeleteButton( 'okToDelete', 'true', 'This user cannot be deleted.', '".DELETE_SELECTION_ID."', 'Are you sure you want to delete the selected user?', '".$formName."' );";
                    $buttons[2] = $button;
                    DisplayToolbar( $buttons, $formName );
                ?>

                <!-- User Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <?php
                            $nameSortIconStr = ( $sortColumn === NAME_SORT_COLUMN ) ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                            $idSortIconStr   = ( $sortColumn === ID_SORT_COLUMN )   ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                            $sortDirectionStr = "SetElementValue( '".$sortDirectionID."', '".GetSortDirection( $sortDirection )."' );";
                            $sortByNameStr = "SetElementValue( '".$sortColumnID."', '".NAME_SORT_COLUMN."');";
                            $sortByIDStr = "SetElementValue( '".$sortColumnID."', '".ID_SORT_COLUMN."');";
                        ?>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByNameStr?>SubmitForm('<?php echo $formName?>');">User Name<?php echo $nameSortIconStr?></a> </td>
                        <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByIDStr?>SubmitForm('<?php echo $formName?>');">User ID<?php echo $idSortIconStr?></a> </td>
                        <td class="dataHeader">Description</td>
                        <td class="dataHeader">Member of these Groups </td>
                        <td class="dataHeader">Assigned these Roles </td>
                    </tr>

                    <?php
                        $initOkToDeleteVal = OkToDeleteUser( $selectedUser );

                        foreach ( $userTable as $key => $val )
                        {
                            if ( $key == $selectedUser )
                                $checkedStr = " checked ";
                            else
                                $checkedStr = "";

                            $userSelectionOnClickStr = "SetElementValue( '".$okToDeleteID."', '".OkToDeleteUser( $key )."' );SetElementValue( '".$selectedUserID."', '".$key."');";

                            CreateListOfGroupHyperlinksForTable( $val->groups, $numGroups, $groupsStr );
                            CreateListOfRoleHyperlinksForTable( $userRolesTable[$key], $roleStr, true, $key );
                    ?>
                            <tr>
                                <td class="dataCell"><input name="userSelector" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> onClick="<?php echo $userSelectionOnClickStr ?>" ></td>
                                <td class="dataCell"><a href="edituser.php?<?php echo $selectedUserID?>=<?php echo $key?>"><?php echo $val->name; ?></a></td>
                                <td class="dataCell"><?php echo $key ?></td>
                                <td class="dataCell"><?php echo $val->description; ?></td>
                                <td class="dataCell"><a href="edituser.php?<?php echo $selectedUserID?>=<?php echo $key?>"><?php echo $numGroups; ?></a><?php echo $groupsStr ?></td>
                                <td class="dataCell"><?php echo $roleStr?></td>
                            </tr>
                    <?php
                        }
                    ?>
                </table>

                <!-- Hidden flag to say whether or not Delete is permitted -->
                <input type="hidden" name="<?php echo $okToDeleteID; ?>" value="<?php echo $initOkToDeleteVal; ?>" >

                <?php DisplayPaginationControl( $currPage, $numUsers, $pageSize, 'usermanagement.php', $sortColumn, $sortDirection ); ?>

            <?php EndContentArea( true, $formName, NULL ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
