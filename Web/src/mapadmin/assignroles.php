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
    CheckForCancel( 'userrolemanagement.php' );

    // Define local vars
    $menuCategory = ASSIGN_ROLES_MENU_ITEM;
    $menuItem = EDIT_ROLES_MENU_ITEM;
    $pageTitle = EDIT_ROLES_TITLE;
    $helpPage = 'HelpDocs/assigning_roles.htm';
    $pageName = 'AssignRolesPage';
    $formName = 'AssignRolesForm';
    $homePage = 'userrolemanagement.php';
    $selectedRole = "";
    $selectedRoleID = 'selectedRole'.NO_PERSISTENCE_FLAG;
    $usersSelected = array();
    $usersSelectedID = 'usersSelected';
    $usersNotSelected = array();
    $usersNotSelectedID = 'usersNotSelected';
    $groupsSelected = array();
    $groupsSelectedID = 'groupsSelected';
    $groupsNotSelected = array();
    $groupsNotSelectedID = 'groupsNotSelected';
    $refreshRequested = false;
    $errorMsg = "";
    $confirmationMsg = "";

    // Get selectedRole
    if ( FirstTimeHere( $pageName ) )
    {
        $refreshRequested = true;
        $selectedRole = ADMIN_ROLE;
    }
    else
    {
        if ( array_key_exists( $selectedRoleID, $_POST ) )
            $selectedRole = $_POST[ $selectedRoleID ];
        else
            $selectedRole = ADMIN_ROLE;

        $refreshRequested = CheckForRefresh();
    }

    // Are we refreshing?
    if ( $refreshRequested  )
    {
        $userList = GetUsers();
        GetUsersOrGroupsByRole( true, $selectedRole, $usersSelected );
        $usersNotSelected = array_diff( $userList, $usersSelected );

        $groupList = GetGroups();
        GetUsersOrGroupsByRole( false, $selectedRole, $groupsSelected );
        $groupsNotSelected = array_diff( $groupList, $groupsSelected );
    }

    // No refresh - the save button must have been hit.
    else
    {
        // Get submitted data
        if ( array_key_exists( $usersSelectedID, $_POST ) )
            $usersSelected = $_POST[ $usersSelectedID ];
        if ( array_key_exists( $usersNotSelectedID, $_POST ) )
            $usersNotSelected = $_POST[ $usersNotSelectedID ];
        if ( array_key_exists( $groupsSelectedID, $_POST ) )
            $groupsSelected = $_POST[ $groupsSelectedID ];
        if ( array_key_exists( $groupsNotSelectedID, $_POST ) )
            $groupsNotSelected = $_POST[ $groupsNotSelectedID ];

        // Validate inputs.
        if ( empty( $selectedRole ) )
            throw new Exception( $errInvalidInput );

        // Create MgStringCollection with just one user to update group memberships.
        $roleToUpdate = new MgStringCollection();
        $roleToUpdate->Add( $selectedRole );

        // Get old list of users for role.
        GetUsersOrGroupsByRole( true, $selectedRole, $oldUsersForRoleList );

        // Find users to lose role permission.
        $revokeList = array_diff( $oldUsersForRoleList, $usersSelected );
        if ( $revokeList != null && !empty( $revokeList ) )
        {
            $permissionsToRevoke = new MgStringCollection();
            foreach ( $revokeList as $permissionToRevoke )
                $permissionsToRevoke->Add( $permissionToRevoke );
            $site->RevokeRoleMembershipsFromUsers( $roleToUpdate, $permissionsToRevoke );
        }

        // Find users to gain role permission.
        $grantList = array_diff( $usersSelected, $oldUsersForRoleList );
        if ( $grantList != null && !empty( $grantList ) )
        {
            $permissionsToGrant = new MgStringCollection();
            foreach ( $grantList as $permissionToGrant )
                $permissionsToGrant->Add( $permissionToGrant );
            $site->GrantRoleMembershipsToUsers( $roleToUpdate, $permissionsToGrant );
        }

        // Get old list of groups for role
        GetUsersOrGroupsByRole( false, $selectedRole, $oldGroupsForRoleList );

        // Find groups to lose role permission.
        $revokeList = array_diff( $oldGroupsForRoleList, $groupsSelected );
        if ( $revokeList != null && !empty( $revokeList ) )
        {
            $permissionsToRevoke = new MgStringCollection();
            foreach ( $revokeList as $permissionToRevoke )
                $permissionsToRevoke->Add( $permissionToRevoke );
            $site->RevokeRoleMembershipsFromGroups( $roleToUpdate, $permissionsToRevoke );
        }

        // Find groups to gain role permission.
        $grantList = array_diff( $groupsSelected, $oldGroupsForRoleList );
        if ( $grantList != null && !empty( $grantList ) )
        {
            $permissionsToGrant = new MgStringCollection();
            foreach ( $grantList as $permissionToGrant )
                $permissionsToGrant->Add( $permissionToGrant );
            $site->GrantRoleMembershipsToGroups( $roleToUpdate, $permissionsToGrant );
        }

        // Everything is OK.
        $confirmationMsg = sprintf( $confSuccessfulUpdate, $selectedRole );
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

                <table class="inputForm">
                    <tr>
                        <td class="editRolesInputLabel" valign="top">Role:</td>
                        <td class="inputFormValue">
                            <select class="inputFormValue" name="<?php echo $selectedRoleID?>" size="1" onChange="RefreshPage('<?php echo $formName,"', '",REFRESH_PAGE_ID; ?>');">
                                <option <?php if ( $selectedRole == ADMIN_ROLE  ) { echo 'selected'; } ?> ><?php echo ADMIN_ROLE; ?></option>
                                <option <?php if ( $selectedRole == AUTHOR_ROLE ) { echo 'selected'; } ?> ><?php echo AUTHOR_ROLE; ?></option>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2">&nbsp;</td>
                    </tr>
                    <tr>
                        <td class="editRolesInputLabel" valign="top">Description:</td>
                        <?php
                        if ( $selectedRole == ADMIN_ROLE )
                            $roleDescription = $descAdmin;
                        else
                        if ( $selectedRole == AUTHOR_ROLE )
                            $roleDescription = $descAuthor;
                        ?>
                        <td class="inputFormValue"><textarea name="textarea" rows="6" wrap="VIRTUAL" class="inputFormValue"><?php echo $roleDescription; ?></textarea></td>
                    </tr>
                </table>

                <!-- Spacing -->
                <br>

                <?php
                $control = new AddRemoveControlRecord();
                $control->leadInStr = 'Users';
                $control->col1Name = $usersNotSelectedID.'[]';
                $control->col3Name = $usersSelectedID.'[]';
                $control->col1Heading = 'Available users';
                $control->col3Heading = 'Users assigned to this role';
                $control->col1List = $usersNotSelected;
                $control->col3List = $usersSelected;
                $control->enabled = true;
                $control->size = 10;
                DisplayAddRemoveControl( $control );

                echo "<br>";

                $control = new AddRemoveControlRecord();
                $control->leadInStr = 'Groups';
                $control->col1Name = $groupsNotSelectedID.'[]';
                $control->col3Name = $groupsSelectedID.'[]';
                $control->col1Heading = 'Available groups';
                $control->col3Heading = 'Groups assigned to this role';
                $control->col1List = $groupsNotSelected;
                $control->col3List = $groupsSelected;
                $control->enabled = true;
                $control->size = 10;
                DisplayAddRemoveControl( $control );

                DisplaySaveCancelBar();
                EndContentArea( true, $formName, "" );
                ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>
