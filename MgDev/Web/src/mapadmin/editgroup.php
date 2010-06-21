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
    CheckForCancel( 'groupmanagement.php' );

    // Define local vars
    $menuCategory = GROUP_MANAGEMENT_MENU_ITEM;
    $menuItem = EDIT_GROUP_MENU_ITEM;
    $pageTitle = EDIT_GROUP_TITLE;
    $helpPage = 'HelpDocs/adding_users_to_groups.htm';
    $pageName = 'EditGroupPage';
    $formName = 'EditGroupForm';
    $homePage = 'groupmanagement.php';
    $selectedGroupID = 'SelectedGroup';
    $description = "";
    $descriptionID = 'description';
    $usersSelected = array();
    $usersSelectedID = 'usersSelected';
    $usersNotSelected = array();
    $usersNotSelectedID = 'usersNotSelected';
    $errorMsg = "";
    $confirmationMsg = "";

    // Is this our first time here?
    if ( FirstTimeHere( $pageName ) )
    {
        // Was the selected group passed in the URL?
        if ( array_key_exists( $selectedGroupID, $_GET ) )
        $selectedGroup = $_GET[ $selectedGroupID ];

        // Get info for selected user
        $selectedGroupData = GetDataForGroup( $selectedGroup, true );
        if ( $selectedGroupData == NULL )
        throw new Exception ( sprintf( $errNotFound, $selectedGroup ) );
        $description = $selectedGroupData->description;
        CopyArray( $selectedGroupData->users, $usersSelected );
        $allUsers = GetUsers();
        $usersNotSelected = array_diff( $allUsers, $usersSelected );
    }

    // The save button was hit.
    else
    {
        // Get submitted data
        if ( array_key_exists( $selectedGroupID, $_POST ) )
        $selectedGroup = $_POST[ $selectedGroupID ];
        if ( array_key_exists( $descriptionID, $_POST ) )
        $description = $_POST[ $descriptionID ];
        if ( array_key_exists( $usersSelectedID, $_POST ) )
        $usersSelected = $_POST[ $usersSelectedID ];
        if ( array_key_exists( $usersNotSelectedID, $_POST ) )
        $usersNotSelected = $_POST[ $usersNotSelectedID ];

        // Update parameters of group.
        $site->UpdateGroup( $selectedGroup, "", $description );

        // Create MgStringCollection with just one group to update members of group.
        $groupToUpdate = new MgStringCollection();
        $groupToUpdate->Add( $selectedGroup );

        // Get old list of users for group
        $oldUserList = GetUserMembers( $selectedGroup );

        // Find users to delete from group.
        $revokeList = array_diff( $oldUserList, $usersSelected );
        if ( !empty( $revokeList ) )
        {
            $usersToDelete = new MgStringCollection();
            foreach ( $revokeList as $userToDelete )
            $usersToDelete->Add( $userToDelete );
            $site->RevokeGroupMembershipsFromUsers( $groupToUpdate, $usersToDelete );
        }

        // Find users to add to group.
        $grantList = array_diff( $usersSelected, $oldUserList );
        if ( !empty( $grantList ) )
        {
            $usersToAdd = new MgStringCollection();
            foreach ( $grantList as $userToAdd )
            $usersToAdd->Add( $userToAdd );
            $site->GrantGroupMembershipsToUsers( $groupToUpdate, $usersToAdd );
        }

        // Everything is OK.
        $confirmationMsg = sprintf( $confSuccessfulUpdate, $selectedGroup );
        SaveSessionVars();
        header( 'Location:  groupmanagement.php?'.strip_tags(SID) );
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
                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="editGroupInputLabel">Group name :</td>
                        <td class="inputFormValue"><input class="inputFormValue" disabled name="<?php echo $selectedGroupID?>" type="text" value="<?php echo $selectedGroup?>" size="60"></td>
                    </tr>
                    <tr>
                        <td class="editGroupInputLabel">Description:</td>
                        <td class="inputFormValue"><textarea class="inputFormValue" name="<?php echo $descriptionID?>" cols="60"><?php echo $description?></textarea></td>
                    </tr>
                </table>

                <!-- User Area -->
                <?php
                $control = new AddRemoveControlRecord();
                $control->leadInStr = 'Members';
                $control->col1Name = $usersNotSelectedID.'[]';
                $control->col3Name = $usersSelectedID.'[]';
                $control->col1Heading = 'Available users';
                $control->col3Heading = 'Users in this group';
                $control->col1List = $usersNotSelected;
                $control->col3List = $usersSelected;
                $control->enabled = true;
                $control->size = 10;
                DisplayAddRemoveControl( $control );
                ?>

                <?php
                DisplaySaveCancelBar();
                EndContentArea( true, $formName, $descriptionID );
                ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>

</html>
