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
    CheckForCancel( 'usermanagement.php' );

    // Local global vals.
    $menuCategory = USER_MANAGEMENT_MENU_ITEM;
    $menuItem = EDIT_USER_MENU_ITEM;
    $pageTitle = EDIT_USER_TITLE;
    $helpPage = 'HelpDocs/adding_and_deleting_users.htm';
    $pageName = 'EditUserPage';
    $formName = 'EditUserForm';
    $homePage = 'usermanagement.php';
    $selectedUserID = 'SelectedUser';
    $userID = "";
    $userIDID = 'userID';
    $userName = "";
    $userNameID = 'userName';
    $password = "";
    $passwordID = 'password';
    $passwordConfirmation = "";
    $passwordConfirmationID = 'passwordConfirmation';
    $description = "";
    $descriptionID = 'description';
    $groupsSelected = array();
    $groupsSelectedID = 'groupsSelected';
    $groupsNotSelected = array();
    $groupsNotSelectedID = 'groupsNotSelected';
    $errorMsg = "";
    $confirmationMsg = "";

    // Is this our first time here?
    if ( FirstTimeHere( $pageName ) )
    {
        // Was the selected user passed in the URL?
        if ( array_key_exists( $selectedUserID, $_GET ) )
        $selectedUser = $_GET[ $selectedUserID ];

        // Get info for selected user
        $selectedUserData = GetDataForUser( $selectedUser, true );
        if ( $selectedUserData == NULL )
        throw new Exception ( sprintf( $errNotFound, $selectedUser ) );
        $userID = $selectedUser;
        $userName = $selectedUserData->name;
        $description = $selectedUserData->description;
        CopyArray( $selectedUserData->groups, $groupsSelected );
        $allGroups = GetGroups();
        $groupsNotSelected = array_diff( $allGroups, $groupsSelected );
    }

    // The save button was hit.
    else
    {
        // Get submitted data
        if ( array_key_exists( $userNameID, $_POST ) )
        $userName = $_POST[ $userNameID ];
        if ( array_key_exists( $userIDID, $_POST ) )
        $userID = $_POST[ $userIDID ];
        if ( array_key_exists( $passwordID, $_POST ) )
        $password = $_POST[ $passwordID ];
        if ( array_key_exists( $passwordConfirmationID, $_POST ) )
        $passwordConfirmation = $_POST[ $passwordConfirmationID ];
        if ( array_key_exists( $descriptionID, $_POST ) )
        $description = $_POST[ $descriptionID ];
        if ( array_key_exists( $groupsSelectedID, $_POST ) )
        $groupsSelected = $_POST[ $groupsSelectedID ];
        if ( array_key_exists( $groupsNotSelectedID, $_POST ) )
        $groupsNotSelected = $_POST[ $groupsNotSelectedID ];

        // Validate inputs.
        if ( empty( $userName ) )
        throw new Exception( $errUserNameMissing );
        if ( (!empty( $password ) || !empty( $passwordConfirmation )) && ( $password != $passwordConfirmation ) )
        throw new Exception( $errPasswordConfirmationFailed );

        // Update parameters of user.
        $site->UpdateUser( $userID, "", $userName, $password, $description );

        // Create MgStringCollection with just one user to update group memberships.
        $userToUpdate = new MgStringCollection();
        $userToUpdate->Add( $userID );

        // Get old list of groups for user
        $oldGroupMemberships = GetGroupMemberships( $userID );

        // Find group memberships to revoke.
        $revokeList = array_diff( $oldGroupMemberships, $groupsSelected );
        if ( $revokeList != null && !empty( $revokeList ) )
        {
            $groupMembershipsToRevoke = new MgStringCollection();
            foreach ( $revokeList as $groupToRevoke )
            $groupMembershipsToRevoke->Add( $groupToRevoke );
            $site->RevokeGroupMembershipsFromUsers( $groupMembershipsToRevoke, $userToUpdate );
        }

        // Find new group memberships to grant.
        $grantList = array_diff( $groupsSelected, $oldGroupMemberships );
        if ( $grantList != null && !empty( $grantList ) )
        {
            $groupMembershipsToGrant = new MgStringCollection();
            foreach ( $grantList as $groupToGrant )
            $groupMembershipsToGrant->Add( $groupToGrant );
            $site->GrantGroupMembershipsToUsers( $groupMembershipsToGrant, $userToUpdate );
        }

        // Everything is OK.
        $confirmationMsg = sprintf( $confSuccessfulUpdate, $userID );
        SaveSessionVars();
        header( 'Location:  usermanagement.php?'.strip_tags(SID) );
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
                        <td class="editUserInputLabel">User ID :</td>
                        <td class="inputFormValue"><input class="inputFormValue" disabled name="<?php echo $userIDID?>" type="text" value="<?php echo $userID?>" size="60"></td>
                    </tr>
                    <tr>
                        <td class="editUserInputLabel">User name :</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $userNameID?>" type="text" value="<?php echo $userName?>" size="60"></td>
                    </tr>
                    <tr>
                        <td class="editUserInputLabel">Description:</td>
                        <td class="inputFormValue"><textarea class="inputFormValue" name="<?php echo $descriptionID?>" cols="60"><?php echo $description?></textarea></td>
                    </tr>
                    <?php if ( $userID != MgUser::Anonymous ) { ?>
                    <tr>
                        <td class="editUserInputLabel">Password:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $passwordID?>" type="password" value="<?php echo $password?>" size="60"></td>
                    </tr>
                    <tr>
                        <td class="editUserInputLabel">Confirm password:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $passwordConfirmationID?>" type="password" value="<?php echo $passwordConfirmation?>" size="60"></td>
                    </tr>
                    <?php } ?>
                </table>

                <!-- Group Area -->
                <?php
                $control = new AddRemoveControlRecord();
                $control->leadInStr = 'Member of';
                $control->col1Name = $groupsNotSelectedID.'[]';
                $control->col3Name = $groupsSelectedID.'[]';
                $control->col1Heading = 'Available groups';
                $control->col3Heading = 'Member of these groups';
                $control->col1List = $groupsNotSelected;
                $control->enabled = true;
                $control->size = 10;

                // SPECIAL HANDLING FOR EVERYONE GROUP:
                // If the Everyone group is in the $groupsSelected list (which it should be), it requires special handling.
                // Membership in this group cannot be revoked and, so, it should not be selected.  The preferrable UI is to
                // disable this option in the select control; however, the disable parameter is not recognized by all browsers
                // for the OPTION tag and is not always recognized in JavaScript for this tag as well.  Therefore, the OnChange
                // parameter of the select control is used.  With this an alert box is popped up when Everyone is selected and
                // the selection is reversed.
                // In addition, to suggest that the Everyone group should not be selected at all, it's display name is changed
                // from "Everyone" to "Everyone (Built-in group)".
                if ( in_array( MgGroup::Everyone, $groupsSelected ) )
                {
                    $control->col3List = array();
                    $control->col3Vals = array();
                    foreach( $groupsSelected as $selectedGroup )
                    {
                        if ( $selectedGroup == MgGroup::Everyone )
                        {
                            $control->col3List[] = $everyoneGroupSelectorLabel;
                            $control->col3Vals[] = MgGroup::Everyone;
                        }
                        else
                        {
                            $control->col3List[] = $selectedGroup;
                            $control->col3Vals[] = $selectedGroup;
                        }
                    }
                    $control->col3OnChange = "DisableOption( '".$groupsSelectedID."[]', '".MgGroup::Everyone."', 'The Everyone group is built-in. Membership in this group cannot be removed.');";
                }
                else
                $control->col3List = $groupsSelected;
                DisplayAddRemoveControl( $control );
                ?>

                <?php
                DisplaySaveCancelBar();
                EndContentArea( true, $formName, $userNameID );
                ?>

            <!-- End of Contents Area -->

        </tr>

    </table>
</body>

</html>
