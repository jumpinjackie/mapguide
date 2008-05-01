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

    require_once 'xmlparser.php';
    require_once 'constants.php';

    ////// Utility functions //////
    function CopyArray( $sourceArray, &$destArray )
    {
        $emptyArray = array();
        $destArray = array_merge( $sourceArray, $emptyArray );
    }

    function CleanUpPath( &$path )
    {
        $path = str_replace('\\', '/', $path );
    }

    function CleanUpMagicQuotes( &$path )
    {
        if ( get_magic_quotes_gpc() )
            $path = str_replace('\\\\', '\\', $path );
    }

    define( 'ID_SORT_COLUMN',               'id' );
    define( 'NAME_SORT_COLUMN',             'name' );
    define( 'DEFAULT_SORT_COLUMN',          ID_SORT_COLUMN );

    define( 'ASCENDING_SORT_DIRECTION',     'a' );
    define( 'DESCENDING_SORT_DIRECTION',    'd' );
    define( 'DEFAULT_SORT_DIRECTION',       ASCENDING_SORT_DIRECTION );

    function key_cmp_a( $a, $b )
    {
        $temp = strnatcasecmp( $a, $b );
        return $temp;
    }

    function key_cmp_d( $a, $b )
    {
        $temp = strnatcasecmp( $b, $a );
        return $temp;
    }

    function name_cmp_a( $a, $b )
    {
        $temp = strnatcasecmp( $a->name, $b->name );
        return $temp;
    }

    function name_cmp_d( $a, $b )
    {
        $temp = strnatcasecmp( $b->name, $a->name );
        return $temp;
    }

    function SortTable( &$table, $sortColumn, $sortDirection )
    {
        if ( $sortColumn == NULL || $sortDirection == NULL )
            return;

        // uasort and uksort are used here because tables are associative arrays and these two sort functions will maintain
        // the key for each item in the list.
        if ( $sortDirection === ASCENDING_SORT_DIRECTION )
            if ( $sortColumn === NAME_SORT_COLUMN )
                uasort( $table, 'name_cmp_a');
            else
                uksort ( $table, 'key_cmp_a' );
        else
            if ( $sortColumn === NAME_SORT_COLUMN )
                uasort( $table, 'name_cmp_d' );
            else
                uksort ( $table, 'key_cmp_d' );
    }

    function GetClientIp()
    {
        $clientIp = array_key_exists( 'REMOTE_ADDR', $_SERVER ) ?  $_SERVER['REMOTE_ADDR'] : "";
        return $clientIp;
    }

    function GetClientAgent()
    {
        return "Map Admin";
    }

    ////// Users //////

    // User Table record type
    class UserTableRecord
    {
        var $name;
        var $description;
        var $groups;

        function UserTableRecord( $name, $description )
        {
            $this->name = $name;
            $this->description = $description;
            $this->groups = array();
        }
    }

    // User data is unicode data - use the appropriate parser
    function userRecParser( $text )
    {
        global $currKey;
        $currKey = "";

        // Process current element
        $name = getUnicodeValue( 'Name', $text );
        if ( empty( $name ) )
            return NULL;
        $fullName = getUnicodeValue( 'FullName', $text );
        $description = getUnicodeValue( 'Description', $text );

        $currKey = $name;
        return new UserTableRecord( $fullName, $description );
    }

    // Function which gets user data
    function GetUserDataFromDB( $targetGroup, $targetRole, &$targetTable )
    {
        global $site;
        global $userSortColumn;
        global $userSortDirection;

        // Call server to get comprehensive list of users.
        $targetTable = array();
        $users = $site->EnumerateUsers( $targetGroup, $targetRole, false );
        $buffer = "";
        $chunk = "";
        do {
            $chunkSize = $users->Read( $chunk, 4096 );
            $buffer = $buffer.$chunk;
        } while ( $chunkSize != 0 );

        // Parse XML in $buffer
        parseUnicodeData( $buffer, $targetTable, 'User', 'userRecParser' );

        SortTable( $targetTable, $userSortColumn, $userSortDirection );
    }

    $userSortDirection = DEFAULT_SORT_DIRECTION;
    function SetUserSortDirection( $sortDirection )
    {
        global $userSortDirection;

        $userSortDirection = $sortDirection;
    }

    $userSortColumm = NAME_SORT_COLUMN;
    function SetUserSortColumn( $sortColumn )
    {
        global $userSortColumn;

        $userSortColumn = $sortColumn;
    }

    $userData = NULL;
    function GetUserData()
    {
        global $userData;

        if ( $userData == NULL )
            GetUserDataFromDB( "", "", $userData );

        return $userData;
    }

    function GetUsers()
    {
        $users = GetUserData();
        return array_keys( $users );
    }

    function DeleteUser( $userID )
    {
        global $site;
        global $userData;

        // Validate
        $users = GetUserData();
        if ( empty( $userID ) || !array_key_exists( $userID, $users ) )
            return false;

        // Delete the user.
        $userToDelete = new MgStringCollection();
        $userToDelete->Add( $userID );
        $site->DeleteUsers( $userToDelete );

        // Update local userData array
        // Note that this is done rather than calling EnumerateUsers, again, because EnumerateUsers may take some time
        // to execute.
        $oldUserData = array();
        CopyArray( $userData, $oldUserData );
        $userData = array();
        foreach ( $oldUserData as $key => $val )
        {
            if ( $key != $userID )
                $userData[ $key ] = $val;
        }

        return true;
    }

    function LoadUserTable( &$targetTable, $firstIndex, $lastIndex, $includeGroups )
    {
        $targetTable = array();
        $users = GetUserData();

        $iUser = -1;
        foreach ( $users as $key => $val )
        {
            $iUser++;
            if ( $iUser < $firstIndex )
                continue;
            if ( $iUser > $lastIndex )
                break;

            $targetTable[ $key ] = $val;

            if ( $includeGroups )
                $val->groups = GetGroupMemberships( $key );
        }
    }

    function GetDataForUser( $userKey, $includeGroups )
    {
        $users = GetUserData();
        if ( !array_key_exists( $userKey, $users ) )
            return NULL;

        $userVal = $users[ $userKey ];
        if ( $includeGroups )
            $userVal->groups = GetGroupMemberships( $userKey );

        return $userVal;
    }

    function GetGroupMemberships( $userKey )
    {
        GetGroupDataFromDB( $userKey, "", $groupMemberships );
        $temp = array_keys ( $groupMemberships );

        // usort is used here because this is not an associative array and we need the keys to be reassigned; i.e. we want
        // $temp[0] to be the first item in the sorted list.
        usort( $temp, "strnatcasecmp" );

        return $temp;
    }

    ////// Groups //////

    // Group Table record type
    class GroupTableRecord
    {
        var $description;
        var $users;

        // Print contents
        function GroupTableRecord( $description )
        {
            $this->description = $description;
            $this->users = array();
        }
    }

    // Group data is unicode data - use the appropriate parser
    function groupRecParser( $text )
    {
        global $currKey;
        $currKey = "";

        // Process current element
        $name = getUnicodeValue( 'Name', $text );
        if ( empty( $name ) )
            return NULL;
        $description = getUnicodeValue( 'Description', $text );

        $currKey = $name;
        return new GroupTableRecord( $description );
    }

    $groupSortColumn = ID_SORT_COLUMN;
    $groupSortDirection = DEFAULT_SORT_DIRECTION;
    function SetGroupSortDirection( $sortDirection )
    {
        global $groupSortDirection;

        $groupSortDirection = $sortDirection;
    }

    function GetGroupDataFromDB( $targetUser, $targetRole, &$targetTable )
    {
        global $site;
        global $groupSortColumn;
        global $groupSortDirection;

        // Call server to get comprehensive list of users.
        $groups = $site->EnumerateGroups( $targetUser, $targetRole );
        $buffer = "";
        $chunk = "";
        do {
            $chunkSize = $groups->Read( $chunk, 4096 );
            $buffer = $buffer.$chunk;
        } while ( $chunkSize != 0 );

        // Parse XML in $buffer
        parseUnicodeData( $buffer, $targetTable, 'Group', 'groupRecParser' );

        SortTable( $targetTable, $groupSortColumn, $groupSortDirection );
    }

    $groupData = NULL;
    function GetGroupData()
    {
        global $groupData;

        if ( $groupData == NULL )
            GetGroupDataFromDB( "", "", $groupData );

        return $groupData;
    }

    function GetGroups()
    {
        $groups = GetGroupData();
        return array_keys( $groups );
    }

    function DeleteGroup( $groupName )
    {
        global $site;
        global $groupData;

        // Validate
        $groups = GetGroupData();
        if ( empty( $groupName ) || !array_key_exists( $groupName, $groups ) )
            return false;

        // Delete the group
        $groupToDelete = new MgStringCollection();
        $groupToDelete->Add( $groupName );
        $site->DeleteGroups( $groupToDelete );

        // Update local groupData array
        // Note that this is done rather than calling EnumerateGroups, again, because EnumerateGroups may take some time
        // to execute.
        $oldGroupData = array();
        CopyArray( $groupData, $oldGroupData );
        $groupData = array();
        foreach ( $oldGroupData as $key => $val )
        {
            if ( $key != $groupName )
                $groupData[ $key ] = $val;
        }

        return true;
    }

    function LoadGroupTable( &$targetTable, $firstIndex, $lastIndex, $includeUsers )
    {
        $targetTable = array();
        $groups = GetGroupData();

        $iGroup = -1;
        foreach ( $groups as $key => $val )
        {
            $iGroup++;
            if ( $iGroup < $firstIndex )
                continue;
            if ( $iGroup > $lastIndex )
                break;

            $targetTable[ $key ] = $val;

            if ( $includeUsers )
                $val->users = GetUserMembers( $key );
        }
    }

    function GetDataForGroup( $groupKey, $includeUsers )
    {
        $groups = GetGroupData();
        if ( !array_key_exists( $groupKey, $groups ) )
            return NULL;

        $groupVal = $groups[ $groupKey ];
        if ( $includeUsers )
            $groupVal->users = GetUserMembers( $groupKey );

        return $groupVal;
    }

    function GetUserMembers( $groupKey )
    {
        GetUserDataFromDB( $groupKey, "", $userMemberships );
        $temp = array_keys ( $userMemberships );

        // usort is used here because this is not an associative array and we need the keys to be reassigned; i.e. we want
        // $temp[0] to be the first item in the sorted list.
        usort( $temp, "strnatcasecmp" );

        return $temp;
    }

    ////// Roles //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Fixed roles for V1.0
    define( 'ADMIN_ROLE',  MgRole::Administrator );
    define( 'AUTHOR_ROLE', MgRole::Author );
    define( 'VIEWER_ROLE', MgRole::Viewer );

    // Function which builds a user table.
    function GetRoles( $targetUser, $targetGroup, &$targetList )
    {
        global $site;

        // Call server to get comprehensive list of roles.
        $roles = $site->EnumerateRoles( $targetUser, $targetGroup );

        // Build list
        $targetList = array();
        $numRoles = $roles->GetCount();
        for ( $i = 0; $i < $numRoles; $i++ )
        {
            $targetList[] = $roles->GetItem( $i );
        }
    }

    ////// UsersOrGroupsByRole //////////////////////////////////////////////////////////////////////////////////

    function GetUsersOrGroupsByRole( $bUser, $targetRole, &$targetList )
    {
        $targetTable = array();
        if ( $bUser )
            GetUserDataFromDB( "", $targetRole, $targetTable );
        else
            GetGroupDataFromDB( "", $targetRole, $targetTable );

        $targetList = array_keys( $targetTable );
    }

    ////// RolesTables ////////////////////////////////////////////////////////////////////////////////////////////////////

    // UserRoles Table record type
    class UserRolesTableRecord
    {
        var $name;
        var $adminOn;
        var $adminFromGroups;
        var $authorOn;
        var $authorFromGroups;
    }

    // Function which builds a userRoles table.
    function LoadUserRolesTable( &$targetTable, $firstIndex, $lastIndex )
    {
        GetUsersOrGroupsByRole( true, ADMIN_ROLE, $usersWithAdminRole );
        GetUsersOrGroupsByRole( false, ADMIN_ROLE, $groupsWithAdminRole );
        GetUsersOrGroupsByRole( true, AUTHOR_ROLE, $usersWithAuthorRole );
        GetUsersOrGroupsByRole( false, AUTHOR_ROLE, $groupsWithAuthorRole );

        $targetTable = array();
        LoadUserTable( $userTable, $firstIndex, $lastIndex, true );
        foreach ( $userTable as $userKey => $userVal )
        {
            // Create record for role data
            $userRolesRec = new UserRolesTableRecord();
            $targetTable[ $userKey ] = $userRolesRec;

            $userRolesRec->name = $userVal->name;

            if ( in_array( $userKey, $usersWithAdminRole ) )
                $userRolesRec->adminOn = true;
            else
                $userRolesRec->adminOn = false;

            $userRolesRec->adminFromGroups = array();
            $i = 0;
            foreach ( $userVal->groups as $groupKey )
            {
                if ( in_array( $groupKey, $groupsWithAdminRole ) )
                {
                    $userRolesRec->adminFromGroups[$i] = $groupKey;
                    $i++;
                }
            }

            if ( in_array( $userKey, $usersWithAuthorRole ) )
                $userRolesRec->authorOn = true;
            else
                $userRolesRec->authorOn = false;

            $userRolesRec->authorFromGroups = array();
            $i = 0;
            foreach ( $userVal->groups as $groupKey )
            {
                if ( in_array( $groupKey, $groupsWithAuthorRole ) )
                {
                    $userRolesRec->authorFromGroups[$i] = $groupKey;
                    $i++;
                }
            }
        }
    }

    // GroupRoles Table record type
    class GroupRolesTableRecord
    {
        var $adminOn;
        var $authorOn;
    }

    // Function which builds a userRoles table.
    function LoadGroupRolesTable( &$targetTable, $firstIndex, $lastIndex )
    {
        GetUsersOrGroupsByRole( false, ADMIN_ROLE, $groupsWithAdminRole );
        GetUsersOrGroupsByRole( false, AUTHOR_ROLE, $groupsWithAuthorRole );

        $targetTable = array();
        LoadGroupTable( $groupTable, $firstIndex, $lastIndex, false );
        foreach ( $groupTable as $groupKey => $groupVal )
        {
            // Create record for role data
            $groupRolesRec = new GroupRolesTableRecord();
            $targetTable[ $groupKey ] = $groupRolesRec;

            // For each role, record the group's role permission
            if ( in_array( $groupKey, $groupsWithAdminRole ) )
                $groupRolesRec->adminOn = true;
            else
                $groupRolesRec->adminOn = false;
            if ( in_array( $groupKey, $groupsWithAuthorRole ) )
                $groupRolesRec->authorOn = true;
            else
                $groupRolesRec->authorOn = false;
        }
    }

    ////// Servers //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Services
    define( 'CLIENT_SERVICE', "ClientService");
    define( 'DRAWING_SERVICE', "DrawingService");
    define( 'FEATURE_SERVICE', "FeatureService");
    define( 'MAPPING_SERVICE', "MappingService");
    define( 'RENDERING_SERVICE', "RenderingService");
    define( 'TILE_SERVICE', "TileService");
    define( 'RESOURCE_SERVICE', "ResourceService");
    define( 'SITE_SERVICE', "SiteService");

    $allPossibleSiteServerServices = array();
    $allPossibleSiteServerServices[0] = CLIENT_SERVICE;
    $allPossibleSiteServerServices[1] = DRAWING_SERVICE;
    $allPossibleSiteServerServices[2] = FEATURE_SERVICE;
    $allPossibleSiteServerServices[3] = MAPPING_SERVICE;
    $allPossibleSiteServerServices[4] = RENDERING_SERVICE;
    $allPossibleSiteServerServices[5] = RESOURCE_SERVICE;
    $allPossibleSiteServerServices[6] = SITE_SERVICE;
    $allPossibleSiteServerServices[7] = TILE_SERVICE;

    $allPossibleSupportServerServices = array();
    $allPossibleSupportServerServices[0] = MAPPING_SERVICE;
    $allPossibleSupportServerServices[1] = RENDERING_SERVICE;
    $allPossibleSupportServerServices[2] = TILE_SERVICE;

    // Server Table record type
    class ServerTableRecord
    {
        var $name;
        var $description;
        var $poweredUp;
        var $version;
        var $online;
        var $services;

        function ServerTableRecord( $name, $description )
        {
            $this->name = $name;
            $this->description = $description;
            $this->poweredUp = false;
            $this->version = "";
            $this->online = false;
            $this->services = array();
            $this->services[ DRAWING_SERVICE ] = false;
            $this->services[ FEATURE_SERVICE ] = false;
            $this->services[ MAPPING_SERVICE ] = false;
            $this->services[ RENDERING_SERVICE ] = false;
            $this->services[ TILE_SERVICE ] = false;
        }
    }

    // The server list is unicode data - use appropriate parser
    function serverRecParser( $text )
    {
        global $currKey;
        $currKey = "";

        // Process current element
        $ipAddress = getUnicodeValue( 'IpAddress', $text );
        if ( empty( $ipAddress ) )
            return NULL;
        $name = getUnicodeValue( 'Name', $text );
        $description = getUnicodeValue( 'Description', $text );

        $currKey = $ipAddress;
        return new ServerTableRecord( $name, $description );
    }

    function GetServerDataFromDB( &$targetTable )
    {
        global $site;
        global $userInfo;

        $targetTable =  array();

        // Put Site Server in $targetTable.
        $serverRec = new ServerTableRecord( "Site Server", "" );
        $serverKey = $site->GetCurrentSiteAddress();
        $targetTable[ $serverKey ] = $serverRec;

        $dbData = $site->EnumerateServers();
        $buffer = "";
        $chunk = "";
        do {
            $chunkSize = $dbData->Read( $chunk, 4096 );
            $buffer = $buffer.$chunk;
        } while ( $chunkSize != 0 );

        // Parse XML in $buffer
        parseUnicodeData( $buffer, $supportServers, 'Server', 'serverRecParser' );

        // Add support servers to target table.
        foreach( $supportServers as $key => $val )
            $targetTable[ $key ] = $val;

        // Get values for Online and Services fields
        foreach ( $targetTable as $key => $val )
        {
            try
            {
                $serverAdmin = new MgServerAdmin();
                $serverAdmin->Open( $key, $userInfo );
                $val->services = GetOptionalServerServices( $serverAdmin );
                $val->online = GetOnline( $serverAdmin );
                $val->version = GetVersion( $serverAdmin );
                $val->poweredUp = true;
            }
            catch ( MgException $e )
            {
                $val->poweredUp = false;
            }
            if ( $serverAdmin != NULL )
                $serverAdmin->Close();
        }
    }

    $serverData = NULL;
    function GetServerData()
    {
        global $serverData;

        if ( $serverData == NULL )
            GetServerDataFromDB( $serverData );

        return $serverData;
    }

    function ReloadServerData()
    {
        global $serverData;

        GetServerDataFromDB( $serverData );
    }

    function GetServers()
    {
        $servers = GetServerData();
        return array_keys( $servers );
    }

    function GetServerName( $serverAddress )
    {
        $name = "";

        $servers = GetServerData();
        if ( array_key_exists( $serverAddress, $servers ) )
            $name = $servers[ $serverAddress ]->name;

        return $name;
    }

    function GetServerCount()
    {
        $servers = GetServerData();
        return sizeof( $servers );
    }

    function DeleteServer( $serverAddress )
    {
        global $site;
        global $serverData;

        // Validate
        $servers = GetServerData();
        if ( empty( $serverAddress ) || !array_key_exists( $serverAddress, $servers ) )
            return false;

        // Delete the server.
        $serverRec = GetDataForServer( $serverAddress );
        if ( $serverRec == NULL )
            return false;
        $serverName = $serverRec->name;
        $site->RemoveServer( $serverName );

        // Update local serverData array
        // Note that this is done rather than calling EnumerateServers, again, because EnumerateServers may take some time
        // to execute.
        $oldServerData = array();
        CopyArray( $serverData, $oldServerData );
        $serverData = array();
        foreach ( $oldServerData as $key => $val )
        {
            if ( $key != $serverAddress )
                $serverData[ $key ] = $val;
        }

        return true;
    }

    function LoadServerTable( &$targetTable, $firstIndex, $lastIndex )
    {
        $targetTable = array();
        $servers = GetServerData();

        $iServer = -1;
        foreach ( $servers as $key => $val )
        {
            $iServer++;
            if ( $iServer < $firstIndex )
                continue;
            if ( $iServer > $lastIndex )
                break;

            $targetTable[ $key ] = $val;
        }
    }

    function GetDataForServer( $serverKey )
    {
        $servers = GetServerData();
        if ( !array_key_exists( $serverKey, $servers ) )
            return NULL;
        else
            return $servers[ $serverKey ];
    }

    function GetOptionalServerServices( $serverAdmin )
    {
        $services = array();
        $services[ DRAWING_SERVICE   ] = false;
        $services[ FEATURE_SERVICE   ] = false;
        $services[ MAPPING_SERVICE   ] = false;
        $services[ RENDERING_SERVICE ] = false;
        $services[ TILE_SERVICE      ] = false;

        $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::HostPropertiesSection );

        $prop = $props->GetItem( MgConfigProperties::HostPropertyDrawingService );
        if ( $prop->GetValue() == "1" )
            $services[ DRAWING_SERVICE ] = true;

        $prop = $props->GetItem( MgConfigProperties::HostPropertyFeatureService );
        if ( $prop->GetValue() == "1" )
            $services[ FEATURE_SERVICE ] = true;

        $prop = $props->GetItem( MgConfigProperties::HostPropertyMappingService );
        if ( $prop->GetValue() == "1" )
            $services[ MAPPING_SERVICE ] = true;

        $prop = $props->GetItem( MgConfigProperties::HostPropertyRenderingService );
        if ( $prop->GetValue() == "1" )
            $services[ RENDERING_SERVICE ] = true;

        $prop = $props->GetItem( MgConfigProperties::HostPropertyTileService );
        if ( $prop->GetValue() == "1" )
            $services[ TILE_SERVICE ] = true;

        return $services;
    }

    function SetOptionalServerServices( $serverAdmin, $services )
    {
        $props = new MgPropertyCollection();

         if ( $services[ DRAWING_SERVICE ] )
            $propVal = "1";
        else
            $propVal = "0";
        $prop = new MgStringProperty( MgConfigProperties::HostPropertyDrawingService, $propVal );
        $props->Add( $prop );

         if ( $services[ FEATURE_SERVICE ] )
            $propVal = "1";
        else
            $propVal = "0";
        $prop = new MgStringProperty( MgConfigProperties::HostPropertyFeatureService, $propVal );
        $props->Add( $prop );

         if ( $services[ MAPPING_SERVICE ] )
            $propVal = "1";
        else
            $propVal = "0";
        $prop = new MgStringProperty( MgConfigProperties::HostPropertyMappingService, $propVal );
        $props->Add( $prop );

         if ( $services[ RENDERING_SERVICE ] )
            $propVal = "1";
        else
            $propVal = "0";
        $prop = new MgStringProperty( MgConfigProperties::HostPropertyRenderingService, $propVal );
        $props->Add( $prop );

         if ( $services[ TILE_SERVICE ] )
            $propVal = "1";
        else
            $propVal = "0";
        $prop = new MgStringProperty( MgConfigProperties::HostPropertyTileService, $propVal );
        $props->Add( $prop );

        // Disable Service manipulation from Site Admin
        // $serverAdmin->SetConfigurationProperties( MgConfigProperties::HostPropertiesSection, $props );
    }

    function GetOnline( $serverAdmin )
    {
        return $serverAdmin->IsOnline();
    }

    function SetOnline( $serverAdmin, $online )
    {
        if ( $online )
            $serverAdmin->BringOnline();
        else
            $serverAdmin->TakeOffline();
    }

    function GetVersion( $serverAdmin )
    {
        $infoProps = $serverAdmin->GetInformationProperties();
        $versionProp = $infoProps->GetItem( MgServerInformationProperties::ServerVersion );
        return $versionProp->GetValue();
    }

    function GetUnmanagedDataMappings( $serverAdmin )
    {
        $mappings = array();
        $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::UnmanagedDataMappingsSection );

        for ( $i = 0; $i < $props->GetCount(); $i++)
        {
            $prop = $props->GetItem($i);
            $mappings[$prop->GetName()] = $prop->GetValue();
        }

        return $mappings;
    }

    // if $mappingName already exists, then that mapping is updated
    // if $mappingName does NOT exist, it is added
    function SetUnmanagedDataMapping( $serverAdmin, $mappingName, $location )
    {
        $props = new MgPropertyCollection();
        $prop = new MgStringProperty( $mappingName, $location );

        $props->Add( $prop );

        $serverAdmin->SetConfigurationProperties( MgConfigProperties::UnmanagedDataMappingsSection, $props );
    }

    function DeleteUnmanagedDataMapping( $serverAdmin, $mappingName )
    {
        $success = false;

        $props = new MgPropertyCollection();
        $prop = new MgStringProperty( $mappingName, "" ); // value doesn't matter
        $props->Add( $prop );

        $serverAdmin->RemoveConfigurationProperties( MgConfigProperties::UnmanagedDataMappingsSection, $props );
        $success = true;

        return $success;
    }

    function LoadMappingTable( &$targetTable, $table, $firstIndex, $lastIndex )
    {
        $targetTable = array();

        $iMapping = -1;
        foreach ( $table as $key => $val )
        {
            $iMapping++;
            if ( $iMapping < $firstIndex )
                continue;
            if ( $iMapping > $lastIndex )
                break;

            $targetTable[ $key ] = $val;
        }
    }


    ////// Properties //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GeneralPropsRecord
    {
        var $displayName;
        var $displayNamePropStr;
        var $locale;
        var $localePropStr;
        var $defaultMessageLocale;
        var $defaultMessageLocalePropStr;
        var $localizationResourcesPath;
        var $localizationResourcesPathPropStr;
        var $tcpIpMtu;
        var $tcpIpMtuPropStr;
        var $tempPath;
        var $tempPathPropStr;
        var $fdoPath;
        var $fdoPathPropStr;
        var $email;
        var $emailPropStr;
        var $connectionTimeOut;
        var $connectionTimeOutPropStr;
        var $connectionTimerInt;
        var $connectionTimerIntPropStr;

        function GeneralPropsRecord()
        {
            $this->displayName = "";
            $this->displayNamePropStr = MgConfigProperties::GeneralPropertyDisplayName;
            $this->locale = "";
            $this->localePropStr = MgConfigProperties::GeneralPropertyLocale;
            $this->defaultMessageLocale = "";
            $this->defaultMessageLocalePropStr = MgConfigProperties::GeneralPropertyDefaultMessageLocale;
            $this->localizationResourcesPath = "";
            $this->localizationResourcesPathPropStr = MgConfigProperties::GeneralPropertyResourcesPath;
            $this->tcpIpMtu = "";
            $this->tcpIpMtuPropStr = MgConfigProperties::GeneralPropertyTcpIpMtu;
            $this->tempPath = "";
            $this->tempPathPropStr = MgConfigProperties::GeneralPropertyTempPath;
            $this->fdoPath = "";
            $this->fdoPathPropStr = MgConfigProperties::GeneralPropertyFdoPath;
            $this->email = "";
            $this->emailPropStr = MgConfigProperties::AdministrativeConnectionPropertyEmail;
            $this->connectionTimeOut = 0;
            $this->connectionTimeOutPropStr = MgConfigProperties::GeneralPropertyConnectionTimeout;
            $this->connectionTimerInt = 0;
            $this->connectionTimerIntPropStr = MgConfigProperties::GeneralPropertyConnectionTimerInterval;
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::GeneralPropertiesSection );

            $prop = $props->GetItem( $this->displayNamePropStr );
            $this->displayName = $prop->GetValue();

            $prop = $props->GetItem( $this->localePropStr );
            $this->locale = $prop->GetValue();

            $prop = $props->GetItem( $this->defaultMessageLocalePropStr );
            $this->defaultMessageLocale = $prop->GetValue();

            $prop = $props->GetItem( $this->localizationResourcesPathPropStr );
            $this->localizationResourcesPath = $prop->GetValue();
            CleanUpPath( $this->localizationResourcesPath );

            $prop = $props->GetItem( $this->tcpIpMtuPropStr );
            $this->tcpIpMtu = $prop->GetValue();

            $prop = $props->GetItem( $this->tempPathPropStr );
            $this->tempPath = $prop->GetValue();
            CleanUpPath( $this->tempPath );

            $prop = $props->GetItem( $this->fdoPathPropStr );
            $this->fdoPath = $prop->GetValue();
            CleanUpPath( $this->fdoPath );

            $prop = $props->GetItem( $this->connectionTimeOutPropStr );
            $this->connectionTimeOut = $prop->GetValue();

            $prop = $props->GetItem( $this->connectionTimerIntPropStr );
            $this->connectionTimerInt = $prop->GetValue();

            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::AdministrativeConnectionPropertiesSection );
            $prop = $props->GetItem( $this->emailPropStr );
            $this->email = $prop->GetValue();
        }

        function ValidateProps()
        {
            global $errInvalidServerDefaultLocale;
            global $errInvalidServerLocalizationResourcesPath;
            global $errInvalidServerTcpIpMtu;
            global $errInvalidServerTempPath;
            global $errInvalidServerFdoPath;
            global $errInvalidConnectionTimeOut;
            global $errInvalidConnectionTimerInt;
            global $errInvalidServiceRegistrationTimerInt;

            if ( empty( $this->defaultMessageLocale ) )
                throw new Exception( $errInvalidServerDefaultLocale );
            if ( empty( $this->localizationResourcesPath ) )
                throw new Exception( $errInvalidServerLocalizationResourcesPath );
            if ( $this->tcpIpMtu < 0 )
                throw new Exception( $errInvalidServerTcpIpMtu );
            if ( empty( $this->tempPath ) )
                throw new Exception( $errInvalidServerTempPath );
            if ( empty( $this->fdoPath ) )
                throw new Exception( $errInvalidServerFdoPath );
            if ( $this->connectionTimeOut <= 0  )
                throw new Exception( $errInvalidConnectionTimeOut );
            if ( $this->connectionTimerInt < 0 )
                throw new Exception( $errInvalidConnectionTimerInt );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->displayNamePropStr , $this->displayName );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->localePropStr , $this->locale );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->defaultMessageLocalePropStr , $this->defaultMessageLocale );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->localizationResourcesPathPropStr, $this->localizationResourcesPath );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->tcpIpMtuPropStr, $this->tcpIpMtu );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->tempPathPropStr, $this->tempPath );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->fdoPathPropStr, $this->fdoPath );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->connectionTimeOutPropStr, $this->connectionTimeOut );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->connectionTimerIntPropStr, $this->connectionTimerInt );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( MgConfigProperties::GeneralPropertiesSection, $props );

            $props->Clear();
            $prop = new MgStringProperty( $this->emailPropStr, $this->email );
            $props->Add( $prop );
            $serverAdmin->SetConfigurationProperties( MgConfigProperties::AdministrativeConnectionPropertiesSection, $props );
        }
    }

    class ConnectionPropsRecord
    {
         var $connStr;
         var $propSectionStr;
         var $maxConns;
         var $maxConnsPropStr;
         var $port;
         var $portPropStr;
         var $queueSize;
         var $queueSizePropStr;
         var $threadPoolSize;
         var $threadPoolSizePropStr;

        function ConnectionPropsRecord( $connStr, $propSectionStr, $maxConnsPropStr, $portPropStr, $queueSizePropStr, $threadPoolSizePropStr )
        {
            $this->connStr = $connStr;
            $this->propSectionStr = $propSectionStr;
            $this->maxConns = 0;
            $this->maxConnsPropStr = $maxConnsPropStr;
            $this->port = 0;
            $this->portPropStr = $portPropStr;
            $this->queueSize = 0;
            $this->queueSizePropStr = $queueSizePropStr;
            $this->threadPoolSize = 0;
            $this->threadPoolSizePropStr = $threadPoolSizePropStr;
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( $this->propSectionStr );

            $prop = $props->GetItem( $this->maxConnsPropStr );
            $this->maxConns = $prop->GetValue();

            $prop = $props->GetItem( $this->portPropStr );
            $this->port = $prop->GetValue();

            $prop = $props->GetItem( $this->queueSizePropStr );
            $this->queueSize = $prop->GetValue();

            $prop = $props->GetItem( $this->threadPoolSizePropStr );
            $this->threadPoolSize = $prop->GetValue();
        }

        function ValidateProps()
        {
            global $errInvalidServerMaxConns;
            global $errInvalidServerConnQueueSize;
            global $errInvalidServerConnThreadPoolSize;

            if ( $this->maxConns < 1 )
                throw new Exception( sprintf( $errInvalidServerMaxConns, $this->connStr ) );
            if ( $this->queueSize < 1 )
                throw new Exception( sprintf( $errInvalidServerConnQueueSize, $this->connStr ) );
            if ( $this->threadPoolSize < 1 )
                throw new Exception( sprintf( $errInvalidServerConnThreadPoolSize, $this->connStr ) );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->maxConnsPropStr, $this->maxConns );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->portPropStr, $this->port );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->queueSizePropStr, $this->queueSize  );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->threadPoolSizePropStr, $this->threadPoolSize );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( $this->propSectionStr, $props );
        }
    }

    class FeatureServicePropsRecord
    {
        var $enabled;
        var $dataConnTimeOut;
        var $dataConnTimeOutPropStr;
        var $dataConnTimerInt;
        var $dataConnTimerIntPropStr;
        var $dataConnPoolSize;
        var $dataConnPoolSizePropStr;
        var $dataConnPoolSizeCustom;
        var $dataConnPoolSizeCustomPropStr;
        var $dataCacheSize;
        var $dataCacheSizePropStr;

        function FeatureServicePropsRecord()
        {
            $this->enabled = true;
            $this->enabledID = "featureServiceEnabled";
            $this->dataConnPoolSize = 1;
            $this->dataConnPoolSizePropStr = MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize;
            $this->dataConnPoolSizeID = "featureDataConnPoolSize";
            $this->dataConnPoolSizeCustom = "";
            $this->dataConnPoolSizeCustomPropStr = MgConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom;
            $this->dataConnPoolSizeCustomID = "featureDataConnPoolSizeCustom";
            $this->dataConnTimeOut = 0;
            $this->dataConnTimeOutPropStr = MgConfigProperties::FeatureServicePropertyDataConnectionTimeout;
            $this->dataConnTimeOutID = "featureDataConnTimeOut";
            $this->dataConnTimerInt = 0;
            $this->dataConnTimerIntPropStr = MgConfigProperties::FeatureServicePropertyDataConnectionTimerInterval;
            $this->dataConnTimerIntID = "featureDataConnTimerInt";
            $this->dataCacheSize = 0;
            $this->dataCacheSizePropStr = MgConfigProperties::FeatureServicePropertyDataCacheSize;
            $this->dataCacheSizeID = "featureDataConnCacheSize";
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::FeatureServicePropertiesSection );

            $prop = $props->GetItem( $this->dataConnPoolSizePropStr );
            $this->dataConnPoolSize = $prop->GetValue();

            $prop = $props->GetItem( $this->dataConnPoolSizeCustomPropStr );
            $this->dataConnPoolSizeCustom = $prop->GetValue();

            $prop = $props->GetItem( $this->dataConnTimeOutPropStr );
            $this->dataConnTimeOut = $prop->GetValue();

            $prop = $props->GetItem( $this->dataConnTimerIntPropStr );
            $this->dataConnTimerInt = $prop->GetValue();

            $prop = $props->GetItem( $this->dataCacheSizePropStr );
            $this->dataCacheSize = $prop->GetValue();
        }

        function ValidateProps()
        {
            global $errInvalidFeatureDataConnPoolSize;
            global $errInvalidFeatureDataConnPoolSizeCustom;
            global $errInvalidFeatureDataConnTimeOut;
            global $errInvalidFeatureDataConnTimerInt;
            global $errInvalidFeatureDataCacheSize;

            if ( $this->dataConnPoolSize <= 0 )
                throw new Exception( $errInvalidFeatureDataConnPoolSize );
            if ( $this->dataConnTimeOut <= 0 )
                throw new Exception( $errInvalidFeatureDataConnTimeOut );
            if ( $this->dataConnTimerInt < 0 )
                throw new Exception( $errInvalidFeatureDataConnTimerInt );
            if ( $this->dataCacheSize < 1 )
                throw new Exception( $errInvalidFeatureDataCacheSize );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->dataConnPoolSizePropStr, $this->dataConnPoolSize );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->dataConnPoolSizeCustomPropStr, $this->dataConnPoolSizeCustom );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->dataConnTimeOutPropStr, $this->dataConnTimeOut );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->dataConnTimerIntPropStr, $this->dataConnTimerInt );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->dataCacheSizePropStr, $this->dataCacheSize );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( MgConfigProperties::FeatureServicePropertiesSection, $props );
        }
    }

    class ResourceServicePropsRecord
    {
        var $enabled;
        var $dataFileTrashFolder;
        var $dataFileTrashFolderPropStr;
        var $libraryDataFileFolder;
        var $libraryDataFileFolderPropStr;
        var $libraryRepositoryFolder;
        var $libraryRepositoryFolderPropStr;
        var $sessionDataFileFolder;
        var $sessionDataFileFolderPropStr;
        var $sessionRepositoryFolder;
        var $sessionRepositoryFolderPropStr;
        var $siteRepositoryFolder;
        var $siteRepositoryFolderPropStr;
        var $resourceSchemaFolder;
        var $resourceSchemaFolderPropStr;
        var $repositoryCheckpointsTimerInterval;
        var $repositoryCheckpointsTimerIntervalPropStr;
        var $packagesPath;
        var $packagesPathPropStr;


        function ResourceServicePropsRecord()
        {
            $this->enabled = true;
            $this->dataFileTrashFolder = "";
            $this->dataFileTrashFolderPropStr = MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName;
            $this->libraryDataFileFolder = "";
            $this->libraryDataFileFolderPropStr = MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath;
            $this->libraryRepositoryFolder = "";
            $this->libraryRepositoryFolderPropStr = MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath;
            $this->sessionDataFileFolder = "";
            $this->sessionDataFileFolderPropStr = MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath;
            $this->sessionRepositoryFolder = "";
            $this->sessionRepositoryFolderPropStr = MgConfigProperties::ResourceServicePropertySessionRepositoryPath;
            $this->siteRepositoryFolder = "";
            $this->siteRepositoryFolderPropStr = MgConfigProperties::ResourceServicePropertySiteRepositoryPath;
            $this->resourceSchemaFolder = "";
            $this->resourceSchemaFolderPropStr = MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath;
            $this->repositoryCheckpointsTimerInterval = 0;
            $this->repositoryCheckpointsTimerIntervalPropStr = MgConfigProperties::ResourceServicePropertyRepositoryCheckpointsTimerInterval;
            $this->packagesPath = "";
            $this->packagesPathPropStr = MgConfigProperties::ResourceServicePropertyPackagesPath;
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::ResourceServicePropertiesSection );

            $prop = $props->GetItem( $this->dataFileTrashFolderPropStr );
            $this->dataFileTrashFolder = $prop->GetValue();
            CleanUpPath( $this->dataFileTrashFolder );

            $prop = $props->GetItem( $this->libraryDataFileFolderPropStr );
            $this->libraryDataFileFolder = $prop->GetValue();
            CleanUpPath( $this->libraryDataFileFolder );

            $prop = $props->GetItem( $this->libraryRepositoryFolderPropStr );
            $this->libraryRepositoryFolder = $prop->GetValue();
            CleanUpPath( $this->libraryRepositoryFolder );

            $prop = $props->GetItem( $this->sessionDataFileFolderPropStr );
            $this->sessionDataFileFolder = $prop->GetValue();
            CleanUpPath( $this->sessionDataFileFolder );

            $prop = $props->GetItem( $this->sessionRepositoryFolderPropStr );
            $this->sessionRepositoryFolder = $prop->GetValue();
            CleanUpPath( $this->sessionRepositoryFolder );

            $prop = $props->GetItem( $this->siteRepositoryFolderPropStr );
            $this->siteRepositoryFolder = $prop->GetValue();
            CleanUpPath( $this->siteRepositoryFolder );

            $prop = $props->GetItem( $this->resourceSchemaFolderPropStr );
            $this->resourceSchemaFolder = $prop->GetValue();
            CleanUpPath( $this->resourceSchemaFolder );

            $prop = $props->GetItem( $this->repositoryCheckpointsTimerIntervalPropStr );
            $this->repositoryCheckpointsTimerInterval = $prop->GetValue();

            $prop = $props->GetItem( $this->packagesPathPropStr );
            $this->packagesPath = $prop->GetValue();
            CleanUpPath( $this->packagesPath );

            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::GeneralPropertiesSection );

        }

        function ValidateProps()
        {
            global $errInvalidResourceDataFileTrashFolder;
            global $errInvalidResourceLibraryDataFileFolder;
            global $errInvalidResourceLibraryRepositoryFolder;
            global $errInvalidResourceSessionDataFileFolder;
            global $errInvalidResourceSessionRepositoryFolder;
            global $errInvalidResourceSiteRepositoryFolder;
            global $errInvalidResourceResourceSchemaFolder;
            global $errInvalidRepositoryCheckpointsTimerInterval;
            global $errInvalidPackagesPath;

            if ( empty( $this->dataFileTrashFolder ) )
                throw new Exception( $errInvalidResourceDataFileTrashFolder );
            if ( empty ($this->libraryDataFileFolder) )
                throw new Exception( $errInvalidResourceLibraryDataFileFolder );
            if ( empty ($this->libraryRepositoryFolder) )
                throw new Exception( $errInvalidResourceLibraryRepositoryFolder );
            if ( empty( $this->sessionDataFileFolder ) )
                throw new Exception( $errInvalidResourceSessionDataFileFolder );
            if ( empty( $this->sessionRepositoryFolder ) )
                throw new Exception( $errInvalidResourceSessionRepositoryFolder );
            if ( empty( $this->siteRepositoryFolder ) )
                throw new Exception( $errInvalidResourceSiteRepositoryFolder );
            if ( empty( $this->resourceSchemaFolder ) )
                throw new Exception( $errInvalidResourceResourceSchemaFolder );
            if ( $this->repositoryCheckpointsTimerInterval < 0 )
                throw new Exception( $errInvalidRepositoryCheckpointsTimerInterval );
            if ( empty( $this->packagesPath ) )
                throw new Exception( $errInvalidPackagesPath );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->dataFileTrashFolderPropStr, $this->dataFileTrashFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->libraryDataFileFolderPropStr, $this->libraryDataFileFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->libraryRepositoryFolderPropStr, $this->libraryRepositoryFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->sessionDataFileFolderPropStr, $this->sessionDataFileFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->sessionRepositoryFolderPropStr, $this->sessionRepositoryFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->siteRepositoryFolderPropStr, $this->siteRepositoryFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->resourceSchemaFolderPropStr, $this->resourceSchemaFolder );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->repositoryCheckpointsTimerIntervalPropStr, $this->repositoryCheckpointsTimerInterval );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->packagesPathPropStr, $this->packagesPath );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( MgConfigProperties::ResourceServicePropertiesSection, $props );

        }

    }


    class TileServicePropsRecord
    {
        var $enabled;
        var $tileCachePath;
        var $tileCachePathPropStr;

        function TileServicePropsRecord()
        {
            $this->enabled = true;
            $this->tileCachePath = "";
            $this->tileCachePathPropStr = MgConfigProperties::TileServicePropertyTileCachePath;
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::TileServicePropertiesSection );

            $prop = $props->GetItem( $this->tileCachePathPropStr );
            $this->tileCachePath = $prop->GetValue();
            CleanUpPath( $this->tileCachePath );
        }

        function ValidateProps()
        {
            global $errInvalidTileCachePath;

            if ( empty( $this->tileCachePath ) )
                throw new Exception( $errInvalidTileCachePath );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->tileCachePathPropStr, $this->tileCachePath );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( MgConfigProperties::TileServicePropertiesSection, $props );
        }
    }



    class SiteServicePropsRecord
    {
        var $enabled;
        var $sessionTimeout;
        var $sessionTimeoutPropStr;
        var $sessionTimerInt;
        var $sessionTimerIntPropStr;

        function SiteServicePropsRecord()
        {
            $this->enabled = true;
            $this->sessionTimeout = 0;
            $this->sessionTimeoutPropStr = MgConfigProperties::SiteServicePropertySessionTimeout;
            $this->sessionTimerInt = 0;
            $this->sessionTimerIntPropStr = MgConfigProperties::SiteServicePropertySessionTimerInterval;
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( MgConfigProperties::SiteServicePropertiesSection );

            $prop = $props->GetItem( $this->sessionTimeoutPropStr );
            $this->sessionTimeout = $prop->GetValue();

            $prop = $props->GetItem( $this->sessionTimerIntPropStr );
            $this->sessionTimerInt = $prop->GetValue();
        }

        function ValidateProps()
        {
            global $errInvalidSessionTimeOut;
            global $errInvalidSessionTimerInt;

            if ( $this->sessionTimeout <= 0 )
                throw new Exception( $errInvalidSessionTimeOut );
            if ( $this->sessionTimerInt <= 0 )
                throw new Exception( $errInvalidSessionTimerInt );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

            $prop = new MgStringProperty( $this->sessionTimeoutPropStr, $this->sessionTimeout );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->sessionTimerIntPropStr, $this->sessionTimerInt );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( MgConfigProperties::SiteServicePropertiesSection, $props );
        }
    }


    ////// Logs //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Logs
    define( 'ACTIVE_LOGS',            "ActiveLogs" );
    define( 'ACCESS_LOG',             MgLogFileType::Access );
    define( 'ADMIN_LOG',             MgLogFileType::Admin );
    define( 'AUTHENTICATION_LOG',      MgLogFileType::Authentication );
    define( 'ERROR_LOG',             MgLogFileType::Error );
    define( 'SESSION_LOG',             MgLogFileType::Session );
    define( 'TRACE_LOG',             MgLogFileType::Trace );

    define( 'COMMA_DELIMITER',        "Comma" );
    define( 'TAB_DELIMITER',        "Tab" );
    define( 'SPACE_DELIMITER',         "Space" );
    define( 'NO_DELIMITER',            "None" );
    $logDelimiters = array();
    $logDelimiters[ COMMA_DELIMITER ] = ',';
    $logDelimiters[ TAB_DELIMITER ] = "\\t";
    $logDelimiters[ SPACE_DELIMITER ] = " ";
    $logDelimiters[ NO_DELIMITER ] = "";

    define( 'ARCHIVE_NONE',         "None" );
    define( 'ARCHIVE_DAILY',        "Day" );
    define( 'ARCHIVE_MONTHLY',        "Month" );
    define( 'ARCHIVE_YEARLY',        "Year" );
    $archiveFrequencies[ ARCHIVE_NONE    ] = "";
    $archiveFrequencies[ ARCHIVE_DAILY   ] = "%d";
    $archiveFrequencies[ ARCHIVE_MONTHLY ] = "%m";
    $archiveFrequencies[ ARCHIVE_YEARLY  ] = "%y";

    $allLogs = array();
    $allLogs[0] = ACCESS_LOG;
    $allLogs[1] = ADMIN_LOG;
    $allLogs[2] = AUTHENTICATION_LOG;
    $allLogs[3] = ERROR_LOG;
    $allLogs[4] = SESSION_LOG;
    $allLogs[5] = TRACE_LOG;

    define( 'AVERAGEOPTIME_PARM', "AVERAGEOPTIME" );
    define( 'CLIENT_PARM', "CLIENT" );
    define( 'CLIENTIP_PARM', "CLIENTIP" );
    define( 'DURATION_PARM', "DURATION" );
    define( 'ENDTIME_PARM', "ENDTIME" );
    define( 'ERROR_PARM', "ERROR" );
    define( 'INFO_PARM', "INFO" );
    define( 'OPID_PARM', "OPID" );
    define( 'OPSFAILED_PARM', "OPSFAILED" );
    define( 'OPSPROCESSED_PARM', "OPSPROCESSED" );
    define( 'OPSRECEIVED_PARM', "OPSRECEIVED" );
    define( 'STACKTRACE_PARM', "STACKTRACE" );
    define( 'STARTTIME_PARM', "STARTTIME" );
    define( 'USER_PARM', "USER" );

    $allLogParms = array();
    $allLogParms[0]  = AVERAGEOPTIME_PARM;
    $allLogParms[1]  = CLIENT_PARM;
    $allLogParms[2]  = CLIENTIP_PARM;
    $allLogParms[3]  = DURATION_PARM;
    $allLogParms[4]  = ENDTIME_PARM;
    $allLogParms[5]  = ERROR_PARM;
    $allLogParms[6]  = INFO_PARM;
    $allLogParms[7]  = OPID_PARM;
    $allLogParms[8]  = OPSFAILED_PARM;
    $allLogParms[9]  = OPSPROCESSED_PARM;
    $allLogParms[10] = OPSRECEIVED_PARM;
    $allLogParms[11] = STACKTRACE_PARM;
    $allLogParms[12] = STARTTIME_PARM;
    $allLogParms[13] = USER_PARM;

    $allAccessLogParms = array();
    $allAccessLogParms[0] = CLIENT_PARM;
    $allAccessLogParms[1] = CLIENTIP_PARM;
    $allAccessLogParms[2] = OPID_PARM;
    $allAccessLogParms[3] = USER_PARM;

    $allAdminLogParms = array();
    $allAdminLogParms[0] = CLIENT_PARM;
    $allAdminLogParms[1] = CLIENTIP_PARM;
    $allAdminLogParms[2] = OPID_PARM;
    $allAdminLogParms[3] = USER_PARM;

    $allAuthenticationLogParms = array();
    $allAuthenticationLogParms[0] = CLIENT_PARM;
    $allAuthenticationLogParms[1] = CLIENTIP_PARM;
    $allAuthenticationLogParms[2] = USER_PARM;

    $allErrorLogParms = array();
    $allErrorLogParms[0] = CLIENT_PARM;
    $allErrorLogParms[1] = CLIENTIP_PARM;
    $allErrorLogParms[2] = ERROR_PARM;
    $allErrorLogParms[3] = STACKTRACE_PARM;
    $allErrorLogParms[4] = USER_PARM;

    $allSessionLogParms = array();
    $allSessionLogParms[0] = CLIENT_PARM;
    $allSessionLogParms[1] = CLIENTIP_PARM;
    $allSessionLogParms[2] = USER_PARM;
    $allSessionLogParms[3] = STARTTIME_PARM;
    $allSessionLogParms[4] = ENDTIME_PARM;
    $allSessionLogParms[5] = OPSFAILED_PARM;
    $allSessionLogParms[6] = OPSRECEIVED_PARM;
    $allSessionLogParms[7] = AVERAGEOPTIME_PARM;

    $allTraceLogParms = array();
    $allTraceLogParms[0] = INFO_PARM;

    class LogPropsRecord
    {
        var $enabled;
        var $filename;
        var $allPossibleParms;
        var $parmsSelected;
        var $parmsNotSelected;

        var $propSectionStr;
        var $enabledPropStr;
        var $filenamePropStr;
        var $parmsPropStr;

         function LogPropsRecord( $propSectionStr, $enabledPropStr, $filenamePropStr, $parmsPropStr, $allPossibleParms )
        {
            $this->propSectionStr = $propSectionStr;
            $this->enabledPropStr = $enabledPropStr;
            $this->filenamePropStr = $filenamePropStr;
            $this->parmsPropStr = $parmsPropStr;

            $this->enabled = false;
            $this->filename = "";
            $this->parmsSelected = array();
            $this->parmsNotSelected = array();
            CopyArray( $allPossibleParms, $this->allPossibleParms );
        }

        function GetProps( $serverAdmin )
        {
            $props = $serverAdmin->GetConfigurationProperties( $this->propSectionStr );

            $prop = $props->GetItem( $this->enabledPropStr );
            if ( $prop->GetValue() == "1" )
                $this->enabled = true;
            else
                $this->enabled = false;

            $prop = $props->GetItem( $this->filenamePropStr );
            $this->filename = $prop->GetValue();
            CleanUpPath( $this->filename );

            $prop = $props->GetItem( $this->parmsPropStr );
            $parmsStr = $prop->GetValue();
            $this->parmsSelected = array();
            $i = 0;
            $parm = strtok( $parmsStr, "," );
            while ( $parm !== false )
            {
                $parm = trim( $parm );
                $this->parmsSelected[$i] = $parm;
                $i++;
                $parm = strtok(",");
            }
            $this->parmsNotSelected = array_diff( $this->allPossibleParms, $this->parmsSelected );
        }

        function ValidateProps()
        {
            global $errInvalidLogFilename;

            if ( $this->enabled && empty( $this->filename ) )
                throw new Exception( $errInvalidLogFilename );
        }

        function SetProps( $serverAdmin )
        {
            $props = new MgPropertyCollection();

             if ( $this->enabled )
                $propVal = "1";
            else
                $propVal = "0";
            $prop = new MgStringProperty( $this->enabledPropStr, $propVal );
            $props->Add( $prop );

            $prop = new MgStringProperty( $this->filenamePropStr, $this->filename );
            $props->Add( $prop );

            $separator = "";
            $parmStr = "";
            foreach ( $this->parmsSelected as $parm )
            {
                $parmStr = $parmStr.$separator.$parm;
                $separator = ", ";
            }
            $prop = new MgStringProperty( $this->parmsPropStr, $parmStr );
            $props->Add( $prop );

            $serverAdmin->SetConfigurationProperties( $this->propSectionStr, $props );
        }

    }

    define( 'ACTIVE_LOG_STATUS',        "Active" );
    define( 'ARCHIVE_LOG_STATUS',         "Archive" );
    class LogFileRecord
    {
        var $type;
        var $status;

        function LogFileRecord( $type, $status )
        {
            $this->type = $type;
            $this->status = $status;
        }
    }

    $allLogFiles = NULL;
    function GetAllLogFiles( $serverAdmin )
    {
        global $allLogFiles;

        if ( $allLogFiles == NULL )
        {
            $allLogFiles = array();
            $mgLogFilesPropertyCollection = $serverAdmin->EnumerateLogs();
            $numLogFiles = $mgLogFilesPropertyCollection->GetCount();
            for ( $i = 0; $i < $numLogFiles; $i+=3 )
            {
                $logFileName = "";
                $logFileType = "";
                $logFileStatus = "";
                for ( $j = 0; $j < 3; $j++ )
                {
                    $logFilePropName = $mgLogFilesPropertyCollection->GetItem( $i+$j )->GetName();
                    $logFilePropVal = $mgLogFilesPropertyCollection->GetItem( $i+$j )->GetValue();

                    if ( $logFilePropName == "LogNameProperty" )
                        $logFileName = $logFilePropVal;
                    else
                    if ( $logFilePropName == "LogTypeProperty" )
                    {
                        if ( $logFilePropVal == 'Access Log' )
                            $logFileType = ACCESS_LOG;
                        else
                        if ( $logFilePropVal == 'Admin Log' )
                            $logFileType = ADMIN_LOG;
                        else
                        if ( $logFilePropVal == 'Authentication Log' )
                            $logFileType = AUTHENTICATION_LOG;
                        else
                        if ( $logFilePropVal == 'Error Log' )
                            $logFileType = ERROR_LOG;
                        else
                        if ( $logFilePropVal == 'Session Log' )
                            $logFileType = SESSION_LOG;
                        else
                        if ( $logFilePropVal == 'Trace Log' )
                            $logFileType = TRACE_LOG;
                    }
                    else
                    if ( $logFilePropName == "LogStatusProperty" )
                        $logFileStatus = $logFilePropVal;

                    if ( !empty( $logFileName ) && !empty( $logFileType ) && !empty( $logFileStatus ) )
                        $allLogFiles[ $logFileName ] = new LogFileRecord( $logFileType, $logFileStatus );
                }
            }
        }

        return $allLogFiles;
    }

    function GetLogFiles( $serverAdmin, $targetLogType )
    {
        $allLogs = GetAllLogFiles( $serverAdmin );
        $logFiles = array();

        if ( $targetLogType == ACTIVE_LOGS )
        {
            foreach ( $allLogs as $logFileName => $logFileRec )
            {
                if ( $logFileRec->status == ACTIVE_LOG_STATUS )
                    $logFiles[] = $logFileName;
            }
        }
        else
        {
            foreach ( $allLogs as $logFileName => $logFileRec )
            {
                if ( $logFileRec->status == ARCHIVE_LOG_STATUS && $logFileRec->type == $targetLogType )
                    $logFiles[] = $logFileName;
            }
        }

        return $logFiles;
    }

    function GetActiveLogFilename( $serverAdmin, $targetLogType )
    {
        $allLogs = GetAllLogFiles( $serverAdmin );
        $filename = "";

        foreach ( $allLogs as $logFileName => $logFileRec )
        {
            if ( $logFileRec->status == ACTIVE_LOG_STATUS && $logFileRec->type == $targetLogType )
            {
                $filename = $logFileName;
                break;
            }
        }
        return $filename;
    }

    function LoadLogFileTable( &$targetTable, $firstIndex, $lastIndex, $serverAdmin, $targetLogType )
    {
        $allLogs = GetAllLogFiles( $serverAdmin );
        $logFiles = GetLogFiles( $serverAdmin, $targetLogType );

        // Load the table
        $targetTable = array();
        for ( $i = $firstIndex; $i <= $lastIndex; $i++ )
        {
            $logFileName = $logFiles[ $i ];
            $targetTable[ $logFileName ] = $allLogs[ $logFileName ];
        }
    }


    ////// Packages //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Package Table record type
    class PackageTableRecord
    {
        var $status;

        function PackageTableRecord( $serverAdmin, $packageName )
        {
            global $errCannotGetPackageStatus;

            $this->status = "";

            try
            {
                $statusInfo = $serverAdmin->GetPackageStatus( $packageName );
                $this->status = ( $statusInfo != NULL ) ? $statusInfo->GetStatusMessage() : "";
            }
            catch ( MgException $e )
            {
                $this->status = sprintf( $errCannotGetPackageStatus, $e->GetMessage() );
            }
        }
    }

    function GetPackagesFromDB( &$targetList )
    {
        global $site;
        global $userInfo;

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );

        // Call server to get comprehensive list of packages.

        $packages = $serverAdmin->EnumeratePackages();
        $serverAdmin->Close();

        // Load table
        $targetList = array();
        $numPackages = $packages != NULL ? $packages->GetCount() : 0 ;
        for ( $i = 0; $i < $numPackages; $i++ )
        {
            $targetList[] = $packages->GetItem( $i );
        }
    }

    $packageList = NULL;
    function GetPackageList()
    {
        global $packageList;

        if ( $packageList == NULL )
            GetPackagesFromDB( $packageList );

        return $packageList;
    }

    function GetPackageCount()
    {
        $packages = GetPackageList();
        return sizeof( $packages );
    }

    function DeletePackage( $packageName )
    {
        global $site;
        global $userInfo;
        global $packageList;

        // Validate
        $packages = GetPackageList();
        if ( empty( $packageName ) || !in_array( $packageName, $packages ) )
            return false;

        // Delete the package
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );
        $serverAdmin->DeletePackage( $packageName );
        $serverAdmin->Close();

        // Update local packageList array.
        // Note that this is done rather than calling EnumeratePackages, again, because EnumeratePackages may take some time
        // to execute.
        $oldPackageList = array();
        CopyArray( $packageList, $oldPackageList );
        $packageList = array();
        foreach ( $oldPackageList as $val )
        {
            if ( $val != $packageName )
                $packageList[] = $val;
        }

        return true;
    }

    function LoadPackageTable( &$targetTable, $firstIndex, $lastIndex )
    {
        global $site;
        global $userInfo;

        $targetTable = array();
        $packages = GetPackageList();

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );

        $iPackage = -1;
        foreach ( $packages as $val )
        {
            $iPackage++;
            if ( $iPackage < $firstIndex )
                continue;
            if ( $iPackage > $lastIndex )
                break;

            $targetTable[ $val ] = new PackageTableRecord( $serverAdmin, $val );
        }

        $serverAdmin->Close();
    }

    ////// WMS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    define( 'WMS_NAME_ITEM',                        "Service.Name");
    define( 'WMS_TITLE_ITEM',                        "Service.Title");
    define( 'WMS_FEES_ITEM',                        "Service.Fees");
    define( 'WMS_ABSTRACT_ITEM',                    "Service.Abstract");
    define( 'WMS_ACCESS_CONSTRAINTS_ITEM',            "Service.AccessConstraints");
    define( 'WMS_KEYWORDS_ITEM',                    "Service.Keywords");
    define( 'WMS_SERVER_NAME_ITEM',                    "SERVER_NAME");
    define( 'WMS_SCRIPT_NAME_ITEM',                    "SCRIPT_NAME");
    define( 'WMS_CONTACT_NAME_ITEM',                "Service.Contact.Name");
    define( 'WMS_CONTACT_ORGANIZATION_ITEM',        "Service.Contact.Organization");
    define( 'WMS_CONTACT_POSITION_ITEM',            "Service.Contact.Position");
    define( 'WMS_CONTACT_ADDRESS_TYPE_ITEM',        "Service.Contact.Address.Type");
    define( 'WMS_CONTACT_ADDRESS_STREET_ITEM',        "Service.Contact.Address.Street");
    define( 'WMS_CONTACT_ADDRESS_CITY_ITEM',        "Service.Contact.Address.City");
    define( 'WMS_CONTACT_ADDRESS_STATE_PROV_ITEM',    "Service.Contact.Address.StateProv");
    define( 'WMS_CONTACT_ADDRESS_POST_CODE_ITEM',    "Service.Contact.Address.PostCode");
    define( 'WMS_CONTACT_ADDRESS_COUNTRY_ITEM',        "Service.Contact.Address.Country");
    define( 'WMS_CONTACT_PHONE_ITEM',                "Service.Contact.Phone");
    define( 'WMS_CONTACT_FAX_ITEM',                    "Service.Contact.Fax");
    define( 'WMS_CONTACT_EMAIL_ITEM',                "Service.Contact.Email");

    $wmsServiceMetadataItems = array(
        WMS_NAME_ITEM,
        WMS_TITLE_ITEM,
        WMS_FEES_ITEM,
        WMS_ABSTRACT_ITEM,
        WMS_ACCESS_CONSTRAINTS_ITEM,
        WMS_KEYWORDS_ITEM,
        WMS_SERVER_NAME_ITEM,
        WMS_SCRIPT_NAME_ITEM,
        WMS_CONTACT_NAME_ITEM,
        WMS_CONTACT_ORGANIZATION_ITEM,
        WMS_CONTACT_POSITION_ITEM,
        WMS_CONTACT_ADDRESS_TYPE_ITEM,
        WMS_CONTACT_ADDRESS_STREET_ITEM,
        WMS_CONTACT_ADDRESS_CITY_ITEM,
        WMS_CONTACT_ADDRESS_STATE_PROV_ITEM,
        WMS_CONTACT_ADDRESS_POST_CODE_ITEM,
        WMS_CONTACT_ADDRESS_COUNTRY_ITEM,
        WMS_CONTACT_PHONE_ITEM,
        WMS_CONTACT_FAX_ITEM,
        WMS_CONTACT_EMAIL_ITEM );

    class WMSPropsRecord
    {
        var $serviceMetadata;
        var $keywords;

        function WMSPropsRecord()
        {
            global $wmsServiceMetadataItems;

            $this->serviceMetadata = array();
            $this->keywords = array();
            foreach ( $wmsServiceMetadataItems as $item )
                $this->serviceMetadata[ $item ] = "";
        }

        function GetProps()
        {
            global $site;
            global $userInfo;
            global $errInvalidWMSFile;

            // Get WMS reader
            $serverAdmin = new MgServerAdmin();
            $serverAdmin->Open( $userInfo );
            $wmsReader = $serverAdmin->GetDocument( 'Wms:OgcWmsService.config' );
            $serverAdmin->Close();

            // Get WMS metadata
            $wmsData = "";
            $chunk = "";
            do {
                $chunkSize = $wmsReader->Read( $chunk, 4096 );
                $wmsData = $wmsData.$chunk;
            } while ( $chunkSize != 0 );
            foreach ( $this->serviceMetadata as $serviceItem => $serviceVal )
            {
                $this->serviceMetadata[ $serviceItem ] = "";
                $itemPos = strpos( $wmsData, $serviceItem );
                if ( $itemPos === false )
                    throw new Exception( $errInvalidWMSFile );
                $valStartPos = strpos( $wmsData, '>', $itemPos );
                $valEndPos = strpos( $wmsData, '</Define>', $itemPos );
                if ( $valStartPos === false || $valEndPos === false || $valStartPos >= $valEndPos )
                    throw new Exception( $errInvalidWMSFile );
                $this->serviceMetadata[ $serviceItem ] = substr( $wmsData, $valStartPos + 1, $valEndPos - $valStartPos - 1 );
            }
            $this->keywords = array();
            if ( !empty( $this->serviceMetadata[ WMS_KEYWORDS_ITEM ] ) )
            {
                $keywordsStr = $this->serviceMetadata[ WMS_KEYWORDS_ITEM ];
                $keywordStart = strpos( $keywordsStr, '<item>' );
                $keywordEnd = strpos( $keywordsStr, '</item>' );
                while ( $keywordStart !== false && $keywordEnd !== false && ($keywordStart + 6) < $keywordEnd )
                {
                    $keywordStart += 6; // Length of '<item>'
                    $this->keywords[] = substr( $keywordsStr, $keywordStart, $keywordEnd - $keywordStart );
                    $keywordEnd += 7;  // Length of '</item>'
                    $keywordStart = strpos( $keywordsStr, '<item>', $keywordEnd );
                    $keywordEnd = strpos( $keywordsStr, '</item>', $keywordEnd );
                }
            }
        }

        function ValidateProps()
        {
            $xmlReservedChars = '"\'&<>\\';
            foreach ( $this->serviceMetadata as $item => $val )
                if ( strcspn( $val, $xmlReservedChars ) != strlen( $val ) )
                    throw new Exception ( "Value ".$val." contains invalid character(s) (\"'&<>\\)." );
        }

        function SetProps()
        {
            global $site;
            global $userInfo;
            global $errInvalidWMSFile;

            // Get WMS reader
            $serverAdmin = new MgServerAdmin();
            $serverAdmin->Open( $userInfo );
            $wmsReader = $serverAdmin->GetDocument( 'Wms:OgcWmsService.config' );

            // Set WMS metadata
            $wmsData = "";
            $chunk = "";
            do {
                $chunkSize = $wmsReader->Read( $chunk, 4096 );
                $wmsData = $wmsData.$chunk;
            } while ( $chunkSize != 0 );
            $keywordsStr = "";
            foreach( $this->keywords as $keyword )
                $keywordsStr = $keywordsStr.'<item>'.$keyword.'</item>';
            $this->serviceMetadata[ WMS_KEYWORDS_ITEM ] = $keywordsStr;
            foreach ( $this->serviceMetadata as $serviceItem => $serviceVal )
            {
                $itemPos = strpos( $wmsData, $serviceItem );
                if ( $itemPos === false )
                    throw new Exception( $errInvalidWMSFile );
                $valStartPos = strpos( $wmsData, '>', $itemPos );
                $valEndPos = strpos( $wmsData, '</Define>', $itemPos );
                if ( $valStartPos === false || $valEndPos === false || $valStartPos >= $valEndPos )
                    throw new Exception( $errInvalidWMSFile );
                $wmsData = substr_replace( $wmsData, $serviceVal, $valStartPos + 1, $valEndPos - $valStartPos - 1 );
            }

            // Save wms config
            $wmsByteSource = new MgByteSource( $wmsData, strlen( $wmsData ) );
            $wmsByteSource->SetMimeType( $wmsReader->GetMimeType() );
            $serverAdmin->SetDocument( 'Wms:OgcWmsService.config', $wmsByteSource->GetReader() );
            $serverAdmin->Close();
        }
    }

   ////// WFS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    define( 'WFS_NAME_ITEM',                        "Service.Name");
    define( 'WFS_TITLE_ITEM',                        "Service.Title");
    define( 'WFS_FEES_ITEM',                        "Service.Fees");
    define( 'WFS_ABSTRACT_ITEM',                    "Service.Abstract");
    define( 'WFS_ACCESS_CONSTRAINTS_ITEM',            "Service.AccessConstraints");
    define( 'WFS_KEYWORDS_ITEM',                    "Service.Keywords");
    define( 'WFS_SERVER_NAME_ITEM',                    "SERVER_NAME");
    define( 'WFS_SCRIPT_NAME_ITEM',                    "SCRIPT_NAME");
    define( 'WFS_CONTACT_NAME_ITEM',                "Service.Contact.Name");
    define( 'WFS_CONTACT_ORGANIZATION_ITEM',        "Service.Contact.Organization");
    define( 'WFS_CONTACT_POSITION_ITEM',            "Service.Contact.Position");
    define( 'WFS_CONTACT_ADDRESS_TYPE_ITEM',        "Service.Contact.Address.Type");
    define( 'WFS_CONTACT_ADDRESS_STREET_ITEM',        "Service.Contact.Address.Street");
    define( 'WFS_CONTACT_ADDRESS_CITY_ITEM',        "Service.Contact.Address.City");
    define( 'WFS_CONTACT_ADDRESS_STATE_PROV_ITEM',    "Service.Contact.Address.StateProv");
    define( 'WFS_CONTACT_ADDRESS_POST_CODE_ITEM',    "Service.Contact.Address.PostCode");
    define( 'WFS_CONTACT_ADDRESS_COUNTRY_ITEM',        "Service.Contact.Address.Country");
    define( 'WFS_CONTACT_PHONE_ITEM',                "Service.Contact.Phone");
    define( 'WFS_CONTACT_FAX_ITEM',                    "Service.Contact.Fax");
    define( 'WFS_CONTACT_EMAIL_ITEM',                "Service.Contact.Email");

    $wfsServiceMetadataItems = array(
        WFS_NAME_ITEM,
        WFS_TITLE_ITEM,
        WFS_FEES_ITEM,
        WFS_ABSTRACT_ITEM,
        WFS_ACCESS_CONSTRAINTS_ITEM,
        WFS_KEYWORDS_ITEM,
        WFS_SERVER_NAME_ITEM,
        WFS_SCRIPT_NAME_ITEM,
        WFS_CONTACT_NAME_ITEM,
        WFS_CONTACT_ORGANIZATION_ITEM,
        WFS_CONTACT_POSITION_ITEM,
        WFS_CONTACT_ADDRESS_TYPE_ITEM,
        WFS_CONTACT_ADDRESS_STREET_ITEM,
        WFS_CONTACT_ADDRESS_CITY_ITEM,
        WFS_CONTACT_ADDRESS_STATE_PROV_ITEM,
        WFS_CONTACT_ADDRESS_POST_CODE_ITEM,
        WFS_CONTACT_ADDRESS_COUNTRY_ITEM,
        WFS_CONTACT_PHONE_ITEM,
        WFS_CONTACT_FAX_ITEM,
        WFS_CONTACT_EMAIL_ITEM);

    class WFSPropsRecord
    {
        var $serviceMetadata;
        var $keywords;

        function WFSPropsRecord()
        {
            global $wfsServiceMetadataItems;

            $this->serviceMetadata = array();
            $this->keywords = array();
            foreach ( $wfsServiceMetadataItems as $item )
                $this->serviceMetadata[ $item ] = "";
        }

        function GetProps()
        {
            global $site;
            global $userInfo;
            global $errInvalidWFSFile;

            // Get WFS reader
            $serverAdmin = new MgServerAdmin();
            $serverAdmin->Open( $userInfo );
            $wfsReader = $serverAdmin->GetDocument( 'Wfs:OgcWfsService.config' );
            $serverAdmin->Close();

            // Get WFS metadata
            $wfsData = "";
            $chunk = "";
            do {
                $chunkSize = $wfsReader->Read( $chunk, 4096 );
                $wfsData = $wfsData.$chunk;
            } while ( $chunkSize != 0 );
            foreach ( $this->serviceMetadata as $serviceItem => $serviceVal )
            {
                $this->serviceMetadata[ $serviceItem ] = "";
                $itemPos = strpos( $wfsData, $serviceItem );
                if ( $itemPos === false )
                    throw new Exception( $errInvalidWFSFile );
                $valStartPos = strpos( $wfsData, '>', $itemPos );
                $valEndPos = strpos( $wfsData, '</Define>', $itemPos );
                if ( $valStartPos === false || $valEndPos === false || $valStartPos >= $valEndPos )
                    throw new Exception( $errInvalidWFSFile );
                $this->serviceMetadata[ $serviceItem ] = substr( $wfsData, $valStartPos + 1, $valEndPos - $valStartPos - 1 );
            }
            $this->keywords = array();
            if ( !empty( $this->serviceMetadata[ WFS_KEYWORDS_ITEM ] ) )
            {
                $keywordsStr = $this->serviceMetadata[ WFS_KEYWORDS_ITEM ];
                $keywordStart = strpos( $keywordsStr, '<item>' );
                $keywordEnd = strpos( $keywordsStr, '</item>' );
                while ( $keywordStart !== false && $keywordEnd !== false && ($keywordStart + 6) < $keywordEnd )
                {
                    $keywordStart += 6; // Length of '<item>'
                    $this->keywords[] = substr( $keywordsStr, $keywordStart, $keywordEnd - $keywordStart );
                    $keywordEnd += 7;  // Length of '</item>'
                    $keywordStart = strpos( $keywordsStr, '<item>', $keywordEnd );
                    $keywordEnd = strpos( $keywordsStr, '</item>', $keywordEnd );
                }
            }
        }

        function ValidateProps()
        {
            $xmlReservedChars = '"\'&<>\\';
            foreach ( $this->serviceMetadata as $item => $val )
                if ( strcspn( $val, $xmlReservedChars ) != strlen( $val ) )
                    throw new Exception ( "Value ".$val." contains invalid character(s) (\"'&<>\\)." );
        }

        function SetProps()
        {
            global $site;
            global $userInfo;
            global $errInvalidWFSFile;

            // Get WFS reader
            $serverAdmin = new MgServerAdmin();
            $serverAdmin->Open( $userInfo );
            $wfsReader = $serverAdmin->GetDocument( 'Wfs:OgcWfsService.config' );

            // Set WFS metadata
            $wfsData = "";
            $chunk = "";
            do {
                $chunkSize = $wfsReader->Read( $chunk, 4096 );
                $wfsData = $wfsData.$chunk;
            } while ( $chunkSize != 0 );
            $keywordsStr = "";
            foreach( $this->keywords as $keyword )
                $keywordsStr = $keywordsStr.'<item>'.$keyword.'</item>';
            $this->serviceMetadata[ WFS_KEYWORDS_ITEM ] = $keywordsStr;
            foreach ( $this->serviceMetadata as $serviceItem => $serviceVal )
            {
                $itemPos = strpos( $wfsData, $serviceItem );
                if ( $itemPos === false )
                    throw new Exception( $errInvalidWFSFile );
                $valStartPos = strpos( $wfsData, '>', $itemPos );
                $valEndPos = strpos( $wfsData, '</Define>', $itemPos );
                if ( $valStartPos === false || $valEndPos === false || $valStartPos >= $valEndPos )
                    throw new Exception( $errInvalidWFSFile );
                $wfsData = substr_replace( $wfsData, $serviceVal, $valStartPos + 1, $valEndPos - $valStartPos - 1 );
            }

            // Save wfs config
            $wfsByteSource = new MgByteSource( $wfsData, strlen( $wfsData ) );
            $wfsByteSource->SetMimeType( $wfsReader->GetMimeType() );
            $serverAdmin->SetDocument( 'Wfs:OgcWfsService.config', $wfsByteSource->GetReader() );
            $serverAdmin->Close();
        }
    }

?>
