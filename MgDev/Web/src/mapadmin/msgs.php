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

    // Error Messages:
    $errorMsg = "";
    $errParm1 = "";
    $errAuthenticationFailed = "Authentication failed.  Please enter a valid username and password.";
    $errPasswordConfirmationFailed = "Password and password confirmation do not match.";
    $errAdminIDMissing = "Administrator's ID is not specified.";
    $errPasswordMissing = "Password is not specified.";
    $errNoUserSelectionForDelete = "User must be selected in order to delete.";
    $errUserIDMissing = "User's ID is not specified.";
    $errUserNameMissing = "User's name is not specified.";
    $errNoGroupSelectionForDelete = "Group must be selected in order to delete.";
    $errGroupNameMissing = "Group's name is not specified.";
    $errMappingNameMissing = "Alias name is not specified.";
    $errMappingNameExists = "Alias name already exists.";
    $errLocationMissing = "Path to external folder is not specified.";
    $errInvalidInput = "Input values are invalid.";
    $errInvalidServerName = "Server name must be specified.";
    $errInvalidServerAddress = "Server IP address must be specified.";
    $errInvalidServerDefaultLocale = "Locale must be specified.";
    $errInvalidServerLocalizationResourcesPath = "Localization resources path must be specified.";
    $errInvalidServerTcpIpMtu = "TCP/IP maximum transmission unit is invalid.";
    $errInvalidServerTempPath = "Temp file path must be specified.";
    $errInvalidServerFdoPath = "Fdo file path must be specified.";
    $errInvalidServerMaxConns = "Maximum %s connections is invalid.";
    $errInvalidServerConnQueueSize = "%s connection queue size is invalid.";
    $errInvalidServerConnThreadPoolSize = "%s connection thread pool size is invalid.";
    $errInvalidSiteResponseTimeOut = "Site service response timeout is invalid.";
    $errInvalidConnectionTimeOut = "Connection timeout is invalid.";
    $errInvalidConnectionTimerInt = "Connection timer interval is invalid.";
    $errInvalidServiceRegistrationTimerInt = "Service registration timer interval is invalid.";
    $errInvalidFeatureDataConnPoolSize = "Feature service data connection pool size is invalid.";
    $errInvalidFeatureDataConnTimeOut = "Feature service data connection timeout is invalid.";
    $errInvalidFeatureDataConnTimerInt = "Feature service data connection timer interval is invalid.";
    $errInvalidFeatureDataCacheSize = "Feature service data cache size is invalid.";
    $errInvalidResourceDataFileTrashFolder = "Resource service trash folder must be specified.";
    $errInvalidResourceLibraryDataFileFolder = "Resource service library data file folder must be specified.";
    $errInvalidResourceLibraryRepositoryFolder = "Resource service library repository folder must be specified.";
    $errInvalidResourceSessionDataFileFolder = "Resource service session data file folder must be specified.";
    $errInvalidResourceSessionRepositoryFolder = "Resource service session repository folder must be specified.";
    $errInvalidResourceSiteRepositoryFolder = "Resource service site repository folder must be specified.";
    $errInvalidResourceResourceSchemaFolder = "Resource service schema folder must be specified.";
    $errInvalidRepositoryCheckpointsTimerInterval = "Repository checkpoints timer interval is invalid.";
    $errInvalidPackagesPath = "Resource service packages folder must be specified.";
    $errInvalidLogRootFolder = "Log root folder must be specified.";
    $errInvalidLogFilename = "Log filename must be specified.";
    $errInvalidLogTimeframe = "Stop time must be later than start time.";
    $errNoServerSelectionForDelete = "Server must be selected in order to delete.";
    $errNotFound = "%s cannot be found.";
    $errInvalidFrequency = "Update Frequency is invalid.";
    $errServerNameMissing = "Server name is not specified.";
    $errServerAddressMissing = "Server address is not specified.";
    $errServerIsDown = "%s is down.";
    $errCannotOpenFolder = 'Cannot access "%s".  Please ensure that the path exists and you have access permission.';
    $errNoPackageSpecified = "No package specified.";
    $errNoResourceSpecified = "No resource folder specified.";
    $errCannotCreatePackage = "Cannot create package %s from folder %s:  ";
    $errNoPackagesFound = "No packages found.";
    $errNoPackageLog = "No package log specified.";
    $errNoPackageSelectionForDelete = "Package must be selected in order to delete.";
    $errFatalError = "Fatal%20error%20encountered!%20%20Please%20login%20again.";
    $errNoLogFileSpecified = "No log file specified.";
    $errNoLogSpecified = "No log specified.";
    $errInvalidMaxLogSize = "Invalid maximum log size.";
    $errHelpPageNotFound = "No help page specified.";
    $errInvalidWMSFile = "Invalid WMS file.";
    $errInvalidWFSFile = "Invalid WFS file.";
    $errInvalidTileCachePath = "Tile service tile cache path must be specified.";
    $errInvalidSessionTimeOut = "Site service session timeout is invalid.";
    $errInvalidSessionTimerInt = "Site service session timer interval is invalid.";
    $errCannotGetPackageStatus = "Cannot get package status: %s";
    $errConnectionFailed = "Cannot establish connection.";

    // Confirmation Messages:
    $confSuccessfulDeletion = "%s has been successfully deleted.";
    $confSuccessfulAddition = "%s has been successfully added.";
    $confSuccessfulUpdate = "%s has been successfully updated.";
    $confSuccessfulRoleUpdate = "Roles have been successfully updated.";
    $confSuccessfulLogRetrieval = "%s log has been successfully retrieved.";
    $confSuccessfulLogClearing = "%s log has been successfully cleared.";
    $confSuccessfulServersUpdate = "Servers have been successfully updated.";
    $confPackageLoadCompleted = "%s load has completed. Please check status.";
    $confPackageMakeCompleted = "Package %s has been created from folder %s.";
    $confSuccessfulWMSUpdate = "WMS properties have been successfully updated.";
    $confSuccessfulWFSUpdate = "WFS properties have been successfully updated.";

    // Role Descriptions:
    $descAdmin = "Administrators maintain the Database.  You must be an administrator to Create/Maintain Users and Groups, establish roles for Users and Groups, and update the Server configuration.";
    $descAuthor = "Authors maintain the maps.  You must be an author to Create/Maintain a map or any of its attributes.";

    // Special label for Everyone group in selectors
    $everyoneGroupSelectorLabel = "Everyone (Built-in group)";
?>
