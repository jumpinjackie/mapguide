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
        CheckForCancel( 'usermanagement.php' );

        // Define local vars
        $menuCategory = USER_MANAGEMENT_MENU_ITEM;
        $menuItem = ADD_USER_MENU_ITEM;
        $pageTitle = ADD_USER_TITLE;
        $helpPage = 'HelpDocs/adding_and_deleting_users.htm';
        $pageName = 'AddUserPage';
        $formName = 'AddUserForm';
        $homePage = 'usermanagement.php';
        $userName = "";
        $userNameID = 'userName';
        $userID = "";
        $userIDID = 'userID';
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
        $confirmationMsg = "";
        $errorMsg = "";

        // Is this our first time here?
        if ( FirstTimeHere( $pageName ) )
        {
            $groups = GetGroups();
            foreach ( $groups as $group )
            {
                if ( $group != MgGroup::Everyone )
                    $groupsNotSelected[] =  $group;
                else
                    $groupsSelected[] = $group;
            }
        }

        // Otherwise, Save button must have been hit
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
            if ( empty( $userID ) )
                throw new Exception( $errUserIDMissing );
            if ( empty( $userName ) )
                throw new Exception( $errUserNameMissing );
            if ( empty( $password ) )
                throw new Exception( $errPasswordMissing );
            if ( $password != $passwordConfirmation )
                throw new Exception( $errPasswordConfirmationFailed );

            // Add new user.
            $site->AddUser( $userID, $userName, $password, $description );

            // Create MgStringCollection with just one user to update group memberships.
            $usersToUpdate = new MgStringCollection();
            $usersToUpdate->Add( $userID );

            // Grant group memberships.
            if ( !empty( $groupsSelected ) )
            {
                $groupMembershipsToGrant = new MgStringCollection();
                foreach ( $groupsSelected as $groupToGrant )
                {
                    if ( $groupToGrant != MgGroup::Everyone )
                        $groupMembershipsToGrant->Add( $groupToGrant );
                }
                if ( $groupMembershipsToGrant->GetCount() > 0 )
                    $site->GrantGroupMembershipsToUsers( $groupMembershipsToGrant, $usersToUpdate );
            }

            // Everything is OK.
            $confirmationMsg = sprintf( $confSuccessfulAddition, $userID );
            $selectedUser = $userID;
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
                        <td class="addUserInputLabel">User ID :</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $userIDID?>" type="text" value="<?php echo $userID?>"></td>
                    </tr>
                    <tr>
                        <td class="addUserInputLabel">User name :</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $userNameID?>" type="text" value="<?php echo $userName?>"></td>
                    </tr>
                    <tr>
                        <td class="addUserInputLabel">Description:</td>
                        <td class="inputFormValue"><textarea class="inputFormValue" name="<?php echo $descriptionID?>"><?php echo $description?></textarea></td>
                    </tr>
                    <tr>
                        <td class="addUserInputLabel">Password:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $passwordID?>" type="password" value="<?php echo $password?>"></td>
                    </tr>
                    <tr>
                        <td width="200px" class="addUserInputLabel">Confirm password:</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $passwordConfirmationID?>" type="password" value="<?php echo $passwordConfirmation?>"></td>
                    </tr>
                </table>

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
                        $control->col3OnChange = "DisableOption( '".$groupsSelectedID."[]', '".MgGroup::Everyone."', 'The Everyone group is built-in and, so, membership in this group cannot be removed.');";
                    }
                    else
                        $control->col3List = $groupsSelected;
                    DisplayAddRemoveControl( $control );

                    DisplaySaveCancelBar();
                ?>

            <?php EndContentArea( true, $formName, $userIDID ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>
</body>

</html>
