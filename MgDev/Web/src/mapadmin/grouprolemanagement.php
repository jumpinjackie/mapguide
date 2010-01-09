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
    CheckForCancel( 'grouprolemanagement.php' );

    // Are we switching to another page?
    CheckForPageSwitch();

    // Define Local Vars
    $menuCategory = ASSIGN_ROLES_MENU_ITEM;
    $menuItem = ASSIGN_ROLES_MENU_ITEM;
    $pageTitle = ASSIGN_ROLES_TITLE;
    $helpPage = 'HelpDocs/managing_roles.htm';
    $pageName = 'GroupRoleManagementPage';
    $formName = 'GroupRoleManagementForm';
    $homePage = NULL;
    $selectedGroupID = 'SelectedGroup';
    $highlightedGroup = NULL;
    $adminOnID = 'AdminOn';
    $adminOnList = array();
    $authorOnID = 'AuthorOn';
    $authorOnList = array();
    $groupUpdateID = 'GroupsToUpdate';
    $groupUpdateList = array();
    $groupRolesTable = array();
    $confirmationMsg = "";
    $errorMsg = "";
    $pageSize = 25;
    $firstPageIndex = -1;
    $lastPageIndex = -1;
    $currPage = 1;
    $groups = array();
    $groupCount = 0;
    $sortDirection= DEFAULT_SORT_DIRECTION;
    $sortDirectionID = SORT_DIRECTION_ID;

    // Get submitted data
    if ( !FirstTimeHere( $pageName ) )
    {
        $groupUpdateList = array_key_exists( $groupUpdateID, $_POST ) ? $_POST[ $groupUpdateID ] : array();
        $adminOnList = array_key_exists( $adminOnID, $_POST ) ? $_POST[ $adminOnID ] : array();
        $authorOnList = array_key_exists( $authorOnID, $_POST ) ? $_POST[ $authorOnID ] : array();
    }
    if ( array_key_exists( $sortDirectionID, $_GET ) )
        $sortDirection = $_GET[$sortDirectionID];
    else
    if ( array_key_exists( $sortDirectionID, $_POST ) )
        $sortDirection = $_POST[$sortDirectionID];
    SetGroupSortDirection( $sortDirection );

    if ( array_key_exists( $selectedGroupID, $_GET ) )
        $highlightedGroup = $_GET[$selectedGroupID];

    // Save data, if requested.
    if ( CheckForSaveData() )
    {
        // Update admin role.
        $groupsToGrant = new MgStringCollection();
        $groupsToRevoke = new MgStringCollection();
        $roleToUpdate = new MgStringCollection();
        $roleToUpdate->Add( ADMIN_ROLE );
        foreach( $groupUpdateList as $groupName )
        {
            if ( in_array( $groupName, $adminOnList ) )
            $groupsToGrant->Add( $groupName ) ;
            else
            $groupsToRevoke->Add( $groupName );
        }
        $site->GrantRoleMembershipsToGroups( $roleToUpdate, $groupsToGrant );
        $site->RevokeRoleMembershipsFromGroups( $roleToUpdate, $groupsToRevoke );
        $confirmationMsg = $confSuccessfulRoleUpdate;

        // Update author role.
        $groupsToGrant->Clear();
        $groupsToRevoke->Clear();
        $roleToUpdate->SetItem( 0, AUTHOR_ROLE );
        foreach( $groupUpdateList as $groupName )
        {
            if ( in_array( $groupName, $authorOnList ) )
            $groupsToGrant->Add( $groupName ) ;
            else
            $groupsToRevoke->Add( $groupName );
        }
        $site->GrantRoleMembershipsToGroups( $roleToUpdate, $groupsToGrant );
        $site->RevokeRoleMembershipsFromGroups( $roleToUpdate, $groupsToRevoke );
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
    $groups = GetGroups();
    $groupCount = sizeof( $groups );
    $currPage = GetPageNumber( $groups, $selectedGroup, $pageSize );
    GetPageRange( $currPage, $groupCount, $pageSize, $firstPageIndex, $lastPageIndex );
    LoadGroupRolesTable( $groupRolesTable, $firstPageIndex, $lastPageIndex );
    if ( !array_key_exists( $selectedGroup, $groupRolesTable ) && $groupCount > 0 )
        $selectedGroup = $groups[ $firstPageIndex ];

    // If not our first time here, use the submitted data
    if ( !FirstTimeHere( $pageName ) )
    {
        foreach ( $groupRolesTable as $groupName => $val )
        {
            if ( in_array( $groupName, $groupUpdateList ) )
            {
                $val->adminOn = in_array( $groupName, $adminOnList ) ? true : false;
                $val->authorOn = in_array( $groupName, $authorOnList ) ? true : false;
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
            <?php BeginContentArea( $formName );
                  DisplayTitleBar( $pageTitle, $helpPage );
            ?>

                <input type="hidden" name="<?php echo TABLE_PAGE_ID?>" value="<?php echo $currPage?>">
                <input type="hidden" name="<?php echo $sortDirectionID?>" value="<?php echo $sortDirection?>">

                <!-- Tabs -->
                <table cellspacing="0" cellpadding="0" class="TabTable">

                    <tr>
                        <td class="DeselectedTab"><a href="userrolemanagement.php">Users</a></td>
                        <td class="SelectedTab">Groups</td>
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

                            DisplayPaginationControl( $currPage, $groupCount, $pageSize, 'grouprolemanagement.php', NULL, $sortDirection );

                            // Toolbar
                            $buttons = array();
                            $button = new ToolbarButtonRecord();
                            $button->label = "Edit Roles";
                            $button->icon = "images/edit.gif";
                            $button->action = "SetElementValue( '".NEXT_PAGE_ID."', 'assignroles.php' );";
                            $buttons[0] = $button;
                            DisplayToolbar( $buttons, $formName );
                            ?>

                            <!-- Role Table-->
                            <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                                <tr>
                                    <?php
                                        $sortIcon = GetSortIcon( $sortDirection );
                                        $sortDirectionStr = "SetElementValue( '".$sortDirectionID."', '".GetSortDirection( $sortDirection )."' );";
                                    ?>
                                    <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr?>SubmitForm('<?php echo $formName?>');">Group Name   <img src="<?php echo $sortIcon?>"></a> </td>
                                    <td class="dataHeader">Administrator Role</td>
                                    <td class="dataHeader">Map Author Role</td>
                                </tr>

                                <?php
                                foreach ( $groupRolesTable as $key => $val )
                                {
                                ?>

                                        <tr>
                                            <?php $tdClass = ($key === $highlightedGroup) ? 'highlightedDataCell' : 'dataCell'; ?>
                                            <td class="<?php echo $tdClass ?>">
                                                <input type="hidden" name="<?php echo $groupUpdateID.'[]'?>" value="<?php echo $key?>" >
                                                <?php
                                                if ( $key == MgGroup::Everyone )
                                                    echo $key;
                                                else
                                                    echo '<a href="editgroup.php?'.$selectedGroupID.'='.$key.'">'.$key.'</a>';
                                                ?>
                                            </td>
                                            <td class="<?php echo $tdClass ?>" align="center"><input name="<?php echo $adminOnID.'[]'?>" type="checkbox" value="<?php echo $key ?>" <?php if ($val->adminOn) { echo 'checked="checked"'; }?>></td>
                                            <td class="<?php echo $tdClass ?>" align="center"><input name="<?php echo $authorOnID.'[]'?>" type="checkbox" value="<?php echo $key ?>" <?php if ($val->authorOn) { echo 'checked="checked"'; }?>></td>
                                        </tr>
                                <?php
                                }
                                ?>
                            </table>

                            <?php
                            DisplayPaginationControl( $currPage, $groupCount, $pageSize, 'grouprolemanagement.php', NULL, $sortDirection );
                            ?>

                            <?php
                            DisplaySaveCancelBar();
                            EndContentArea( true, $formName, "" );
                            ?>

                    </tr>
                </table>

            </td>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
