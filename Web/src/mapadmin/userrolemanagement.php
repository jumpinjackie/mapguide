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

    // Are we cancelling?
    CheckForCancel( 'userrolemanagement.php' );

    // Are we switching to another page?
    CheckForPageSwitch();

    // Define Local Vars
    $menuCategory = ASSIGN_ROLES_MENU_ITEM;
    $menuItem = ASSIGN_ROLES_MENU_ITEM;
    $pageTitle = ASSIGN_ROLES_TITLE;
    $helpPage = 'HelpDocs/managing_roles.htm';
    $pageName = 'UserRoleManagementPage';
    $formName = 'UserRoleManagementForm';
    $homePage = NULL;
    $selectedUserID = 'SelectedUser';
    $highlightedUser = NULL;
    $adminOnID = 'AdminOn';
    $adminOnList = array();
    $authorOnID = 'AuthorOn';
    $authorOnList = array();
    $userUpdateID = 'UsersToUpdate';
    $userUpdateList = array();
    $userRolesTable = array();
    $confirmationMsg = "";
    $errorMsg = "";
    $pageSize = 25;
    $firstPageIndex = -1;
    $lastPageIndex = -1;
    $currPage = 1;
    $users = array();
    $userCount = 0;
    $sortDirection= DEFAULT_SORT_DIRECTION;
    $sortDirectionID = SORT_DIRECTION_ID;
    $sortColumn = NAME_SORT_COLUMN;
    $sortColumnID = SORT_COLUMN_ID;

    // Get submitted data
    if ( !FirstTimeHere( $pageName ) )
    {
        $userUpdateList = array_key_exists( $userUpdateID, $_POST ) ? $_POST[ $userUpdateID ] : array();
        $adminOnList = array_key_exists( $adminOnID, $_POST ) ? $_POST[ $adminOnID ] : array();
        $authorOnList = array_key_exists( $authorOnID, $_POST ) ? $_POST[ $authorOnID ] : array();
    }

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

    if ( array_key_exists( $selectedUserID, $_GET ) )
        $highlightedUser = $_GET[$selectedUserID];

    // Save data, if requested.
    if ( CheckForSaveData() )
    {
        // Update admin role.
        $usersToGrant = new MgStringCollection();
        $usersToRevoke = new MgStringCollection();
        $roleToUpdate = new MgStringCollection();
        $roleToUpdate->Add( ADMIN_ROLE );
        foreach( $userUpdateList as $userID )
        {
            if ( in_array( $userID, $adminOnList ) )
            $usersToGrant->Add( $userID ) ;
            else
            $usersToRevoke->Add( $userID );
        }
        $site->GrantRoleMembershipsToUsers( $roleToUpdate, $usersToGrant );
        $site->RevokeRoleMembershipsFromUsers( $roleToUpdate, $usersToRevoke );
        $confirmationMsg = $confSuccessfulRoleUpdate;

        // Update author role.
        $usersToGrant->Clear();
        $usersToRevoke->Clear();
        $roleToUpdate->SetItem( 0, AUTHOR_ROLE );
        foreach( $userUpdateList as $userID )
        {
            if ( in_array( $userID, $authorOnList ) )
            $usersToGrant->Add( $userID ) ;
            else
            $usersToRevoke->Add( $userID );
        }
        $site->GrantRoleMembershipsToUsers( $roleToUpdate, $usersToGrant );
        $site->RevokeRoleMembershipsFromUsers( $roleToUpdate, $usersToRevoke );
        $confirmationMsg = $confSuccessfulRoleUpdate;
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

// Load the display table.
try
{
    $users = GetUsers();
    $userCount = sizeof( $users );
    $currPage = GetPageNumber( $users, $selectedUser, $pageSize );
    GetPageRange( $currPage, $userCount, $pageSize, $firstPageIndex, $lastPageIndex );
    LoadUserRolesTable( $userRolesTable, $firstPageIndex, $lastPageIndex );
    if ( !array_key_exists( $selectedUser, $userRolesTable ) && $userCount > 0 )
        $selectedUser = $users[ $firstPageIndex ];

    // If not our first time here, use the submitted data
    if ( !FirstTimeHere( $pageName ) )
    {
        foreach ( $userRolesTable as $userID => $val )
        {
            if ( in_array( $userID, $userUpdateList ) )
            {
                $val->adminOn = in_array( $userID, $adminOnList ) ? true : false;
                $val->authorOn = in_array( $userID, $authorOnList ) ? true : false;
            }
        }
    }
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

?>

<!-- PAGE DEFINITION --><!-- PAGE DEFINITION -->
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
            <?php BeginContentArea( $formName );
                  DisplayTitleBar( $pageTitle, $helpPage );
            ?>

                <input type="hidden" name="<?php echo TABLE_PAGE_ID?>" value="<?php echo $currPage?>">
                <input type="hidden" name="<?php echo $sortColumnID?>" value="<?php echo $sortColumn?>">
                <input type="hidden" name="<?php echo $sortDirectionID?>" value="<?php echo $sortDirection?>">

                <!-- Tabs -->
                <table cellspacing="0" cellpadding="0" class="TabTable">

                    <tr>
                        <td class="SelectedTab">Users</td>
                        <td class="DeselectedTab"><a href="grouprolemanagement.php">Groups</a></td>
                        <td class="NoTabs">&nbsp;</td>
                    </tr>

                    <tr>
                        <td class="TabTableBody" colspan="3">

                            <!-- Spacing -->
                            <br>

                            <?php
                            DeclareHiddenVars( $pageName );

                            DisplaySaveCancelBar();
                            DisplayConfirmationMsg( $confirmationMsg );
                            DisplayErrorMsg( $errorMsg );

                            DisplayPaginationControl( $currPage, $userCount, $pageSize, 'userrolemanagement.php', $sortColumn, $sortDirection );

                            // Toolbar
                            $buttons = array();
                            $button = new ToolbarButtonRecord();
                            $button->label = "Edit Roles";
                            $button->icon = "images/edit.gif";
                            $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'assignroles.php' );";
                            $buttons[0] = $button;
                            DisplayToolbar( $buttons, $formName );
                            ?>

                            <!-- Role Table-->
                            <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                                <?php
                                    $nameSortIconStr = ( $sortColumn === NAME_SORT_COLUMN ) ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                                    $idSortIconStr   = ( $sortColumn === ID_SORT_COLUMN )   ? '   <img src="'.GetSortIcon( $sortDirection ).'">' : '';
                                    $sortDirectionStr = "SetElementValue( '".$sortDirectionID."', '".GetSortDirection( $sortDirection )."' );";
                                    $sortByNameStr = "SetElementValue( '".$sortColumnID."', '".NAME_SORT_COLUMN."');";
                                    $sortByIDStr = "SetElementValue( '".$sortColumnID."', '".ID_SORT_COLUMN."');";
                                ?>
                                <tr>
                                    <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByNameStr?>SubmitForm('<?php echo $formName?>');">User Name<?php echo $nameSortIconStr?></a></td>
                                    <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr.$sortByIDStr?>SubmitForm('<?php echo $formName?>');">User ID<?php echo $idSortIconStr?></a></td>
                                    <td class="dataHeader" align="center" colspan="2">Administrator Role</td>
                                    <td class="dataHeader" align="center" colspan="2">Map Author Role</td>
                                </tr>
                                <tr>
                                    <td class="TableSubHeading">&nbsp;</td>
                                    <td class="TableSubHeading">&nbsp;</td>
                                    <td class="TableSubHeading">User Level</td>
                                    <td class="TableSubHeading">Group Level</td>
                                    <td class="TableSubHeading">User Level</td>
                                    <td class="TableSubHeading">Group Level</td>
                                </tr>

                                <?php
                                foreach ( $userRolesTable as $key => $val )
                                {
                                    CreateListOfGroupHyperlinksForTable( $val->adminFromGroups, $numAdminGroups, $adminGroupList );
                                    CreateListOfGroupHyperlinksForTable( $val->authorFromGroups, $numAuthorGroups, $authorGroupList );
                                    $tdClass = ($key === $highlightedUser) ? 'highlightedDataCell' : 'dataCell';
                                ?>
                                        <tr>
                                            <td class="<?php echo $tdClass ?>"><a href="edituser.php?<?php echo $selectedUserID?>=<?php echo $key?>"><?php echo $val->name; ?></a></td>
                                            <td class="<?php echo $tdClass ?>"><input type="hidden" name="<?php echo $userUpdateID.'[]'?>" value="<?php echo $key?>" ><?php echo $key ?></td>
                                            <td class="<?php echo $tdClass ?>" align="center"><input name="<?php echo $adminOnID.'[]'; ?>" type="checkbox" value="<?php echo $key ?>" <?php if ($val->adminOn) { echo 'checked="checked"'; }?>></td>
                                            <td class="<?php echo $tdClass ?>"><input type="checkbox" disabled <?php if ($numAdminGroups>0) { echo 'checked="checked"'; }?>><?php echo $adminGroupList; ?></td>
                                            <td class="<?php echo $tdClass ?>" align="center"><input name="<?php echo $authorOnID.'[]'; ?>" type="checkbox" value="<?php echo $key ?>" <?php if ($val->authorOn) { echo 'checked="checked"'; }?>></td>
                                            <td class="<?php echo $tdClass ?>"><input type="checkbox" disabled <?php if ($numAuthorGroups>0) { echo 'checked="checked"'; }?>><?php echo $authorGroupList; ?></td>
                                        </tr>
                                <?php
                                }
                                ?>
                            </table>
                            <?php
                            DisplayPaginationControl( $currPage, $userCount, $pageSize, 'userrolemanagement.php', $sortColumn, $sortDirection );
                            DisplaySaveCancelBar();
                            ?>
                       </td>
                    </tr>
                </table>

            <?php EndContentArea( true, $formName, "" );?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
