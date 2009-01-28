<?php

//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
        $menuItem = ADD_GROUP_MENU_ITEM;
        $pageTitle = ADD_GROUP_TITLE;
        $helpPage = 'HelpDocs/adding_and_deleting_groups.htm';
        $pageName = 'AddGroupPage';
        $formName = 'AddGroupForm';
        $homePage = 'groupmanagement.php';
        $helpPage = 'HelpDocs/adding_and_deleting_groups.htm';
        $groupName = "";
        $groupNameID = 'groupName';
        $description = "";
        $descriptionID = 'description';
        $usersSelected = array();
        $usersSelectedID = 'usersSelected';
        $usersNotSelected = array();
        $usersNotSelectedID = 'usersNotSelected';
        $confirmationMsg = "";
        $errorMsg = "";

        // Is this our first time here?
        if ( FirstTimeHere( $pageName ) )
            $usersNotSelected = GetUsers();

        // Otherwise, Save button must have been hit
        else
        {
            // Get submitted data
            if ( array_key_exists( $groupNameID, $_POST ) )
                $groupName = $_POST[ $groupNameID ];
            if ( array_key_exists( $descriptionID, $_POST ) )
                $description = $_POST[ $descriptionID ];
            if ( array_key_exists( $usersSelectedID, $_POST ) )
                $usersSelected = $_POST[ $usersSelectedID ];
            if ( array_key_exists( $usersNotSelectedID, $_POST ) )
                $usersNotSelected = $_POST[ $usersNotSelectedID ];

            // Validate inputs.
            if ( empty( $groupName ) )
                throw new Exception( $errGroupNameMissing );

            // Add the specified group.
            $site->AddGroup( $groupName, $description );

            // Create MgStringCollection with just one group to update user memberships.
            $groupsToUpdate = new MgStringCollection();
            $groupsToUpdate->Add( $groupName );

            // Add users to group.
            if ( !empty( $usersSelected ) )
            {
                $usersToAdd = new MgStringCollection();
                foreach ( $usersSelected as $userToAdd )
                    $usersToAdd->Add( $userToAdd );
                $site->GrantGroupMembershipsToUsers( $groupsToUpdate, $usersToAdd );
            }

            // Everything is OK.
            $confirmationMsg = sprintf( $confSuccessfulAddition, $groupName );
            $selectedGroup = $groupName;
            SaveSessionVars();
            header( 'Location:  groupmanagement.php?'.strip_tags(SID) );
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
            <?php DisplayLefthandSash( $menuItem); ?>

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
                        <td class="addGroupInputLabel">Group name :</td>
                        <td class="inputFormValue"><input class="inputFormValue" name="<?php echo $groupNameID?>" type="text" value="<?php echo $groupName?>"></td>
                    </tr>
                    <tr>
                        <td class="addGroupInputLabel">Description:</td>
                        <td class="inputFormValue"><textarea class="inputFormValue" name="<?php echo $descriptionID?>" <?php echo $description?>></textarea></td>
                    </tr>
                </table>

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

                <?php DisplaySaveCancelBar(); ?>

            <?php EndContentArea( true, $formName, $groupNameID ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>

</html>
