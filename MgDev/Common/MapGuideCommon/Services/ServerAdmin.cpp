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

#include "MapGuideCommon.h"
#include "ServerAdmin.h"
#include "ServerAdminDefs.h"
#include "Command.h"

static int ServerAdmin_Service = (int)MgPacketParser::msiServerAdmin;

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default Constructor for a ServerAdmin object.
/// </summary>
MgServerAdmin::MgServerAdmin()
{
    m_warnings = new MgWarnings();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Self Destructor
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgServerAdmin::Dispose()
{
    delete this;
}

/// <summary>
/// Destructor
/// </summary>
MgServerAdmin::~MgServerAdmin()
{
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Opens a connection to a specified support Server.
/// <param name="server">
/// Server IP address or DNS name.
/// </summary>
/// </param>
/// <param name="userInformation">
/// UserCredentials to authenticate against
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgServerNotFoundException
/// MgConnectionFailedException
void MgServerAdmin::Open(CREFSTRING supportServer, MgUserInformation* userInformation)
{
    MgSiteManager* siteManager = MgSiteManager::GetInstance();
    m_connProp = siteManager->GetSupportServerConnectionProperties(supportServer, userInformation, MgSiteInfo::Admin);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Opens a connection to a server.
/// </summary>
/// <param name="userInformation">
/// UserCredentials to authenticate against
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgServerNotFoundException
/// MgConnectionFailedException
void MgServerAdmin::Open(MgUserInformation* userInformation)
{
    MgSiteManager* siteManager = MgSiteManager::GetInstance();
    m_connProp = siteManager->GetConnectionProperties(userInformation, MgSiteInfo::Admin, true);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Closes the current Server connection.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::Close()
{
    m_connProp = NULL;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// GetWarningsObject() method returns the latest warnings.
/// </summary>
///
/// <returns>
/// Returns a pointer to the MgWarnings.
/// </returns>
MgWarnings* MgServerAdmin::GetWarningsObject()
{
    return m_warnings;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// HasWarning() method reports if there is a current warning.
/// </summary>
///
/// <returns>
/// Returns true or false
/// </returns>
bool MgServerAdmin::HasWarning()
{
    return m_warnings != NULL ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the configuration properties for the specified property section.
/// </summary>
/// <param name="propertySection">
/// The property section to get.
/// </param>
/// <returns>
/// The collection of configuration properties associated with the specified property section.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidPropertySectionException
/// MgPropertySectionNotAvailableException
MgPropertyCollection* MgServerAdmin::GetConfigurationProperties(CREFSTRING propertySection)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetConfigurationProperties, // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &propertySection,  // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the configuration properties for the specified property section.
/// </summary>
/// <param name="propertySection">
/// The property section to set.
/// </param>
/// <param name="properties">
/// The collection of configuration properties associated with the specified property section that you want to set.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidPropertySectionException
/// MgPropertySectionNotAvailableException
/// MgPropertySectionReadOnlyException
/// MgInvalidPropertyException
void MgServerAdmin::SetConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::SetConfigurationProperties, // Command Code
                        2,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &propertySection,  // Argument#1
                        MgCommand::knObject, properties,        // Argument#2
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the configuration properties for the specified property section.
/// If the properties are not specified, then the entire section will be removed.
/// </summary>
/// <param name="propertySection">
/// The property section to set.
/// </param>
/// <param name="properties">
/// The collection of configuration properties associated with the specified property section that you want to remove.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidPropertySectionException
/// MgPropertySectionNotAvailableException
/// MgPropertySectionReadOnlyException
/// MgInvalidPropertyException
void MgServerAdmin::RemoveConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::RemoveConfigurationProperties, // Command Code
                        2,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &propertySection,  // Argument#1
                        MgCommand::knObject, properties,        // Argument#2
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Clears the specified log.
/// </summary>
/// <param name="log">
/// The log to be cleared. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
/// SessionLog, TraceLog)
/// </param>
/// <returns>
/// True if the log was successfully cleared, false otherwise.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgNullReferenceException
bool MgServerAdmin::ClearLog(CREFSTRING log)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knInt8,                      // Return type expected
                        MgServerAdminServiceOpId::ClearLog,     // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &log,              // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the specified log if it exists.
/// </summary>
/// <param name="fileName'>
/// The name of the log to be deleted from the logs directory (does not include path)
/// </param>
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgFileNotFoundException
/// MgFileIoException
void MgServerAdmin::DeleteLog(CREFSTRING fileName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::DeleteLog,    // Command Code
                        1,                                      // No. of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &fileName,         // Argument #1
                        MgCommand::knNone);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Renames the specified log to the new name given
/// </summary>
/// <param name="oldFileName">
/// The current name of the log file
/// </param>
/// <param name="newFileName">
/// The new name to give the log file
/// </param>
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgDuplicateFileException
/// MgFileNotFoundException
/// MgInvalidArgumentException
/// MgFileIoException
void MgServerAdmin::RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::RenameLog,    // Command Code
                        2,                                      // No. of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &oldFileName,      // Argument #1
                        MgCommand::knString, &newFileName,      // Argument #2
                        MgCommand::knNone);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns a property collection listing the contents of the logs directory.
/// </summary>
/// <returns>
/// A property collection listing the contents of the logs directory.
/// </returns>
///
/// EXCEPTIONS:
/// MgFileIoException
MgPropertyCollection* MgServerAdmin::EnumerateLogs()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::EnumerateLogs,// Command Code
                        0,                                      // No. of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the contents of the specified log. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
/// SessionLog, TraceLog)
/// </summary>
/// <param name="log">
/// The log to retrieve.
/// </param>
/// <returns>
/// The log contents in a ByteReader.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgNullReferenceException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetLog(CREFSTRING log)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetLog,       // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &log,              // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the specified number of entries from the specified log.
/// </summary>
/// <param name="log">
/// The log to retrieve. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
/// SessionLog, TraceLog)
/// </param>
/// <param name="numEntries">
/// The number of entries to return.
/// </param>
/// <returns>
/// The log contents in a ByteReader.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgArgumentOutOfRangeException
/// MgNullReferenceException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetLog(CREFSTRING log, INT32 numEntries)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetLog,       // Command Code
                        2,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &log,              // Argument#1
                        MgCommand::knInt32, numEntries,         // Argument#2
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the contents of the specified log between two dates.  A maximum 24 hour
/// period is allowed to be retrieved.  This will use the current file name convention
/// for the log so if the filename was changed it will be unable to find entries
/// located previous files.
/// </summary>
/// <param name="log">
/// The log to retrieve. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
/// MapLayerAccessLog, SessionLog, TraceLog)
/// </param>
/// <param name="fromDate">
/// Date & time to start pulling entries from
/// </param>
/// <param name="toDate">
/// Last date & time to pull entries for
/// </param>
/// <returns>
/// The log contents in a ByteReader.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidLogTypeException
/// MgNullReferenceException
/// MgInvalidArgumentException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetLog(CREFSTRING log, MgDateTime* fromDate, MgDateTime* toDate)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetLog,       // Command Code
                        3,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &log,              // Argument#1
                        MgCommand::knObject, fromDate,          // Argument#2
                        MgCommand::knObject, toDate,            // Argument#3
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the contents of the specified log file.
/// </summary>
/// <param name="logFile">
/// The log file to retrieve.
/// </param>
/// <returns>
/// The log file contents in a ByteReader.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgNullReferenceException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetLogFile(CREFSTRING logFile)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetLogFile,   // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &logFile,          // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Allows the server to process client operations.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::BringOnline()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::BringOnline,  // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Prevents the server from processing client operations.  When offline, the
/// adminstrator can access the server via "Admin" operations without worrying
/// about Mg clients using the server.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::TakeOffline()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::TakeOffline,  // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the online status of the server.
/// </summary>
/// <returns>
/// True for online, False for offline.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
bool MgServerAdmin::IsOnline()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knInt8,                      // Return type expected
                        MgServerAdminServiceOpId::IsOnline,     // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}


///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the information properties for the server.
/// </summary>
/// <param name="server">
/// The server name or IP address.
/// </param>
/// <returns>
/// The collection of information properties.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
MgPropertyCollection* MgServerAdmin::GetInformationProperties()
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::GetInformationProperties, // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Delete the specified package, if able.
/// </summary>
///
/// <param name="packageName">
/// The name of the package to be deleted.  Available packages can be found by
/// using EnumeratePackages().
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidArgumentException
/// MgFileIoException
/// MgFileNotFoundException
void MgServerAdmin::DeletePackage(CREFSTRING packageName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::DeletePackage,// Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &packageName,      // Argument #1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Enumerates the packages available in the package directory.
/// </summary>
///
/// <returns>
/// An MgStringCollection containing a list of packages in the packages directory
/// </returns>
///
/// EXCEPTIONS:
/// MgOutOfMemoryException
/// MgFileNotFoundException
/// MgFileIoException
MgStringCollection* MgServerAdmin::EnumeratePackages()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgServerAdminServiceOpId::EnumeratePackages, // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets current log of the specified package
/// </summary>
///
/// <param name="packageName">
/// The name of the package to get the status for.  Available packages can be
/// found by using EnumeratePackages().
/// </param>
///
/// <returns>
/// An MgByteReader containing the contents of the package's log.
/// </returns>
///
/// EXCEPTIONS:
/// MgFileNotFoundException
/// MgFileIoException
/// MgInvalidArgumentException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetPackageLog(CREFSTRING packageName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                        MgCommand::knObject,                        // Return type expected
                        MgServerAdminServiceOpId::GetPackageLog,    // Command Code
                        1,                                          // No of arguments
                        ServerAdmin_Service,                        // Service Id
                        BUILD_VERSION(1,0,0),                       // Operation version
                        MgCommand::knString, &packageName,          // Argument #1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves the current status of the specified package.
///
/// \param packageName
/// The name of the package to get the status for.
/// Available packages can be found by using EnumeratePackages().
///
/// \return
/// The status of the package.
///
MgPackageStatusInformation* MgServerAdmin::GetPackageStatus(CREFSTRING packageName)
{
    ACE_ASSERT(m_connProp != NULL);
    MgCommand cmd;

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knObject,                                // Return type
        MgServerAdminServiceOpId::GetPackageStatus,         // Command Code
        1,                                                  // Number of arguments
        ServerAdmin_Service,                                // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knString, &packageName,                  // Argument #1
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgPackageStatusInformation*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified package into a resource repository.
///
void MgServerAdmin::LoadPackage(CREFSTRING packageName)
{
    assert(m_connProp != NULL);
    MgCommand cmd;

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knVoid,                                  // Return type
        MgServerAdminServiceOpId::LoadPackage,              // Command Code
        1,                                                  // Number of arguments
        ServerAdmin_Service,                                // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knString, &packageName,                  // Argument #1
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void MgServerAdmin::MakePackage(MgResourceIdentifier* resource,
    CREFSTRING packageName, CREFSTRING packageDescription)
{
    assert(m_connProp != NULL);
    MgCommand cmd;

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knVoid,                                  // Return type
        MgServerAdminServiceOpId::MakePackage,              // Command Code
        3,                                                  // Number of arguments
        ServerAdmin_Service,                                // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knObject, resource,                      // Argument #1
        MgCommand::knString, &packageName,                  // Argument #2
        MgCommand::knString, &packageDescription,           // Argument #3
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// Registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// List of server information from all other servers within a site.
/// The returned list may be NULL.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException

MgSerializableCollection* MgServerAdmin::RegisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    MgCommand cmd;

    MG_TRY()

    cmd.ExecuteCommand(m_connProp,                                          // Connection
                        MgCommand::knObject,                                // Return type
                        MgServerAdminServiceOpId::RegisterServicesOnServers,// Command code
                        1,                                                  // Number of arguments
                        ServerAdmin_Service,                                // Service ID
                        BUILD_VERSION(1,0,0),                               // Operation version
                        MgCommand::knObject, serverInfoList,                // Argument #1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_CATCH_AND_THROW(L"MgServerAdmin.RegisterServicesOnServers")

    return (MgSerializableCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// Un-registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException

void MgServerAdmin::UnregisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    MG_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                                              // Connection
                        MgCommand::knVoid,                                      // Return type
                        MgServerAdminServiceOpId::UnregisterServicesOnServers,  // Command code
                        1,                                                      // Number of arguments
                        ServerAdmin_Service,                                    // Service ID
                        BUILD_VERSION(1,0,0),                                   // Operation version
                        MgCommand::knObject, serverInfoList,                    // Argument #1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_CATCH_AND_THROW(L"MgServerAdmin.UnregisterServicesOnServers")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Send a resource change notification.
///
void MgServerAdmin::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    MG_TRY()

    assert(m_connProp != NULL);
    MgCommand cmd;

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knVoid,                                  // Return type
        MgServerAdminServiceOpId::NotifyResourcesChanged,   // Command code
        1,                                                  // Number of arguments
        ServerAdmin_Service,                                // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knObject, resources,                     // Argument #1
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_CATCH_AND_THROW(L"MgServerAdmin.NotifyResourcesChanged")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Specifies the maximum size in kilobytes for the log files.  When the maximum
/// size is exceeded, the current log will be archived, and a new log will be created.
///
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::SetMaximumLogSize(INT32 size)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knVoid,                    // Return type expected
                        MgServerAdminServiceOpId::SetMaximumLogSize,       // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knInt32, size,              // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Specifies the delimiter to use for separating the data fields in the logs.
///
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::SetLogDelimiter(CREFSTRING delimiter)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::SetLogsDelimiter, // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &delimiter,      // Argument #1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Enables/disables maximum log file size restriction.  Maximum log file size is
/// set with MgServerAdmin::SetMaximumLogSize().
///
/// </summary>
/// <param name="useMaxSize">
/// Log file size restriction is enforced if useMaxSize = true.
/// using EnumeratePackages().
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdmin::EnableMaximumLogSize(bool useMaxSize)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgServerAdminServiceOpId::EnableMaximumLogSize, // Command Code
                        1,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knInt8, (INT8)useMaxSize,      // Argument #1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if log file size is restricted to the maximum size
/// set with MgServerAdmin::SetMaximumLogSize().
///
/// </summary>
/// <returns>
/// The status of the use of the maximum log size restriction
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
bool MgServerAdmin::IsMaximumLogSizeEnabled()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                        MgCommand::knInt8,                      // Return type expected
                        MgServerAdminServiceOpId::IsMaximumLogSizeEnabled,     // Command Code
                        0,                                      // No of arguments
                        ServerAdmin_Service,                    // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the contents of the specified document.
/// </summary>
/// <param name="identifier">
/// The document to retrieve.
/// </param>
/// <returns>
/// The document contents in a ByteReader.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgNullReferenceException
/// MgOutOfMemoryException
MgByteReader* MgServerAdmin::GetDocument(CREFSTRING identifier)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                            // Connection
                       MgCommand::knObject,                   // Return type expected
                       MgServerAdminServiceOpId::GetDocument, // Command Code
                       1,                                     // No of arguments
                       ServerAdmin_Service,                   // Service Id
                       BUILD_VERSION(1,0,0),                  // Operation version
                       MgCommand::knString, &identifier,      // Argument#1
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the contents of the specified document.
/// </summary>
/// <param name="identifier">
/// The document to set.
/// </param>
/// <param name="data">
/// The data to set the document contents to.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgNullReferenceException
/// MgOutOfMemoryException
void MgServerAdmin::SetDocument(CREFSTRING identifier, MgByteReader* data)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                             // Connection
                       MgCommand::knVoid,                      // Return type expected
                       MgServerAdminServiceOpId::SetDocument,  // Command Code
                       2,                                      // No of arguments
                       ServerAdmin_Service,                    // Service Id
                       BUILD_VERSION(1,0,0),                   // Operation version
                       MgCommand::knString, &identifier,       // Argument#1
                       MgCommand::knObject, data,              // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Get the unique identifier for the class
///</summary>
///<returns>Class Identifider.</returns>
///
INT32 MgServerAdmin::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Sets the warnings if any
///</summary>
///
void MgServerAdmin::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgWarnings> ptrWarning = warning;
        Ptr<MgStringCollection> ptrCol = ptrWarning->GetMessages();
        this->m_warnings->AddMessages(ptrCol);
    }
}
