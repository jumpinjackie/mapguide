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

// Load display data
try
{
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Define local Vars
    $menuCategory = GROUP_MANAGEMENT_MENU_ITEM;
    $pageTitle = GROUP_MANAGEMENT_MENU_ITEM;
    $helpPage = 'HelpDocs/managing_groups.htm';
    $pageName = 'GroupManagementPage';
    $formName = 'GroupManagementForm';
    $homePage = NULL;
    $selectedGroupID = 'SelectedGroup';
    $groupTable = array();
    $pageSize = 25;
    $firstPageIndex = -1;
    $lastPageIndex = -1;
    $groups = array();
    $groupCount = 0;
    $currPage = 1;
    $sortDirection= DEFAULT_SORT_DIRECTION;
    $sortDirectionID = SORT_DIRECTION_ID;
    $groupRolesTable = array();

    try
    {
        // get input values
        if ( array_key_exists( $selectedGroupID, $_GET ) )
            $selectedGroup = $_GET[ $selectedGroupID ];
        else
        if ( array_key_exists( $selectedGroupID, $_POST ) )
            $selectedGroup = $_POST[ $selectedGroupID ];

        if ( array_key_exists( $sortDirectionID, $_GET ) )
            $sortDirection = $_GET[$sortDirectionID];
        else
        if ( array_key_exists( $sortDirectionID, $_POST ) )
            $sortDirection = $_POST[$sortDirectionID];
        SetGroupSortDirection( $sortDirection );

        // Are we switching to another page?
        CheckForPageSwitch();

        // Are we deleting a group?
        if ( CheckForDeleteSelection() && DeleteGroup( $selectedGroup ) )
            $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedGroup );
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

    $groups = GetGroups();
    $groupCount = sizeof( $groups );
    $currPage = GetPageNumber( $groups, $selectedGroup, $pageSize );
    GetPageRange( $currPage, $groupCount, $pageSize, $firstPageIndex, $lastPageIndex );
    LoadGroupTable( $groupTable, $firstPageIndex, $lastPageIndex, true );
    if ( !array_key_exists( $selectedGroup, $groupTable ) && $groupCount > 0 )
        $selectedGroup = $groups[ $firstPageIndex ];
    if ( $selectedGroup == MgGroup::Everyone )
    {
        if ( $firstPageIndex + 1 <= $lastPageIndex )
        $selectedGroup = $groups[ $firstPageIndex+1 ];
        else
        $selectedGroup = "";
    }
    LoadGroupRolesTable( $groupRolesTable, $firstPageIndex, $lastPageIndex );
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
                <input type="hidden" name="<?php echo $sortDirectionID?>" value="<?php echo $sortDirection?>">
                <input type="hidden" name="<?php echo $selectedGroupID?>" value="<?php echo $selectedGroup?>">

                <?php

                DisplayPaginationControl( $currPage, $groupCount, $pageSize, 'groupmanagement.php', NULL, $sortDirection );

                // Toolbar
                $buttons = array();
                $button = new ToolbarButtonRecord();
                $button->label = "Add Group";
                $button->icon = "images/new.gif";
                $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'addgroup.php');";
                $button->disabled = false;
                $buttons[0] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "Edit Group";
                $button->icon = "images/edit.gif";
                $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'editgroup.php');";
                $button->disabled = !empty( $selectedGroup ) ? false : true;
                $buttons[1] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "Delete Group";
                $button->icon = "images/delete.gif";
                $button->action =
                "SetElementValueWithConfirmation( '".DELETE_SELECTION_ID."', 'Are you sure you want to delete the selected group?')";
                $button->disabled = !empty( $selectedGroup ) ? false : true;
                $buttons[2] = $button;
                DisplayToolbar( $buttons, $formName );
                ?>

                <!-- Group Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <?php
                            $sortIcon = GetSortIcon( $sortDirection );
                            $sortDirectionStr = "SetElementValue( '".$sortDirectionID."', '".GetSortDirection( $sortDirection )."' );";
                        ?>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader"><a href="#" onClick="<?php echo $sortDirectionStr?>SubmitForm('<?php echo $formName?>');">Group Name   <img src="<?php echo $sortIcon?>"></a> </td>
                        <td class="dataHeader">Description</td>
                        <td class="dataHeader">Members</td>
                        <td class="dataHeader">Assigned these Roles </td>
                    </tr>
                    <?php
                    foreach ( $groupTable as $key => $val )
                    {
                        if ( $key == $selectedGroup )
                            $checkedStr = " checked ";
                        else
                            $checkedStr = "";
                        CreateListOfUserHyperlinksForTable( $val->users, $numUsers, $usersStr );
                        CreateListOfRoleHyperlinksForTable( $groupRolesTable[$key], $roleStr, false, $key );
                        ?>
                            <tr>
                                <?php if ( $key == MgGroup::Everyone ) { ?>
                                    <td class="dataCell"></td>
                                    <td class="dataCell"><?php echo $key; ?></td>
                                    <td class="dataCell"><?php echo $val->description; ?></td>
                                    <td class="dataCell"><?php echo $numUsers.$usersStr ?></td>
                                <?php } else { ?>
                                    <td class="dataCell"><input name="groupSelector" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> onClick="SetElementValue( '<?php echo $selectedGroupID ?>', '<?php echo $key?>');" ></td>
                                    <td class="dataCell"><a href="editgroup.php?<?php echo $selectedGroupID?>=<?php echo $key?>"><?php echo $key; ?></a></td>
                                    <td class="dataCell"><?php echo $val->description; ?></td>
                                    <td class="dataCell"><a href="editgroup.php?<?php echo $selectedGroupID?>=<?php echo $key?>"><?php echo $numUsers; ?></a><?php echo $usersStr ?></td>
                                <?php } ?>
                                    <td class="dataCell"><?php echo $roleStr?></td>
                            </tr>
                        <?php
                    }
                        ?>
                    </table>

                <?php
                DisplayPaginationControl( $currPage, $groupCount, $pageSize, 'groupmanagement.php', NULL, $sortDirection );
                ?>

                <?php
                EndContentArea( true, $formName, "" );
                ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>

</html>
