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

    ////// Website strings //////

    define( 'LOGIN_TITLE',  "Login" );

    define( 'SERVER_MANAGEMENT_MENU_ITEM',          "Manage Servers" );
    define( 'ADD_SERVER_MENU_ITEM',                 "Add Server" );
    define( 'CONFIGURE_SERVER_MENU_ITEM',           "Configure Server" );
    define( 'CONFIGURE_SERVICES_MENU_ITEM',         "Configure Services" );
    define( 'CONFIGURE_LOGS_MENU_ITEM',             "Configure Logs" );
    define( 'LOG_MANAGEMENT_MENU_ITEM',             "Manage Logs" );
    $serversMenuGroup[ SERVER_MANAGEMENT_MENU_ITEM ] =          'servermanagement.php';
    $serversMenuGroup[ CONFIGURE_SERVER_MENU_ITEM ] =           'serverproperties.php';
    $serversMenuGroup[ CONFIGURE_SERVICES_MENU_ITEM ] =         'servicesproperties.php';
    $serversMenuGroup[ CONFIGURE_LOGS_MENU_ITEM ] =             'logproperties.php';
    $serversMenuGroup[ LOG_MANAGEMENT_MENU_ITEM ] =             'logmanagement.php';

    define( 'GROUP_MANAGEMENT_MENU_ITEM',   "Manage Groups" );
    define( 'ADD_GROUP_MENU_ITEM',          "Add Group" );
    define( 'EDIT_GROUP_MENU_ITEM',         "Edit Group" );
    $groupsMenuGroup[ GROUP_MANAGEMENT_MENU_ITEM ] =    'groupmanagement.php';
    $groupsMenuGroup[ ADD_GROUP_MENU_ITEM ] =       'addgroup.php';
    $groupsMenuGroup[ EDIT_GROUP_MENU_ITEM ] =      'editgroup.php';

    define( 'USER_MANAGEMENT_MENU_ITEM',    "Manage Users" );
    define( 'ADD_USER_MENU_ITEM',           "Add User" );
    define( 'EDIT_USER_MENU_ITEM',          "Edit User" );
    $usersMenuGroup[ USER_MANAGEMENT_MENU_ITEM ] =  'usermanagement.php';
    $usersMenuGroup[ ADD_USER_MENU_ITEM ] =             'adduser.php';
    $usersMenuGroup[ EDIT_USER_MENU_ITEM ] =            'edituser.php';

    define( 'ASSIGN_ROLES_MENU_ITEM',           "Assign Roles" );
    define( 'EDIT_ROLES_MENU_ITEM',             "Edit Roles" );
    $rolesMenuGroup[ ASSIGN_ROLES_MENU_ITEM ] =     'userrolemanagement.php';
    $rolesMenuGroup[ EDIT_ROLES_MENU_ITEM ] =       'assignroles.php';

    define( 'PACKAGE_MANAGEMENT_MENU_ITEM', "Manage Packages" );
    $packagesMenuGroup[ PACKAGE_MANAGEMENT_MENU_ITEM ] = 'packagemanagement.php';

    define( 'CONFIGURE_WMS_MENU_ITEM', "Configure WMS" );
    $wmsMenuGroup[ CONFIGURE_WMS_MENU_ITEM ] = 'wmsproperties.php';

    define( 'CONFIGURE_WFS_MENU_ITEM', "Configure WFS" );
    $wfsMenuGroup[ CONFIGURE_WFS_MENU_ITEM ] = 'wfsproperties.php';

    define( 'ADD_SERVER_TITLE',                 "Add Server" );
    define( 'CONFIGURE_SERVER_TITLE',           "Configure Server" );
    define( 'CONFIGURE_LOGS_TITLE',             "Configure Logs" );
    define( 'CONFIGURE_SERVICES_TITLE',         "Configure Services" );
    define( 'LOG_MANAGEMENT_TITLE',             "Manage Logs" );
    define( 'SELECT_SITE_SERVER_TITLE',         "Select Site Server" );
    define( 'UNMANAGEDDATA_MANAGEMENT_TITLE',   "Configure External Files" );
    define( 'ADD_UNMANAGEDDATA_TITLE',          "Add Alias" );
    define( 'EDIT_UNMANAGEDDATA_TITLE',         "Edit Alias" );

    define( 'ADD_GROUP_TITLE',                  "Add Group" );
    define( 'EDIT_GROUP_TITLE',                 "Edit Group" );

    define( 'ADD_USER_TITLE',                   "Add User" );
    define( 'EDIT_USER_TITLE',                  "Edit User" );

    define( 'ASSIGN_ROLES_TITLE',               "Assign Roles" );
    define( 'EDIT_ROLES_TITLE',                 "Edit Roles" );

    define( 'LOAD_PACKAGES_TITLE',              "Load Packages" );

    define( 'CONFIGURE_WMS_TITLE',              "Configure WMS" );

    define( 'CONFIGURE_WFS_TITLE',              "Configure WFS" );


    define( 'UNMANAGEDDATA_MANAGEMENT_MENU_ITEM',                    "Configure External Files" );
    define( 'ADD_UNMANAGEDDATA_MENU_ITEM',                           "Add Alias" );
    define( 'EDIT_UNMANAGEDDATA_MENU_ITEM',                          "Edit Alias" );
    $unmanagedDataMenuGroup[ UNMANAGEDDATA_MANAGEMENT_MENU_ITEM ] =  'unmanageddatamanagement.php';
    $unmanagedDataMenuGroup[ ADD_UNMANAGEDDATA_MENU_ITEM ] =         'addunmanageddata.php';
    $unmanagedDataMenuGroup[ EDIT_UNMANAGEDDATA_MENU_ITEM ] =        'editunmanageddata.php';

    define( 'HELP_TITLE',                       "Help" );


    $logLabels = array();
    $logLabels[ ACCESS_LOG         ] = 'Access';
    $logLabels[ ADMIN_LOG          ] = 'Admin';
    $logLabels[ AUTHENTICATION_LOG ] = 'Authentication';
    $logLabels[ ERROR_LOG          ] = 'Error';
    $logLabels[ SESSION_LOG        ] = 'Session';
    $logLabels[ TRACE_LOG          ] = 'Trace';

    $logLabelsWithActiveLabel = array();
    $logLabelsWithActiveLabel[ ACTIVE_LOGS        ] = 'Current (Live)';
    $logLabelsWithActiveLabel[ ACCESS_LOG         ] = 'Access';
    $logLabelsWithActiveLabel[ ADMIN_LOG          ] = 'Admin';
    $logLabelsWithActiveLabel[ AUTHENTICATION_LOG ] = 'Authentication';
    $logLabelsWithActiveLabel[ ERROR_LOG          ] = 'Error';
    $logLabelsWithActiveLabel[ SESSION_LOG        ] = 'Session';
    $logLabelsWithActiveLabel[ TRACE_LOG          ] = 'Trace';


    ////// Hidden Vars //////

    define( 'PAGE_NAME_ID',         "PageNameID" );
    define( 'SAVE_DATA_ID',         "SaveDataID" );
    define( 'CANCEL_PAGE_ID',       "CancelPageID" );
    define( 'REFRESH_PAGE_ID',      "RefreshPageID" );
    define( 'NEXT_PAGE_ID',         "NextPageID" );
    define( 'DELETE_SELECTION_ID',  "DeleteSelectionID" );
    define( 'LOGOUT_ID',            "LogoutID" );
    define( 'ERROR_MSG_ID',         "ErrorMsg" );
    define( 'POPUP_NAME_ID',        "PopupNameID" );

    function DeclareHiddenVars( $pageName )
    {
        echo '<input type="hidden" name="',PAGE_NAME_ID,        '" value="',$pageName,'">', "\n";
        echo '<input type="hidden" name="',SAVE_DATA_ID,        '" value="">', "\n";
        echo '<input type="hidden" name="',CANCEL_PAGE_ID,      '" value="">', "\n";
        echo '<input type="hidden" name="',REFRESH_PAGE_ID,     '" value="">', "\n";
        echo '<input type="hidden" name="',NEXT_PAGE_ID,        '" value="">', "\n";
        echo '<input type="hidden" name="',DELETE_SELECTION_ID, '" value="" >',"\n";
        echo '<input type="hidden" name="',LOGOUT_ID,           '" value="" >',"\n";
        echo '<input type="hidden" name="',POPUP_NAME_ID,           '" value="" >',"\n";
    }

    function GetHiddenVar( $varID )
    {
        if ( array_key_exists( $varID, $_POST ) )
            return $_POST[ $varID ];
        else
            return "";
    }

    function CheckForSaveData()
    {
        if ( GetHiddenVar( SAVE_DATA_ID ) != "" )
            return true;
        else
            return false;
    }

    function CheckForCancel( $redirection )
    {
        global $confirmationMsg;
        global $errorMsg;

        if ( GetHiddenVar( CANCEL_PAGE_ID ) != "" )
        {
            // Redirect to specified page.
            $confirmationMsg = "";
            $errorMsg = "";
            SaveSessionVars();
            $headerStr = 'Location:  '.$redirection;
            if ( strpos( $redirection, '?' ) === false )
                $headerStr = $headerStr.'?';
            else
                $headerStr = $headerStr.'&';
            $headerStr = $headerStr.strip_tags(SID);
            header( $headerStr );
            exit();
        }
    }

    function SavePopup( $popupName )
    {
        global $popups;

        if ( !empty( $popupName ) && !in_array( $popupName, $popups ) )
            $popups[] = $popupName;
    }

    function CheckForPopupRegistration()
    {
        SavePopup( GetHiddenVar( POPUP_NAME_ID ) );
    }

    function FirstTimeHere( $pageName )
    {
        if ( $pageName == GetHiddenVar( PAGE_NAME_ID ) )
            return false;
        else
            return true;
    }

    function CheckForLogout()
    {
        if ( GetHiddenVar( LOGOUT_ID ) != "" )
        {
            header( 'Location: login.php' );
            exit();
        }
    }

    function CheckForPageSwitch()
    {
        $nextPage = GetHiddenVar( NEXT_PAGE_ID );
        if ( $nextPage != "" )
            SwitchToPage( $nextPage );
    }

    function SwitchToPage( $destPage )
    {
        global $confirmationMsg;
        global $selectedUser;
        global $selectedGroup;

        if ( $destPage != "" )
        {
            $confirmationMsg = "";
            SaveSessionVars();
            $nextHeader = 'Location:  '.$destPage;
            if ( strpos( $destPage, '?' ) === false )
                $nextHeader = $nextHeader.'?';
            else
                $nextHeader = $nextHeader.'&';
            $nextHeader = $nextHeader.strip_tags(SID);
            header( $nextHeader );
            exit();
        }
    }

    function CheckForRefresh()
    {
        if ( GetHiddenVar( REFRESH_PAGE_ID ) != "" )
            return true;
        else
            return false;
    }

    function CheckForDeleteSelection()
    {
        if ( GetHiddenVar( DELETE_SELECTION_ID ) == "true" )
            return true;
        else
            return false;
    }

    function GetSortDirection( $sortdir )
    {
        return $sortdir === ASCENDING_SORT_DIRECTION ? DESCENDING_SORT_DIRECTION : ASCENDING_SORT_DIRECTION;
    }

    function GetSortIcon( $sortDir )
    {
        if ( $sortDir === ASCENDING_SORT_DIRECTION )
            return 'images/sort_ascending.gif';
        else
            return 'images/sort_descending.gif';
    }

    ////// Display Page Components/Controls //////

    function OutputHeader( $pageTitle )
    {
        echo '<head>',"\n";
        echo '  <title>',APP_TITLE,' - ',$pageTitle,'</title>', "\n";

        echo '  <meta content="text/html; charset=utf-8" http-equiv="Content-Type">',"\n";
        echo '  <meta http-equiv="content-script-type" content="text/javascript" />',"\n";
        echo '  <meta http-equiv="content-style-type" content="text/css" />', "\n";

        echo '  <link href="css/resizablestyles.css" rel="stylesheet" type="text/css">', "\n";

        echo '  <script type="text/javascript" src="js/scripts.js"></script>', "\n";
        echo '</head>',"\n";
    }

    function DisplaySiteNameAndLogo()
    {
        $bannerTitle2 = BANNER_TITLE2;
        $bannerTitle = empty( $bannerTitle2 ) ? BANNER_TITLE1 : BANNER_TITLE1.'<br>'.BANNER_TITLE2;

        echo '      <table class="nameAndLogo">',"\n";
        echo '          <tr>',"\n";
        echo '              <td>',"\n";
        echo '                  <div class="appName">'.$bannerTitle.'</div>',"\n";
        echo '              </td>',"\n";

        $appLogo = APP_LOGO;
        if ( !empty( $appLogo) )
        {
            echo '              <td>',"\n";
            echo '                  <div align="right" class="logo"><img src="'.APP_LOGO.'"></div>',"\n";
            echo '              </td>',"\n";
        }

        echo '          </tr>',"\n";
        echo '      </table>',"\n";
    }

    function DisplayHorizontalNavigationBar( $bLogout, $formName, $homePage )
    {
        echo '      <table class="horizontalNav">', "\n";
        echo '          <tr>', "\n";
        echo '              <td class="horizontalNav">', "\n";
        if ( $homePage != NULL )
        {
        echo '                  <a href="',$homePage,'" class="navLinkBold">Home</a>',"\n";
        echo '                  &nbsp;&nbsp;|&nbsp;&nbsp;', "\n";
        }
        echo '                  <a href="#" class="navLinkBold" onClick="window.open( ', "'about.php', 'AdminAboutBox', 'width=600, height=550, scrollbars=yes, resizable=yes, toolbar=yes, menubar=yes'",' ); return false;">About</a>', "\n";
        if ( $bLogout )
        {
        echo '                  &nbsp;&nbsp;|&nbsp;&nbsp;', "\n";
        echo '                  <a href="#" class="navLinkBold" onClick="Logout( ',"'",$formName,"', '",LOGOUT_ID,"'",' )">Logout</a>', "\n";
        }
        echo '              </td>', "\n";
        echo '          </tr>', "\n";
        echo '      </table>', "\n";
    }

    function DisplayBanner()
    {
        $bannerTitle = APP_TITLE;
        echo '      <table border="0" cellspacing="0" cellpadding="2" class="'.BANNER_CLASS.'">',"\n";
        echo '          <tr>',"\n";
        echo '              <td class="'.BANNER_CLASS.'">'.$bannerTitle.'</td>',"\n";
        echo '          </tr>',"\n";
        echo '      </table>',"\n";
    }

    function DisplaySiteHeader( $bNameAndLogo, $bBanner, $bHorizontalNavBar, $bLogout, $formName, $homePage )
    {
        echo '  <td colspan="2" class="header">',"\n";
        if ( $bNameAndLogo )
                    DisplaySiteNameAndLogo();
        if ( $bBanner )
                    DisplayBanner();
        if ( $bHorizontalNavBar )
                    DisplayHorizontalNavigationBar( $bLogout, $formName, $homePage );
        echo '  </td>',"\n";
    }

    function DisplayMenuGroup( $currMenuItem, $topItem, $menuGroup )
    {
        $indent = "";
        $class = "navLinkHeading";
        foreach( $menuGroup as $menuItem => $href )
        {
            if ( $currMenuItem != $menuItem && ($menuItem == EDIT_GROUP_MENU_ITEM || $menuItem == EDIT_USER_MENU_ITEM || $menuItem == EDIT_UNMANAGEDDATA_MENU_ITEM ) )
                continue;
            echo '      <tr>',"\n";
            if ( $currMenuItem == $menuItem )
                echo '          <td class="'.$class.'">'.$indent.$menuItem.'</td>'."\n";
            else
                echo '          <td class="'.$class.'">'.$indent.'<a href="'.$href.'" >'.$menuItem.'</a></td>'."\n";
            echo '      </tr>',"\n";

            $indent = "&nbsp;&nbsp;&nbsp;";
            $class = "navLinkBold";
        }
    }

    function DisplayLefthandSash( $currMenuItem )
    {
        global $serversMenuGroup;
        global $groupsMenuGroup;
        global $usersMenuGroup;
        global $rolesMenuGroup;
        global $packagesMenuGroup;
        global $wmsMenuGroup;
        global $wfsMenuGroup;
        global $unmanagedDataMenuGroup;

        echo '<td class="lhSash">',"\n";
        echo '  <table cellspacing="0" cellpadding="0" border="0">',"\n";

        DisplayMenuGroup( $currMenuItem, SERVER_MANAGEMENT_MENU_ITEM, $serversMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, GROUP_MANAGEMENT_MENU_ITEM, $groupsMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, USER_MANAGEMENT_MENU_ITEM, $usersMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, ASSIGN_ROLES_MENU_ITEM, $rolesMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, PACKAGE_MANAGEMENT_MENU_ITEM, $packagesMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, CONFIGURE_WMS_MENU_ITEM, $wmsMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, CONFIGURE_WFS_MENU_ITEM, $wfsMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        DisplayMenuGroup( $currMenuItem, UNMANAGEDDATA_MANAGEMENT_MENU_ITEM, $unmanagedDataMenuGroup );
        echo "<tr><td><hr></td></tr>\n";
        echo '  </table>',"\n";
        echo '</td>',"\n";
    }

    function BeginContentArea( $formName )
    {
        echo '  <td class="content">',"\n";
        echo '      <form name="',$formName,'" action="',$_SERVER['PHP_SELF'],'" method="post" onSubmit="PersistAllInputFields();">',"\n";
    }

    function EndContentArea( $saveSessionVars, $formName, $initialFocus )
    {
        global $confirmationMsg;
        global $errorMsg;
        global $packageLoadErrorMsg;

        if ( $saveSessionVars )
        {
            $confirmationMsg = "";
            $errorMsg = "";
            $packageLoadErrorMsg = "";
            SaveSessionVars();
        }

        if ( $formName != NULL )
        {
            echo '      </form>',"\n";
            if ( $initialFocus != NULL )
                echo '      <script type="text/javascript" >document.'.$formName.'.'.$initialFocus.'.focus();</script>',"\n";
        }

        echo '  </td>',"\n";
    }

    function DisplayTitleBar( $title, $helpPage )
    {
        $hrColspan = $helpPage == NULL ? '1' : '2';

        echo '<table class="titleBar">',"\n";
        echo '  <tr>',"\n";
        echo '      <td class="pageTitle">'.$title.'</td>',"\n";
        DisplayHelpButton( $helpPage );
        echo '  </tr>',"\n";
        echo '  <tr>',"\n";
        echo '      <td class="titleBar" colspan="'.$hrColspan.'"><hr noshade class="titleBar"></td>',"\n";
        echo '  </tr>',"\n";
        echo '</table>',"\n";
    }

    function DisplayConfirmationMsg( $msg )
    {
        if ( !empty( $msg ) )
        {
            echo '<div class="confirmationMsg">',"\n";
            echo $msg;
            echo '<br></div>',"\n";
            echo '<br>',"\n";
        }
    }

    function DisplayErrorMsg( $msg )
    {
        if ( !empty( $msg ) )
        {
            echo '<div class="errorMsg">',"\n";
            echo $msg;
            echo '<br></div>',"\n";
            echo '<br>',"\n";
        }
    }

    function DisplaySiteServerSelector( $selectedSiteServerID )
    {
        global $site;

        echo '<select class="serverSelector" name="',$selectedSiteServerID.NO_PERSISTENCE_FLAG,'" size="1" >', "\n";
        for($i = 0; $i < $site->GetSiteServerCount(); $i++)
        {
            $selectedStr = "";
            $siteInfo = $site->GetSiteServerInfo($i);
            if($siteInfo != NULL)
            {
                $target = $siteInfo->GetTarget();
                $port = $siteInfo->GetPort(MgSiteInfo_Admin);
                echo '<option class="serverSelector" ',$selectedStr,'>',$target,':',$port,'</option>', "\n";
            }
        }
        echo '</select>', "\n";
    }

    function GetSiteServerSelection( $selectedSiteServerID )
    {
        global $site;
        global $_POST;
        global $_GET;
        global $errNotFound;
        $selectedSite = NULL;
        $selectedSiteServerName = "";
        if ( array_key_exists( $selectedSiteServerID.NO_PERSISTENCE_FLAG, $_POST ) )
        {
            $selectedSiteServerName = $_POST[ $selectedSiteServerID.NO_PERSISTENCE_FLAG ];
        }
        else
        {
            if ( array_key_exists( $selectedSiteServerID, $_GET ) )
                $selectedSiteServerName = $_GET[ $selectedSiteServerID ];
        }
        if($site != NULL && $selectedSiteServerName != "")
        {
            for($i = 0; $i < $site->GetSiteServerCount(); $i++)
            {
                $siteInfo = $site->GetSiteServerInfo($i);
                if($siteInfo != NULL)
                {
                    $target = $siteInfo->GetTarget();
                    $port = $siteInfo->GetPort(MgSiteInfo_Admin);
                    $id = $target.":".strval($port);
                    if($id == $selectedSiteServerName)
                    {
                        $selectedSite = $siteInfo;
                    }
                }
            }
        }
        return $selectedSite;
    }


    class ToolbarButtonRecord
    {
        var $id;
        var $label;
        var $icon;
        var $action;
        var $disabled;
        var $submitForm;

        function ToolbarButtonRecord()
        {
            $this->id = "";
            $this->label = "";
            $this->icon = "";
            $this->action = "";
            $this->disabled = false;
            $this->submitForm = true;
        }
    }
    function DisplayStandaloneToolbar( $buttons, $formName, $class )
    {
        CreateToolbar( $buttons, $formName, $class );
    }
    function DisplayToolbar( $buttons, $formName )
    {
        CreateToolbar( $buttons, $formName, "toolbar" );
    }
    function CreateToolbar( $buttons, $formName, $class )
    {
        if ( $buttons == null || empty( $buttons ) )
            return;

        echo '<table class="'.$class.'" >',"\n";
        echo '  <tr>',"\n";
        echo '      <td class="toolbar">',"\n";

        foreach ( $buttons as $button )
        {
            if ( !$button->disabled )
            {
                echo '  <a href="#"';

                if ( !empty( $button->id ) )
                    echo  ' name="', $button->id, '"', "\n";

                $haveAction = !empty( $button->action );
                $haveForm = !empty( $formName );
                if ( $haveAction || $haveForm )
                {
                    echo '      onClick="';
                    if ( $haveAction )
                        echo $button->action;
                    if ( $haveAction && $haveForm )
                        echo ";";
                    if ( $haveForm && $button->submitForm )
                        echo "SubmitForm('",$formName,"');";
                    echo '"';
                }

                echo "  >\n";
            }

            if ( !empty( $button->icon ) )
                echo '  <img src="', $button->icon, '">', "\n";
            if ( !empty( $button->label ) )
                echo  ' ', $button->label, "\n";

            if ( !$button->disabled )
                echo  '</a>',"\n";
            else
                echo "\n";

            echo '&nbsp;&nbsp;&nbsp;', "\n";
        }
        echo '      </td>',"\n";
        echo '  </tr>',"\n";
        echo '</table>',"\n";
    }

    function CreateJavaScriptArrayStr( $arrayName, $values )
    {
        $arrayStr = ' var '.$arrayName.' = ';

        if ( $values != null )
        {
            $arrayStr = $arrayStr.'new Array(';
            $separator = ' ';
            foreach ( $values as $value )
            {
                $arrayStr = $arrayStr.$separator."'".$value."'";
                $separator = ', ';
            }
            $arrayStr = $arrayStr.' );';
        }
        else
            $arrayStr = $arrayStr.'null;';

        return $arrayStr;
    }

    define( 'NO_PERSISTENCE_FLAG', "_DO_NOT_PERSIST" );
    function DisplaySaveCancelBar()
    {
        echo "&nbsp;<br>";

        echo "<!-- Save/Cancel Bar -->\n";
        echo '<table class="saveCancel" border="0" cellspacing="0" cellpadding="4">',"\n";
        echo '  <tr>', "\n";
        echo '      <td class="saveCancel">',"\n";
        echo '          <input class="saveCancelButtons" name="Save" type="submit"  onClick="SetElementValue(',"'",SAVE_DATA_ID,"', 'true' );",'"value="Save">',"\n";
        echo '          &nbsp;',"\n";
        echo '          <input  class="saveCancelButtons" name="Cancel" type="submit" onClick="SetElementValue(',"'",CANCEL_PAGE_ID,"', 'true' );",'"value="Cancel">',"\n";
        echo '      </td>',"\n";
        echo '  </tr>',"\n";
        echo '</table>',"\n";

        echo "&nbsp;<br>";
    }

    define( 'ADD_BUTTON_ID',        "AddButton" );
    define( 'REMOVE_BUTTON_ID',     "RemoveButton" );

    class AddRemoveControlRecord
    {
        var $enabled;
        var $size;
        var $leadInStr;
        var $col1Name;
        var $col3Name;
        var $col1Heading;
        var $col3Heading;
        var $col1List;
        var $col3List;
        var $col1OnChange;
        var $col3OnChange;
        var $col1Vals;
        var $col3Vals;

        function AddRemoveControlRecord()
        {
            $this->enabled = false;
            $this->size = 10;
            $this->leadInStr = "";
            $this->col1Name = "";
            $this->col3Name = "";
            $this->col1Heading = "";
            $this->col3Heading = "";
            $this->col1List = array();
            $this->col3List = array();
            $this->col1OnChange = "";
            $this->col3OnChange = "";
            $this->col1Vals = array();
            $this->col3Vals = array();
        }
    }
    function DisplayAddRemoveControl( $control )
    {
        // Display Lead In, if specified
        if ( !empty( $control->leadInStr ) )
        {
            echo '&nbsp;',"\n";
            echo '<br>',"\n";
            echo '  <table border="0" cellspacing="0" cellpadding="4" width="100%" >',"\n";
            echo '      <tr>',"\n";
            echo '          <td class="PropertyBar" colspan="2">',$control->leadInStr,'</td>',"\n";
            echo '      </tr>',"\n";
            echo '  </table>',"\n";
            echo '&nbsp;',"\n";
            echo '<br>',"\n";
        }

        // Is the control enabled?
        if ( $control->enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled";

        // Display control
        echo '<table class="addRemoveControl">',"\n";

        echo '  <tr>',"\n";
        echo '      <td class="addRemoveSelectorHeading">',$control->col1Heading,'</td>',"\n";
        echo '      <td class="addRemoveButtons"></td>',"\n";
        echo '      <td class="addRemoveSelectorHeading">',$control->col3Heading,'</td>',"\n";
        echo '  </tr>',"\n";

        echo '  <tr>',"\n";
        echo '      <td class="addRemoveSelector">',"\n";
                        $onChangeStr = !empty( $control->col1OnChange ) ? ' onChange="'.$control->col1OnChange.'" ' : '';
        echo '          <select name="',$control->col1Name,'"',$enabledStr,' size="',$control->size,'"',$onChangeStr,' multiple="multiple" class="addRemoveSelector">',"\n";
                        $i = 0;
                        foreach ( $control->col1List as $col1Item )
                        {
                            $col1Val = !empty( $control->col1Vals ) ? $control->col1Vals[$i] : $col1Item;
                            $i++;
        echo '              <option value="',$col1Val,'">',$col1Item,"</option>\n";
                        }
        echo '          </select>',"\n";
        echo '      </td>',"\n";
        echo '      <td class="addRemoveButtons">',"\n";
        echo '          <input type="button" name = "',ADD_BUTTON_ID,'" value="Add >>" class="addRemoveButtons"',$enabledStr,' onClick="MoveSelectionFromAtoB(',"'",$control->col1Name,"', '",$control->col3Name,"'",')">',"\n";
        echo '          <br><br>',"\n";
        echo '          <input type="button" name = "',REMOVE_BUTTON_ID,'" value="<< Remove" class="addRemoveButtons"',$enabledStr,' onClick="MoveSelectionFromAtoB(',"'",$control->col3Name,"', '",$control->col1Name,"'",')">',"\n";
        echo '      </td>',"\n";
        echo '      <td class="addRemoveSelector">',"\n";
                        $onChangeStr = !empty( $control->col3OnChange ) ? ' onChange="'.$control->col3OnChange.'" ' : '';
        echo '          <select name="',$control->col3Name,'"',$enabledStr,' size="',$control->size,'"',$onChangeStr,' multiple="multiple" class="addRemoveSelector">',"\n";
                        $i=0;
                        foreach ( $control->col3List as $col3Item )
                        {
                            $col3Val = !empty( $control->col3Vals ) ? $control->col3Vals[$i] : $col3Item;
                            $i++;
        echo '              <option value="',$col3Val,'">',$col3Item,"</option>\n";
                        }
        echo '          </select>',"\n";
        echo '      </td>',"\n";
        echo '  </tr>',"\n";

        echo '</table>',"\n";
    }

    function DisplayServerSelector( $selectedServerID, $selectedServer, $formName )
    {
        $serverTable = GetServerData();

        echo '<select class="serverSelector" name="',$selectedServerID.NO_PERSISTENCE_FLAG,'" size="1" onChange="RefreshPage(',"'",$formName,"', '",REFRESH_PAGE_ID,"'",');" >', "\n";

        foreach ( $serverTable as $serverKey => $serverRec )
        {
            if ( $selectedServer == $serverKey )
                $selectedStr = "selected";
            else
                $selectedStr = "";
            echo '<option class="serverSelector" ',$selectedStr,'>',$serverRec->name,'</option>', "\n";
        }

        echo '</select>', "\n";
    }

    function GetServerSelection( $selectedServerID, &$selectedServer )
    {
        global $site;
        global $_POST;
        global $_GET;
        global $errNotFound;

        $serverTable = GetServerData();

        if ( array_key_exists( $selectedServerID.NO_PERSISTENCE_FLAG, $_POST ) )
        {
            $selectedServerName = $_POST[ $selectedServerID.NO_PERSISTENCE_FLAG ];
            foreach ( $serverTable as $serverKey => $serverRec )
            {
                if ( $serverRec->name == $selectedServerName )
                {
                    $selectedServer = $serverKey;
                    break;
                }
            }
        }
        else
        {
            if ( array_key_exists( $selectedServerID, $_GET ) )
                $selectedServer = $_GET[ $selectedServerID ];
        }
        if ( !array_key_exists( $selectedServer, $serverTable ) )
            throw new Exception( sprintf( $errNotFound, $selectedServer ) );
    }

    class ServiceSelectorRecord
    {
        var $serverAddress;
        var $drawingOnID;
        var $drawingOn;
        var $featureOnID;
        var $featureOn;
        var $mappingOnID;
        var $mappingOn;
        var $renderingOnID;
        var $renderingOn;
        var $tileOnID;
        var $tileOn;
        var $enabled;

        function ServiceSelectorRecord()
        {
            $this->serverAddress = "";
            $this->drawingOnID = "drawingServiceOn";
            $this->drawingOn = true;
            $this->featureOnID = "featureServiceOn";
            $this->featureOn = true;
            $this->mappingOnID = "mappingServiceOn";
            $this->mappingOn = true;
            $this->renderingOnID = "renderingServiceOn";
            $this->renderingOn = false;
            $this->tileOnID = "tileServiceOn";
            $this->tileOn = true;
            $this->enabled = true;
        }
    }

    function DisplayServiceSelector( $serviceSelector )
    {
        global $site;

        if ( $serviceSelector == NULL )
            return;

        $siteServerAddress = $site->GetCurrentSiteAddress();
        if ( $serviceSelector->serverAddress == $siteServerAddress )
        {
            echo '<input type="checkbox" checked disabled>Site<br>', "\n";
            echo '<input type="checkbox" checked disabled><a href="servicesproperties.php?Server=',$serviceSelector->serverAddress,'">Resource</a><br>', "\n";
        }

        if ( $serviceSelector->enabled )
            $enabledStr = "";
        else
            $enabledStr = "disabled ";

        echo '<input name="',$serviceSelector->drawingOnID,'" type="checkbox" value="',$serviceSelector->serverAddress,'" ',$enabledStr;
        if( $serviceSelector->drawingOn )
            echo ' checked ';
        echo '>Drawing<br>', "\n";

        echo '<input name= "',$serviceSelector->featureOnID,'" type="checkbox" value="',$serviceSelector->serverAddress,'" ',$enabledStr;
        if ( $serviceSelector->featureOn )
            echo ' checked ';
        echo '><a href="servicesproperties.php?Server=',$serviceSelector->serverAddress,'">Feature</a><br>', "\n";

        echo '<input name="',$serviceSelector->mappingOnID,'" type="checkbox" value="',$serviceSelector->serverAddress,'" ',$enabledStr;
        if ( $serviceSelector->mappingOn )
            echo 'checked ';
        echo '>Mapping<br>', "\n";

        echo '<input name="',$serviceSelector->renderingOnID,'" type="checkbox" value="',$serviceSelector->serverAddress,'" ',$enabledStr;
        if ( $serviceSelector->renderingOn )
            echo 'checked ';
        echo '>Rendering<br>', "\n";

        echo '<input name="',$serviceSelector->tileOnID,'" type="checkbox" value="',$serviceSelector->serverAddress,'" ',$enabledStr;
        if ( $serviceSelector->tileOn )
            echo 'checked ';
        echo '><a href="servicesproperties.php?Server=',$serviceSelector->serverAddress,'">Tile</a><br>', "\n";
    }

    class OnlineSelectorRecord
    {
        var $onlineID;
        var $onlineVal;
        var $enabled;

        function OnlineSelectorRecord()
        {
            $this->onlineID = "";
            $this->onlineVal = false;
            $this->enabled = true;
        }
    }

    function DisplayOnlineSelector( $onlineSelector )
    {
        if ( $onlineSelector == NULL )
            return;

        if ( $onlineSelector->onlineVal )
        {
            $onlineChecked = " checked ";
            $offlineChecked = "";
        }
        else
        {
            $onlineChecked = "";
            $offlineChecked = " checked ";
        }

        if ( $onlineSelector->enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '<input name="',$onlineSelector->onlineID,'" type="radio"  value="Online" ',$onlineChecked,$enabledStr,'>Online<br>', "\n";
        echo '<input name="',$onlineSelector->onlineID,'" type="radio"  value="Offline" ',$offlineChecked,$enabledStr,'>Offline', "\n";
    }

    function DisplayLogSelector( $selectedLogID, $selectedLog, $pageLink )
    {
        global $logLabels;

        foreach ( $logLabels as $key =>$label )
        {
            echo '  <td class="';
            if ( $key == $selectedLog )
            {
                echo 'selectedLogTab">';
                echo $label;
            }
            else
            {
                echo 'deselectedLogTab">';
                echo '<a href="'.$pageLink.'?'.$selectedLogID.'='.$key.'">';
                echo $label;
                echo '</a>';
            }
            echo '</td>',"\n";
        }

        echo '<td class="noLogTabs">&nbsp;</td>', "\n";
    }
    function DisplayLogSelectorWithActiveTab( $selectedLogID, $selectedLog, $pageLink )
    {
        global $logLabelsWithActiveLabel;

        CreateLogSelector( $logLabelsWithActiveLabel, $selectedLogID, $selectedLog, $pageLink );
    }

    function CreateLogSelector( $requiredLabels, $selectedLogID, $selectedLog, $pageLink )
    {
        foreach ( $requiredLabels as $key => $label )
        {
            echo '  <td class="';
            if ( $key == $selectedLog )
            {
                echo 'SelectedLogTab">';
                echo $label;
            }
            else
            {
                echo 'DeselectedLogTab">';
                echo '<a href="'.$pageLink.'?'.$selectedLogID.'='.$key.'">';
                echo $label;
                echo '</a>';
            }
            echo '</td>',"\n";
        }

        echo '<td class="NoLogTabs">&nbsp;</td>', "\n";
    }

    function GetLogSelection( $selectedLogID, &$selectedLog )
    {
        global $_GET;
        global $_POST;

        if ( array_key_exists( $selectedLogID, $_GET ) )
            $selectedLog = $_GET[ $selectedLogID ];
        else
        if ( array_key_exists( $selectedLogID, $_POST ) )
            $selectedLog = $_POST[ $selectedLogID ];
    }

    function DisplayEnabledBar( $label, $name, $checked, $enabled, $enableFields )
    {
        // Set checked String
        if ( $checked )
            $checkedStr = "checked";
        else
            $checkedStr = "";

        // Set Enabled String
        if ( $enabled )
            $enabledStr = "";
        else
            $enabledStr = "disabled";

        // Set OnClick string
        if ( $enableFields != null && !empty( $enableFields ) )
        {
            $onClickStr = ' onClick = "';
            $arrayStr = CreateJavaScriptArrayStr( 'fields', $enableFields );
            $onClickStr = $onClickStr.$arrayStr;
            $onClickStr = $onClickStr.' ChangeEnable( fields );"';
        }
        else
            $onClickStr = "";

        echo "<!-- Enabled Bar -->\n";
        echo "<br><br>\n";
        echo '<table width=100% border="0" cellspacing="0" cellpadding="4">',"\n";
        echo '  <tr>', "\n";
        echo '      <td class="PropertyBar" align="left">',"\n";
        echo '          ',$label, "\n";
        echo '      </td>',"\n";
        echo '      <td class="PropertyBar" align="right">',"\n";
        echo '          <input name="',$name,'" type="checkbox"',$checkedStr,' ',$enabledStr, $onClickStr,' >Enabled',"\n";
        echo '      </td>',"\n";
        echo '  </tr>',"\n";
        echo '</table>',"\n";
        echo "&nbsp;<br>\n";
    }

    define ( 'TABLE_PAGE_ID', "TablePageID" );

    function GetPageNumber( $items, $currItem, $pageSize )
    {
        global $_GET;
        global $_POST;

        $page = 1;

        if ( array_key_exists( TABLE_PAGE_ID, $_GET ) )
            $page = $_GET[ TABLE_PAGE_ID ];
        else
        if ( array_key_exists( TABLE_PAGE_ID, $_POST ) )
            $page = $_POST[ TABLE_PAGE_ID ];
        else
        if ( !empty( $currItem ) )
        {
            $itemIndex = array_search( $currItem, $items );
            if ( $itemIndex != NULL )
                $page = floor( $itemIndex / $pageSize ) + 1;
        }

        $numItems = sizeof( $items );
        $numPages = ceil( $numItems / $pageSize );
        if ( $page > $numPages )
            $page = 1;

        return $page;
    }

    define( 'SORT_DIRECTION_ID',    "Sort" );
    define( 'SORT_COLUMN_ID',       "SortColumn" );

    function DisplayPaginationControl( $currPage, $numItems, $pageSize, $pageLink, $sortColumn=NULL, $sortDirection=NULL )
    {
        if ( $numItems <= $pageSize )
            return;

        $pageCount = ceil( $numItems / $pageSize );
        echo '<input type="hidden" name="',TABLE_PAGE_ID,'" value="',$currPage,'" >',"\n";

        $currGroup = floor( ($currPage-1) / 10 );
        $first = ( $currGroup * 10 ) + 1;
        $last = ( $currGroup + 1 ) * 10;
        if ( $last > $pageCount )
            $last = $pageCount;

        $sortDirectionStr = ( $sortDirection == NULL ) ? "" : "&".SORT_DIRECTION_ID.'='.$sortDirection;
        $sortColumnStr = ( $sortColumn == NULL ) ? "" : "&".SORT_COLUMN_ID.'='.$sortColumn;

        echo '<div class="paginationControl" align="center">',"\n";

        if ( $currPage > 1 )
            echo '  <a href="',$pageLink,'?',TABLE_PAGE_ID,'=',$currPage-1,$sortDirectionStr,$sortColumnStr,'" >&lt;&lt;Prev</a>',"\n";

        for ( $i = $first; $i <= $last; $i++ )
        {
            if ( $i == $currPage )
                echo '  &nbsp;&nbsp;',$i, "\n";
            else
                echo '  &nbsp;&nbsp;<a href="',$pageLink,'?',TABLE_PAGE_ID,'=',$i,$sortDirectionStr,$sortColumnStr,'" >',$i,'</a>',"\n";
        }

        if ( $currPage < $pageCount )
            echo '  &nbsp;&nbsp;<a href="',$pageLink,'?',TABLE_PAGE_ID,'=',$currPage+1,$sortDirectionStr,$sortColumnStr,'" >Next>></a>',"\n";

        echo '</div>',"\n";
    }

    function GetPageRange( $currPage, $numItems, $pageSize, &$firstIndex, &$lastIndex )
    {
        if ( $pageSize == 0 )
        {
            $firstIndex = 0;
            $lastIndex = $numItems - 1;
        }
        else
        {
            $firstIndex = ( $currPage - 1 ) * $pageSize;
            $lastIndex = $firstIndex + $pageSize - 1;
            if ( $lastIndex > ($numItems - 1) )
                $lastIndex = $numItems - 1;
        }
    }

    define( 'DISPLAY_NAME_ID',                  "serverDisplayName" );
    define( 'LOCALE_ID',                        "serverLocale" );
    define( 'DEFAULT_MESSAGE_LOCALE_ID',        "serverDefaultMessageLocale" );
    define( 'LOCALIZATION_RESOURCES_PATH_ID',   "serverLocalizationResourcesPath" );
    define( 'TCP_IP_MTU_ID',                    "serverTcpIpMtu" );
    define( 'TEMP_PATH_ID',                     "serverTempPath" );
    define( 'FDO_PATH_ID',                      "serverFdoPath" );
    define( 'EMAIL_ID',                         "serverEmail" );
    define( 'CONNECTION_TIMEOUT_ID',            "connectionTimeOut" );
    define( 'CONNECTION_TIMER_INT_ID',          "connectionTimerInt" );

    function GetServerGeneralPropVals( $post, $props )
    {
        if ( array_key_exists( DISPLAY_NAME_ID, $post ) )
        {
            $props->displayName = $post[ DISPLAY_NAME_ID ];
            CleanUpMagicQuotes( $props->displayName );
        }
        if ( array_key_exists( LOCALE_ID, $post ) )
            $props->locale = $post[ LOCALE_ID ];
        if ( array_key_exists( DEFAULT_MESSAGE_LOCALE_ID, $post ) )
            $props->defaultMessageLocale = $post[ DEFAULT_MESSAGE_LOCALE_ID ];
        if ( array_key_exists( LOCALIZATION_RESOURCES_PATH_ID, $post ) )
        {
            $props->localizationResourcesPath = $post[ LOCALIZATION_RESOURCES_PATH_ID ];
            CleanUpMagicQuotes( $props->localizationResourcesPath );
        }
        if ( array_key_exists( TCP_IP_MTU_ID, $post ) )
            $props->tcpIpMtu = $post[ TCP_IP_MTU_ID ];
        if ( array_key_exists( TEMP_PATH_ID, $post ) )
        {
            $props->tempPath = $post[ TEMP_PATH_ID ];
            CleanUpMagicQuotes( $props->tempPath );
        }
        if ( array_key_exists( FDO_PATH_ID, $post ) )
        {
            $props->fdoPath = $post[ FDO_PATH_ID ];
            CleanUpMagicQuotes( $props->fdoPath );
        }
        if ( array_key_exists( EMAIL_ID, $post ) )
            $props->email = $post[ EMAIL_ID ];
        if ( array_key_exists( CONNECTION_TIMEOUT_ID, $post ) )
            $props->connectionTimeOut = $post[ CONNECTION_TIMEOUT_ID ];
        if ( array_key_exists( CONNECTION_TIMER_INT_ID, $post ) )
            $props->connectionTimerInt = $post[ CONNECTION_TIMER_INT_ID ];
    }

    function DisplayServerGeneralPropVals( $enabled, $props )
    {
        if ( $enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">General Properties</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Display name:</td>',"\n";
        echo '          <td  class="inputFormValue"><input class="inputFormValue" name="',DISPLAY_NAME_ID,'"',$enabledStr,' type="text" value="',$props->displayName,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Locale:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',LOCALE_ID,'"',$enabledStr,' type="text" value="',$props->locale,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Default message locale:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',DEFAULT_MESSAGE_LOCALE_ID,'"',$enabledStr,' type="text" value="',$props->defaultMessageLocale,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Localization resources path:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',LOCALIZATION_RESOURCES_PATH_ID,'"',$enabledStr,' type="text" value="',$props->localizationResourcesPath,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">TCP/IP maximum transmission unit:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',TCP_IP_MTU_ID,'"',$enabledStr,' type="text" value="',$props->tcpIpMtu,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Temporary file path:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',TEMP_PATH_ID,'"',$enabledStr,' type="text" value="',$props->tempPath,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">FDO file path:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FDO_PATH_ID,'"',$enabledStr,' type="text" value="',$props->fdoPath,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Administrator',"'",'s email address:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',EMAIL_ID,'"',$enabledStr,' type="text" value="',$props->email,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Connection timeout (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',CONNECTION_TIMEOUT_ID,'"',$enabledStr,' type="text" value="',$props->connectionTimeOut,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Connection timer interval (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',CONNECTION_TIMER_INT_ID,'"',$enabledStr,' type="text" value="',$props->connectionTimerInt,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }

    define( 'MAX_CONNS_ID',         "MaxConns" );
    define( 'PORT_ID',              "Port" );
    define( 'QUEUE_SIZE_ID',        "QueueSize" );
    define( 'THREAD_POOL_SIZE_ID',  "ThreadPoolSize" );

    function GetServerConnectionPropVals( $post, $props, $connStr )
    {
        if ( array_key_exists( $connStr.MAX_CONNS_ID, $post ) )
            $props->maxConns = $post[ $connStr.MAX_CONNS_ID ];
        if ( array_key_exists( $connStr.PORT_ID, $post ) )
            $props->port = $post[ $connStr.PORT_ID ];
        if ( array_key_exists( $connStr.QUEUE_SIZE_ID, $post ) )
            $props->queueSize = $post[ $connStr.QUEUE_SIZE_ID ];
        if ( array_key_exists( $connStr.THREAD_POOL_SIZE_ID, $post ) )
            $props->threadPoolSize = $post[ $connStr.THREAD_POOL_SIZE_ID ];
    }

    function DisplayServerConnectionPropVals( $enabled, $props, $connStr, $title )
    {
        if ( $enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">',$title,'</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum connections:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',$connStr.MAX_CONNS_ID,'"',$enabledStr,' type="text" value="',$props->maxConns,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Port:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',$connStr.PORT_ID,'"',$enabledStr,' type="text" value="',$props->port,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum queued operations:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',$connStr.QUEUE_SIZE_ID,'"',$enabledStr,' type="text" value="',$props->queueSize,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum threads in pool:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',$connStr.THREAD_POOL_SIZE_ID,'"',$enabledStr,' type="text" value="',$props->threadPoolSize,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }


    define( 'LOG_ENABLED_ID',               "Enabled" );
    define( 'LOG_FILENAME_ID',              "Filename" );
    define( 'LOG_PARMS_SELECTED_ID',        "ParmsSelected" );
    define( 'LOG_PARMS_NOT_SELECTED_ID',    "ParmsNotSelected" );

    function GetLogPropVals( $post, $props, $logStr )
    {
        if ( array_key_exists( $logStr.LOG_ENABLED_ID, $post ) )
            $props->enabled = true;
        else
            $props->enabled = false;
        if ( array_key_exists( $logStr.LOG_FILENAME_ID, $post ) )
        {
            $props->filename = $post[ $logStr.LOG_FILENAME_ID ];
            CleanUpMagicQuotes( $props->filename );
        }
        if ( array_key_exists( $logStr.LOG_PARMS_SELECTED_ID, $post ) )
            $props->parmsSelected = $post[ $logStr.LOG_PARMS_SELECTED_ID ];
        if ( array_key_exists( $logStr.LOG_PARMS_NOT_SELECTED_ID, $post) )
            $props->parmsNotSelected = $post[ $logStr.LOG_PARMS_NOT_SELECTED_ID ];
    }


    define( 'FEATURE_SERVICE_ENABLED_ID',           "featureServiceEnabled" );
    define( 'FEATURE_SERVICE_CONN_POOL_SIZE_ID',    "featureDataConnPoolSize" );
    define( 'FEATURE_SERVICE_CONN_POOL_SIZE_CUSTOM_ID', "featureDataConnPoolSizeCustom" );
    define( 'FEATURE_SERVICE_CONN_TIMEOUT_ID',      "featureDataConnTimeOut" );
    define( 'FEATURE_SERVICE_CONN_TIMER_INT_ID',    "featureConnectionTimerInteval" );
    define( 'FEATURE_SERVICE_DATA_CACHE_SIZE_ID',   "featureDataCacheSize" );

    function GetFeatureServicePropVals( $post, $props )
    {
        if ( array_key_exists( FEATURE_SERVICE_ENABLED_ID, $post ) )
            $props->enabled = '1';
        else
            $props->enabled = '0';
        if ( array_key_exists( FEATURE_SERVICE_CONN_POOL_SIZE_ID, $post ) )
            $props->dataConnPoolSize = $post[ FEATURE_SERVICE_CONN_POOL_SIZE_ID ];
        if ( array_key_exists( FEATURE_SERVICE_CONN_POOL_SIZE_CUSTOM_ID, $post ) )
            $props->dataConnPoolSizeCustom = $post[ FEATURE_SERVICE_CONN_POOL_SIZE_CUSTOM_ID ];
        if ( array_key_exists( FEATURE_SERVICE_CONN_TIMEOUT_ID, $post ) )
            $props->dataConnTimeOut = $post[ FEATURE_SERVICE_CONN_TIMEOUT_ID ];
        if ( array_key_exists( FEATURE_SERVICE_CONN_TIMER_INT_ID, $post ) )
            $props->dataConnTimerInt = $post[ FEATURE_SERVICE_CONN_TIMER_INT_ID ];
        if ( array_key_exists( FEATURE_SERVICE_DATA_CACHE_SIZE_ID, $post ) )
            $props->dataCacheSize = $post[ FEATURE_SERVICE_DATA_CACHE_SIZE_ID ];
    }


    function DisplayFeatureServicePropVals( $enabled, $props )
    {
        $sectionFields = array( FEATURE_SERVICE_CONN_POOL_SIZE_ID, FEATURE_SERVICE_CONN_POOL_SIZE_CUSTOM_ID, FEATURE_SERVICE_CONN_TIMEOUT_ID, FEATURE_SERVICE_CONN_TIMER_INT_ID, FEATURE_SERVICE_DATA_CACHE_SIZE_ID );
        DisplayEnabledBar( "Feature Service", FEATURE_SERVICE_ENABLED_ID, $props->enabled, false, $sectionFields );

        if ( $enabled && $props->enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <table  border="0" cellspacing="0" class="inputForm">',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum connections pooled per provider (default):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FEATURE_SERVICE_CONN_POOL_SIZE_ID,'"',$enabledStr,' type="text" value="',$props->dataConnPoolSize,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum connections pooled per provider (custom):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FEATURE_SERVICE_CONN_POOL_SIZE_CUSTOM_ID,'"',$enabledStr,' type="text" value="',$props->dataConnPoolSizeCustom,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Data connection timeout (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FEATURE_SERVICE_CONN_TIMEOUT_ID,'"',$enabledStr,' type="text" value="',$props->dataConnTimeOut,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Data connection timer interval (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FEATURE_SERVICE_CONN_TIMER_INT_ID,'"',$enabledStr,' type="text" value="',$props->dataConnTimerInt,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Maximum features cached:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',FEATURE_SERVICE_DATA_CACHE_SIZE_ID,'"',$enabledStr,' type="text" value="',$props->dataCacheSize,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }

    define( 'RESOURCE_SERVICE_DATA_FILE_TRASH_FOLDER_ID',                   "resourceDataFileTrashFolder" );
    define( 'RESOURCE_SERVICE_LIBRARY_DATA_FILE_FOLDER_ID',                 "resourceLibraryDataFileFolder" );
    define( 'RESOURCE_SERVICE_LIBRARY_REPOSITORY_FOLDER_ID',                "resourceLibraryRepositoryFolder" );
    define( 'RESOURCE_SERVICE_SESSION_DATA_FILE_FOLDER_ID',                 "resourceSessionDataFileFolder" );
    define( 'RESOURCE_SERVICE_SESSION_REPOSITORY_FOLDER_ID',                "resourceSessionRepositoryFolder" );
    define( 'RESOURCE_SERVICE_SITE_REPOSITORY_FOLDER_ID',                   "resourceSiteRepositoryFolder" );
    define( 'RESOURCE_SERVICE_RESOURCE_SCHEMA_FOLDER_ID',                   "resourceResourceSchemaFolder" );
    define( 'RESOURCE_SERVICE_REPOSITORY_CHECKPOINTS_TIMER_INTERVAL_ID',    "resourceResourceRepositoryCheckpointsTimerInterval" );
    define( 'RESOURCE_SERVICE_PACKAGES_PATH_ID',                            "resourceResourcePackagesPath" );

    function GetResourceServicePropVals( $post, $props )
    {
        $props->enabled = true;

        if ( array_key_exists( RESOURCE_SERVICE_DATA_FILE_TRASH_FOLDER_ID, $post ) )
        {
            $props->dataFileTrashFolder = $post[ RESOURCE_SERVICE_DATA_FILE_TRASH_FOLDER_ID ];
            CleanUpMagicQuotes( $props->dataFileTrashFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_LIBRARY_DATA_FILE_FOLDER_ID, $post ) )
        {
            $props->libraryDataFileFolder = $post[ RESOURCE_SERVICE_LIBRARY_DATA_FILE_FOLDER_ID ];
            CleanUpMagicQuotes( $props->libraryDataFileFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_LIBRARY_REPOSITORY_FOLDER_ID, $post ) )
        {
            $props->libraryRepositoryFolder = $post[ RESOURCE_SERVICE_LIBRARY_REPOSITORY_FOLDER_ID ];
            CleanUpMagicQuotes( $props->libraryRepositoryFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_SESSION_DATA_FILE_FOLDER_ID, $post ) )
        {
            $props->sessionDataFileFolder = $post[ RESOURCE_SERVICE_SESSION_DATA_FILE_FOLDER_ID ];
            CleanUpMagicQuotes( $props->sessionDataFileFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_SESSION_REPOSITORY_FOLDER_ID, $post ) )
        {
            $props->sessionRepositoryFolder = $post[ RESOURCE_SERVICE_SESSION_REPOSITORY_FOLDER_ID ];
            CleanUpMagicQuotes( $props->sessionRepositoryFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_SITE_REPOSITORY_FOLDER_ID, $post ) )
        {
            $props->siteRepositoryFolder = $post[ RESOURCE_SERVICE_SITE_REPOSITORY_FOLDER_ID ];
            CleanUpMagicQuotes( $props->siteRepositoryFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_RESOURCE_SCHEMA_FOLDER_ID, $post ) )
        {
            $props->resourceSchemaFolder = $post[ RESOURCE_SERVICE_RESOURCE_SCHEMA_FOLDER_ID ];
            CleanUpMagicQuotes( $props->resourceSchemaFolder );
        }
        if ( array_key_exists( RESOURCE_SERVICE_REPOSITORY_CHECKPOINTS_TIMER_INTERVAL_ID, $post ) )
        {
            $props->repositoryCheckpointsTimerInterval = $post[ RESOURCE_SERVICE_REPOSITORY_CHECKPOINTS_TIMER_INTERVAL_ID ];
        }
        if ( array_key_exists( RESOURCE_SERVICE_PACKAGES_PATH_ID, $post ) )
        {
            $props->packagesPath = $post[ RESOURCE_SERVICE_PACKAGES_PATH_ID ];
            CleanUpMagicQuotes( $props->packagesPath );
        }
    }

    function DisplayResourceServicePropVals( $enabled, $props )
    {
        DisplayEnabledBar( "Resource Service", "ResourceServiceEnabled", true, false, NULL );

        if ( $enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <table border="0" cellspacing="0" class="inputForm">',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Data file trash folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_DATA_FILE_TRASH_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->dataFileTrashFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Library data file folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_LIBRARY_DATA_FILE_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->libraryDataFileFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Library repository folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_LIBRARY_REPOSITORY_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->libraryRepositoryFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Session data file folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_SESSION_DATA_FILE_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->sessionDataFileFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Session repository folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_SESSION_REPOSITORY_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->sessionRepositoryFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Site repository folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_SITE_REPOSITORY_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->siteRepositoryFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Resource schema folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_RESOURCE_SCHEMA_FOLDER_ID,'"',$enabledStr,' type="text" value="',$props->resourceSchemaFolder,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Repository checkpoints timer interval (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_REPOSITORY_CHECKPOINTS_TIMER_INTERVAL_ID,'"',$enabledStr,' type="text" value="',$props->repositoryCheckpointsTimerInterval,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Packages folder:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',RESOURCE_SERVICE_PACKAGES_PATH_ID,'"',$enabledStr,' type="text" value="',$props->packagesPath,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }


    define( 'TILE_SERVICE_ENABLED_ID',                                 "tileServiceEnabled" );
    define( 'TILE_SERVICE_TILE_CACHE_PATH_ID',                         "tileTileCachePath" );

    function GetTileServicePropVals( $post, $props )
    {
        if ( array_key_exists( TILE_SERVICE_ENABLED_ID, $post ) )
            $props->enabled = true;
        else
            $props->enabled = false;
        if ( array_key_exists( TILE_SERVICE_TILE_CACHE_PATH_ID, $post ) )
        {
            $props->tileCachePath = $post[ TILE_SERVICE_TILE_CACHE_PATH_ID ];
            CleanUpMagicQuotes( $props->tileCachePath );
        }
    }

    function DisplayTileServicePropVals( $enabled, $props )
    {
        $sectionFields = array( TILE_SERVICE_TILE_CACHE_PATH_ID );
        DisplayEnabledBar( "Tile Service", TILE_SERVICE_ENABLED_ID, $props->enabled, false, $sectionFields );

        if ( $enabled && $props->enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <table border="0" cellspacing="0" class="inputForm">',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Tile cache path:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',TILE_SERVICE_TILE_CACHE_PATH_ID,'"',$enabledStr,' type="text" value="',$props->tileCachePath,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }


    define( 'SITE_SERVICE_SESSION_TIMEOUT_ID',                          "sessionTimeout" );
    define( 'SITE_SERVICE_SESSION_TIMER_INT_ID',                        "sessionTimerInt" );

    function GetSiteServicePropVals( $post, $props )
    {
        $props->enabled = true;
        if ( array_key_exists( SITE_SERVICE_SESSION_TIMEOUT_ID, $post ) )
            $props->sessionTimeout = $post[ SITE_SERVICE_SESSION_TIMEOUT_ID ];
        if ( array_key_exists( SITE_SERVICE_SESSION_TIMER_INT_ID, $post ) )
            $props->sessionTimerInt = $post[ SITE_SERVICE_SESSION_TIMER_INT_ID ];
    }

    function DisplaySiteServicePropVals( $enabled, $props )
    {
        DisplayEnabledBar( "Site Service", "SiteServiceEnabled", true, false, NULL );

        if ( $enabled )
            $enabledStr = "";
        else
            $enabledStr = " disabled ";

        echo '  <table border="0" cellspacing="0" class="inputForm">',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Session timeout (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',SITE_SERVICE_SESSION_TIMEOUT_ID,'"',$enabledStr,' type="text" value="',$props->sessionTimeout,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Session timer interval (seconds):</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',SITE_SERVICE_SESSION_TIMER_INT_ID,'"',$enabledStr,' type="text" value="',$props->sessionTimerInt,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }


    define( 'WMS_SERVICE_NAME_ID',                  "wmsName" );
    define( 'WMS_SERVICE_TITLE_ID',                 "wmsTitle" );
    define( 'WMS_SERVICE_ABSTRACT_ID',              "wmsAbstract" );
    define( 'WMS_SERVICE_FEES_ID',                  "wmsFees" );
    define( 'WMS_SERVICE_ACCESS_CONSTRAINTS_ID',    "wmsAccessConstraints" );
    define( 'WMS_SERVICE_KEYWORDS_ID',              "wmsKeywords" );
    define( 'WMS_SERVICE_SERVER_NAME_ID',           "wmsServerName" );
    define( 'WMS_SERVICE_SCRIPT_NAME_ID',           "wmsScriptName" );

    function GetWmsServicePropVals( $post, $props )
    {
        if ( array_key_exists( WMS_SERVICE_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_NAME_ITEM ] = $post[ WMS_SERVICE_NAME_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_TITLE_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_TITLE_ITEM ] = $post[ WMS_SERVICE_TITLE_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_ABSTRACT_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_ABSTRACT_ITEM ] = $post[ WMS_SERVICE_ABSTRACT_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_FEES_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_FEES_ITEM ] = $post[ WMS_SERVICE_FEES_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_ACCESS_CONSTRAINTS_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_ACCESS_CONSTRAINTS_ITEM ] = $post[ WMS_SERVICE_ACCESS_CONSTRAINTS_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_KEYWORDS_ID, $post ) )
        {
            $keywordsStr = $post[ WMS_SERVICE_KEYWORDS_ID ];
            $keyword = strtok( $keywordsStr, "," );
            while ( $keyword !== false )
            {
                $keyword = trim( $keyword );
                if ( !empty( $keyword ) )
                    $props->keywords[] = $keyword;
                $keyword = strtok(",");
            }
        }
        if ( array_key_exists( WMS_SERVICE_SERVER_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_SERVER_NAME_ITEM ] = $post[ WMS_SERVICE_SERVER_NAME_ID ];
            CleanUpMagicQuotes( $props->serviceMetadata[ WMS_SERVER_NAME_ITEM ] );
        }
        if ( array_key_exists( WMS_SERVICE_SCRIPT_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_SCRIPT_NAME_ITEM ] = $post[ WMS_SERVICE_SCRIPT_NAME_ID ];
            CleanUpMagicQuotes( $props->serviceMetadata[ WMS_SCRIPT_NAME_ITEM ] );
        }
    }

    function DisplayWmsServicePropVals( $props )
    {
        $keywordsStr = '';
        foreach ( $props->keywords as $keyword )
            $keywordsStr = empty( $keywordsStr ) ? $keyword : $keywordsStr.", ".$keyword;

        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">General Properties</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_NAME_ID,'" disabled type="text" value="',$props->serviceMetadata[ WMS_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Title:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_TITLE_ID,'" type="text" value="',$props->serviceMetadata[ WMS_TITLE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Abstract:</td>',"\n";
        echo '          <td class="inputFormValue"><textarea class="inputFormValue" name="',WMS_SERVICE_ABSTRACT_ID,'">',$props->serviceMetadata[ WMS_ABSTRACT_ITEM ],'</textarea>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Fees:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_FEES_ID,'" type="text" value="',$props->serviceMetadata[ WMS_FEES_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Access constraints:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_ACCESS_CONSTRAINTS_ID,'" type="text" value="',$props->serviceMetadata[ WMS_ACCESS_CONSTRAINTS_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Keywords:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_KEYWORDS_ID,'" type="text" value="',$keywordsStr,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Server name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_SERVER_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WMS_SERVER_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Script name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_SCRIPT_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WMS_SCRIPT_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }

    define( 'WMS_SERVICE_CONTACT_NAME_ID',                  "wmsContactName" );
    define( 'WMS_SERVICE_CONTACT_ORGANIZATION_ID',          "wmsContactOrganization" );
    define( 'WMS_SERVICE_CONTACT_POSITION_ID',              "wmsContactPosition" );
    define( 'WMS_SERVICE_CONTACT_ADDRESS_TYPE_ID',          "wmsContactAddressType" );
    define( 'WMS_SERVICE_CONTACT_STREET_ID',                "wmsContactStreet" );
    define( 'WMS_SERVICE_CONTACT_CITY_ID',                  "wmsContactCity" );
    define( 'WMS_SERVICE_CONTACT_STATE_PROV_ID',            "wmsContactStateProv" );
    define( 'WMS_SERVICE_CONTACT_POST_CODE_ID',             "wmsContactPostCode" );
    define( 'WMS_SERVICE_CONTACT_COUNTRY_ID',               "wmsContactCountry" );
    define( 'WMS_SERVICE_CONTACT_PHONE_ID',                 "wmsContactPhone" );
    define( 'WMS_SERVICE_CONTACT_FAX_ID',                   "wmsContactFax" );
    define( 'WMS_SERVICE_CONTACT_EMAIL_ID',                 "wmsContactEmail" );

    function GetWmsServiceContactPropVals( $post, $props )
    {
        if ( array_key_exists( WMS_SERVICE_CONTACT_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_NAME_ITEM ] = $post[ WMS_SERVICE_CONTACT_NAME_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_ORGANIZATION_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ORGANIZATION_ITEM ] = $post[ WMS_SERVICE_CONTACT_ORGANIZATION_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_POSITION_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_POSITION_ITEM ] = $post[ WMS_SERVICE_CONTACT_POSITION_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_ADDRESS_TYPE_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_TYPE_ITEM ] = $post[ WMS_SERVICE_CONTACT_ADDRESS_TYPE_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_STREET_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_STREET_ITEM ] = $post[ WMS_SERVICE_CONTACT_STREET_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_CITY_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_CITY_ITEM ] = $post[ WMS_SERVICE_CONTACT_CITY_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_STATE_PROV_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_STATE_PROV_ITEM ] = $post[ WMS_SERVICE_CONTACT_STATE_PROV_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_POST_CODE_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_POST_CODE_ITEM ] = $post[ WMS_SERVICE_CONTACT_POST_CODE_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_COUNTRY_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_ADDRESS_COUNTRY_ITEM ] = $post[ WMS_SERVICE_CONTACT_COUNTRY_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_PHONE_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_PHONE_ITEM ] = $post[ WMS_SERVICE_CONTACT_PHONE_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_FAX_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_FAX_ITEM ] = $post[ WMS_SERVICE_CONTACT_FAX_ID ];
        }
        if ( array_key_exists( WMS_SERVICE_CONTACT_EMAIL_ID, $post ) )
        {
            $props->serviceMetadata[ WMS_CONTACT_EMAIL_ITEM ] = $post[ WMS_SERVICE_CONTACT_EMAIL_ID ];
        }
    }

    function DisplayWmsServiceContactPropVals( $props )
    {
        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">Contact Properties</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Organization:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_ORGANIZATION_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ORGANIZATION_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Position:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_POSITION_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_POSITION_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2">&nbsp;</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Address type:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_ADDRESS_TYPE_ID,'" disabled type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_TYPE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Street:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_STREET_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_STREET_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">City:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_CITY_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_CITY_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">State/Province:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_STATE_PROV_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_STATE_PROV_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Post code:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_POST_CODE_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_POST_CODE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Country:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_COUNTRY_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_ADDRESS_COUNTRY_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2">&nbsp;</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Phone:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_PHONE_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_PHONE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Fax:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_FAX_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_FAX_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Email:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WMS_SERVICE_CONTACT_EMAIL_ID,'" type="text" value="',$props->serviceMetadata[ WMS_CONTACT_EMAIL_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }



    define( 'WFS_SERVICE_NAME_ID',                  "wfsName" );
    define( 'WFS_SERVICE_TITLE_ID',                 "wfsTitle" );
    define( 'WFS_SERVICE_ABSTRACT_ID',              "wfsAbstract" );
    define( 'WFS_SERVICE_FEES_ID',                  "wfsFees" );
    define( 'WFS_SERVICE_ACCESS_CONSTRAINTS_ID',    "wfsAccessConstraints" );
    define( 'WFS_SERVICE_KEYWORDS_ID',              "wfsKeywords" );
    define( 'WFS_SERVICE_SERVER_NAME_ID',           "wfsServerName" );
    define( 'WFS_SERVICE_SCRIPT_NAME_ID',           "wfsScriptName" );

    function GetWfsServicePropVals( $post, $props )
    {
        if ( array_key_exists( WFS_SERVICE_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_NAME_ITEM ] = $post[ WFS_SERVICE_NAME_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_TITLE_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_TITLE_ITEM ] = $post[ WFS_SERVICE_TITLE_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_ABSTRACT_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_ABSTRACT_ITEM ] = $post[ WFS_SERVICE_ABSTRACT_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_FEES_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_FEES_ITEM ] = $post[ WFS_SERVICE_FEES_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_ACCESS_CONSTRAINTS_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_ACCESS_CONSTRAINTS_ITEM ] = $post[ WFS_SERVICE_ACCESS_CONSTRAINTS_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_KEYWORDS_ID, $post ) )
        {
            $keywordsStr = $post[ WFS_SERVICE_KEYWORDS_ID ];
            $keyword = strtok( $keywordsStr, "," );
            while ( $keyword !== false )
            {
                $keyword = trim( $keyword );
                if ( !empty( $keyword ) )
                    $props->keywords[] = $keyword;
                $keyword = strtok(",");
            }
        }
        if ( array_key_exists( WFS_SERVICE_SERVER_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_SERVER_NAME_ITEM ] = $post[ WFS_SERVICE_SERVER_NAME_ID ];
            CleanUpMagicQuotes( $props->serviceMetadata[ WFS_SERVER_NAME_ITEM ] );
        }
        if ( array_key_exists( WFS_SERVICE_SCRIPT_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_SCRIPT_NAME_ITEM ] = $post[ WFS_SERVICE_SCRIPT_NAME_ID ];
            CleanUpMagicQuotes( $props->serviceMetadata[ WFS_SCRIPT_NAME_ITEM ] );
        }
    }

    function DisplayWfsServicePropVals( $props )
    {
        $keywordsStr = '';
        foreach ( $props->keywords as $keyword )
            $keywordsStr = empty( $keywordsStr ) ? $keyword : $keywordsStr.", ".$keyword;

        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">General Properties</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_NAME_ID,'" disabled type="text" value="',$props->serviceMetadata[ WFS_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Title:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_TITLE_ID,'" type="text" value="',$props->serviceMetadata[ WFS_TITLE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Abstract:</td>',"\n";
        echo '          <td class="inputFormValue"><textarea class="inputFormValue" name="',WFS_SERVICE_ABSTRACT_ID,'">',$props->serviceMetadata[ WFS_ABSTRACT_ITEM ],'</textarea>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Fees:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_FEES_ID,'" type="text" value="',$props->serviceMetadata[ WFS_FEES_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Access constraints:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_ACCESS_CONSTRAINTS_ID,'" type="text" value="',$props->serviceMetadata[ WFS_ACCESS_CONSTRAINTS_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Keywords:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_KEYWORDS_ID,'" type="text" value="',$keywordsStr,'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Server name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_SERVER_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WFS_SERVER_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Script name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_SCRIPT_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WFS_SCRIPT_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }

    define( 'WFS_SERVICE_CONTACT_NAME_ID',                  "wfsContactName" );
    define( 'WFS_SERVICE_CONTACT_ORGANIZATION_ID',          "wfsContactOrganization" );
    define( 'WFS_SERVICE_CONTACT_POSITION_ID',              "wfsContactPosition" );
    define( 'WFS_SERVICE_CONTACT_ADDRESS_TYPE_ID',          "wfsContactAddressType" );
    define( 'WFS_SERVICE_CONTACT_STREET_ID',                "wfsContactStreet" );
    define( 'WFS_SERVICE_CONTACT_CITY_ID',                  "wfsContactCity" );
    define( 'WFS_SERVICE_CONTACT_STATE_PROV_ID',            "wfsContactStateProv" );
    define( 'WFS_SERVICE_CONTACT_POST_CODE_ID',             "wfsContactPostCode" );
    define( 'WFS_SERVICE_CONTACT_COUNTRY_ID',               "wfsContactCountry" );
    define( 'WFS_SERVICE_CONTACT_PHONE_ID',                 "wfsContactPhone" );
    define( 'WFS_SERVICE_CONTACT_FAX_ID',                   "wfsContactFax" );
    define( 'WFS_SERVICE_CONTACT_EMAIL_ID',                 "wfsContactEmail" );


    function GetWfsServiceContactPropVals( $post, $props )
    {
        if ( array_key_exists( WFS_SERVICE_CONTACT_NAME_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_NAME_ITEM ] = $post[ WFS_SERVICE_CONTACT_NAME_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_ORGANIZATION_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ORGANIZATION_ITEM ] = $post[ WFS_SERVICE_CONTACT_ORGANIZATION_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_POSITION_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_POSITION_ITEM ] = $post[ WFS_SERVICE_CONTACT_POSITION_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_ADDRESS_TYPE_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_TYPE_ITEM ] = $post[ WFS_SERVICE_CONTACT_ADDRESS_TYPE_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_STREET_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_STREET_ITEM ] = $post[ WFS_SERVICE_CONTACT_STREET_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_CITY_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_CITY_ITEM ] = $post[ WFS_SERVICE_CONTACT_CITY_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_STATE_PROV_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_STATE_PROV_ITEM ] = $post[ WFS_SERVICE_CONTACT_STATE_PROV_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_POST_CODE_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_POST_CODE_ITEM ] = $post[ WFS_SERVICE_CONTACT_POST_CODE_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_COUNTRY_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_ADDRESS_COUNTRY_ITEM ] = $post[ WFS_SERVICE_CONTACT_COUNTRY_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_PHONE_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_PHONE_ITEM ] = $post[ WFS_SERVICE_CONTACT_PHONE_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_FAX_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_FAX_ITEM ] = $post[ WFS_SERVICE_CONTACT_FAX_ID ];
        }
        if ( array_key_exists( WFS_SERVICE_CONTACT_EMAIL_ID, $post ) )
        {
            $props->serviceMetadata[ WFS_CONTACT_EMAIL_ITEM ] = $post[ WFS_SERVICE_CONTACT_EMAIL_ID ];
        }
    }

    function DisplayWfsServiceContactPropVals( $props )
    {
        echo '  <br><br>',"\n";
        echo '  <table class="inputForm" border="0" cellspacing="0" cellpadding="4" >',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="PropertyBar" colspan="2">Contact Properties</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2"><br></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Name:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_NAME_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_NAME_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Organization:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_ORGANIZATION_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ORGANIZATION_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Position:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_POSITION_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_POSITION_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2">&nbsp;</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Address type:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_ADDRESS_TYPE_ID,'" disabled type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_TYPE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Street:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_STREET_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_STREET_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">City:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_CITY_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_CITY_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">State/Province:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_STATE_PROV_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_STATE_PROV_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Post code:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_POST_CODE_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_POST_CODE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Country:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_COUNTRY_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_ADDRESS_COUNTRY_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td colspan="2">&nbsp;</td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Phone:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_PHONE_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_PHONE_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Fax:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_FAX_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_FAX_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '      <tr>',"\n";
        echo '          <td class="propsInputLabel">Email:</td>',"\n";
        echo '          <td class="inputFormValue"><input class="inputFormValue" name="',WFS_SERVICE_CONTACT_EMAIL_ID,'" type="text" value="',$props->serviceMetadata[ WFS_CONTACT_EMAIL_ITEM ],'"></td>',"\n";
        echo '      </tr>',"\n";
        echo '  </table>',"\n";
    }


    ////// Utility Functions //////

    function CreateListOfItemsForTable( $items, &$numItems, &$itemListStr )
    {
        $numItems = sizeof( $items );
        $itemListStr = "";
        if ( $numItems > 0 )
            $itemListStr = $itemListStr." (".$items[0];
        if ( $numItems > 1 )
            $itemListStr = $itemListStr.', '.$items[1];
        if ( $numItems > 2 )
            $itemListStr = $itemListStr.", ...";
        if ( $numItems > 0 )
            $itemListStr = $itemListStr.")";
    }

    function CreateListOfRoleHyperlinksForTable( $item, &$itemListStr, $bUserTable, $currSelection )
    {
        $adminOn  = $item->adminOn  || ( $bUserTable && sizeof( $item->adminFromGroups  ) > 0 );
        $authorOn = $item->authorOn || ( $bUserTable && sizeof( $item->authorFromGroups ) > 0 );

        $rolesStr = "";
        if ( $adminOn )
            $rolesStr = $rolesStr.'Administrator';
        if ( $adminOn && $authorOn )
            $rolesStr = $rolesStr.', ';
        if ( $authorOn )
            $rolesStr = $rolesStr.'Author';

        $itemListStr = "";
        if ( !empty( $rolesStr ) )
        {
            if ( $bUserTable )
                $itemListStr = '<a href="userrolemanagement.php?SelectedUser='.$currSelection.'">';
            else
                $itemListStr = '<a href="grouprolemanagement.php?SelectedGroup='.$currSelection.'">';
            $itemListStr = $itemListStr.$rolesStr.'</a>';
        }
    }

    function CreateListOfUserHyperlinksForTable( $items, &$numItems, &$itemListStr )
    {
        $numItems = sizeof( $items );
        $itemListStr = "";

        //  show maximum ten users
        $maxItems = $numItems;
        if ( $maxItems > 10 )
            $maxItems = 10;

        if ( $maxItems > 0 )
        {
            $itemListStr = $itemListStr.' (<a href="edituser.php?SelectedUser='.$items[0].'">'.$items[0].'</a>';

            for ( $i = 1; $i < $maxItems; $i++ )
            {
                $itemListStr = $itemListStr.', <a href="edituser.php?SelectedUser='.$items[$i].'">'.$items[$i].'</a>';
            }

            //  show ellipses if there are more than ten users
            if ( $numItems > 10 )
                $itemListStr = $itemListStr.", ...";

            $itemListStr = $itemListStr.")";
        }
    }

    function CreateListOfGroupHyperlinksForTable( $items, &$numItems, &$itemListStr )
    {
        $numItems = sizeof( $items );
        $itemListStr = "";

        if ( $numItems > 0 )
        {
            $itemListStr = $itemListStr.' (';

            for ( $i = 0; $i < $numItems; $i++ )
            {
                if ( $i > 0 )
                    $itemListStr = $itemListStr.', ';
                if ( $items[$i] == MgGroup::Everyone )
                    $itemListStr = $itemListStr.$items[$i];
                else
                    $itemListStr = $itemListStr.'<a href="editgroup.php?SelectedGroup='.$items[$i].'">'.$items[$i].'</a>';
            }

            $itemListStr = $itemListStr.")";
        }
    }
    function CheckForFatalMgException( $exception )
    {
        if ( $exception instanceof MgAuthenticationFailedException ||
             $exception instanceof MgConnectionFailedException ||
             $exception instanceof MgConnectionNotOpenException ||
             $exception instanceof MgDuplicateSessionException ||
             $exception instanceof MgLicenseException ||
             $exception instanceof MgLicenseExpiredException ||
             $exception instanceof MgSessionExpiredException )
        {
            LogoutWithFatalException();
        }
    }

    function LogoutWithFatalException()
    {
        global $site;
        global $errFatalError;

        $site->Close();
        header( "Location: login.php?".ERROR_MSG_ID."=".$errFatalError );
        exit();
    }

    function RegisterPopup( $pageName, $registerSelf, $valuesToPersist )
    {
        $popupForm = 'popupRegistrationForm';
        $popupName = $registerSelf ? 'window.name' : 'parent.name';

        echo '<body onLoad="SetElementValue('."'".POPUP_NAME_ID."', ".$popupName." );SubmitForm('".$popupForm."');".'">'."\n";
            BeginContentArea( $popupForm );
            DeclareHiddenVars( $pageName );
            if ( $valuesToPersist != NULL )
                foreach ( $valuesToPersist as $id => $val )
                    echo '<input type="hidden" name="'.$id.'" value="'.$val.'">',"\n";
            EndContentArea( true, $popupForm, NULL );
        echo '</body>'."\n";

        exit();
    }

    define( 'HELP_PAGE_ID', "HelpPage" );
    function DisplayHelpButton( $helpPage )
    {
        if ( $helpPage == NULL )
            return;

        $onClickStr = "window.open( 'help.php?".HELP_PAGE_ID."=".$helpPage."', 'HelpWindow', 'width=800, height=600, scrollbars, resizable, menubar=yes, toolbar=yes' ); return false;";

        echo '      <td class="helpButton">',"\n";
        echo '          <a href="#" onClick="'.$onClickStr.'" >',"\n";
        echo '              <img class="helpButton" src="images/help.gif">&nbsp;Help',"\n";
        echo '          </a>',"\n";
        echo '      </td>',"\n";
    }

?>
