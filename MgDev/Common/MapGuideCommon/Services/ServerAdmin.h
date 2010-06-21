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

#ifndef MGSERVERADMIN_H_
#define MGSERVERADMIN_H_

#include "ServerAdminDefs.h"

/// \cond INTERNAL
///////////////////////////////////////////////////////////
/// \brief
/// This class contains methods to allow MapGuide clients to perform administration related
/// operations:
/// - Server Configuration
/// - Logging
/// - Online/Offline
/// - Server Health
///
class MG_MAPGUIDE_API MgServerAdmin : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerAdmin)

EXTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor for an MgServerAdmin object.
    ///
    /// \return
    /// Nothing
    ///
    MgServerAdmin();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgServerAdmin();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Opens a connection to a specified Server.
    ///
    /// \param server
    /// Server IP address or DNS name.
    /// \param userInformation
    /// User information - credentials, locale, session identifier
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgServerNotFoundException
    /// \exception MgConnectionFailedException
    ///
    void Open(CREFSTRING server, MgUserInformation* userInformation);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Opens a connection to a Server.
    ///
    /// \param userInformation
    /// User information - credentials, locale, session identifier
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgServerNotFoundException
    /// \exception MgConnectionFailedException
    ///
    void Open(MgUserInformation* userInformation);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Closes the current Server connection.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void Close();

    /// \brief
    /// HasWarning() method reports if there is a current warning.
    ///
    /// \return
    /// Returns true or false
    ///
    /// \exception MgConnectionNotOpenException
    ///
    bool HasWarning();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetWarningsObject() method returns all the current warning.
    ///
    /// \return
    /// Returns a pointer to the MgWarnings object.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    MgWarnings* GetWarningsObject();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the configuration properties for the specified property section.
    ///
    /// \param propertySection
    /// The property section to get.
    ///
    /// \return
    /// The collection of configuration properties associated with the specified property section.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidPropertySectionException
    /// \exception MgPropertySectionNotAvailableException
    ///
    MgPropertyCollection* GetConfigurationProperties(CREFSTRING propertySection);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the configuration properties for the specified property section.
    ///
    /// \param propertySection
    /// The property section to set.
    /// \param properties
    /// The collection of configuration properties associated with the specified property section that you want to set.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidPropertySectionException
    /// \exception MgPropertySectionNotAvailableException
    /// \exception MgPropertySectionReadOnlyException
    /// \exception MgInvalidPropertyException
    ///
    void SetConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the configuration properties for the specified property section.
    /// If the properties are not specified, then the entire section will be removed.
    ///
    /// \param propertySection
    /// The property section to remove.
    /// \param properties
    /// The collection of configuration properties associated with the specified property section that you want to remove.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidPropertySectionException
    /// \exception MgPropertySectionNotAvailableException
    /// \exception MgPropertySectionReadOnlyException
    /// \exception MgInvalidPropertyException
    ///
    void RemoveConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Clears the specified log.
    ///
    /// \param log
    /// The log to be cleared. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
    /// SessionLog, TraceLog)
    ///
    /// \return
    /// True if the log was successfully cleared, false otherwise.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullReferenceException
    ///
    bool ClearLog(CREFSTRING log);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the specified log if it exists.
    ///
    /// \param fileName
    /// The name of the log to be deleted from the logs directory (does not include path)
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgFileNotFoundException
    /// \exception MgFileIoException
    ///
    void DeleteLog(CREFSTRING fileName);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Renames the specified log to the new name given
    ///
    /// \param oldFileName
    /// The current name of the log file
    /// \param newFileName
    /// The new name to give the log file
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgDuplicateFileException
    /// \exception MgFileNotFoundException
    /// \exception MgInvalidArgumentException
    /// \exception MgFileIoException
    ///
    void RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a string collection listing the contents of the logs directory.
    ///
    /// \return
    /// A string collection listing the contents of the logs directory.
    ///
    /// \exception MgFileIoException
    ///
    MgPropertyCollection* EnumerateLogs();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the contents of the specified log.
    ///
    /// \param log
    /// The log to retrieve. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
    /// SessionLog, TraceLog)
    ///
    /// \return
    /// The log contents in a ByteReader.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullReferenceException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetLog(CREFSTRING log);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the specified number of entries from the specified log.
    ///
    /// \param log
    /// The log to retrieve. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
    /// SessionLog, TraceLog)
    /// \param numEntries
    /// The number of entries to return.
    ///
    /// \return
    /// The log contents in a ByteReader.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgNullReferenceException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetLog(CREFSTRING log, INT32 numEntries);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the contents of the specified log between two dates.  A maximum 24 hour
    /// period is allowed to be retrieved.  This will use the current file name convention
    /// for the log so if the filename was changed it will be unable to find entries
    /// located previous files.
    ///
    /// \param log
    /// The log to retrieve. (AccessLog, AdminLog, AuthenticationLog, ErrorLog,
    /// MapLayerAccessLog, SessionLog, TraceLog)
    /// \param fromDate
    /// Date & time to start pulling entries from
    /// \param toDate
    /// Last date & time to pull entries for
    ///
    /// \return
    /// The log contents in a ByteReader.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidLogTypeException
    /// \exception MgNullReferenceException
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetLog(CREFSTRING log, MgDateTime* fromDate, MgDateTime* toDate);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the contents of the specified log file.
    ///
    /// \param logFile
    /// The log file to retrieve.
    ///
    /// \return
    /// The log contents in a ByteReader.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullReferenceException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetLogFile(CREFSTRING logFile);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Allows the server to process client operations.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void BringOnline();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Prevents the server from processing client operations.  When offline, the
    /// adminstrator can access the server via "Admin" operations without worrying
    /// about MapGuide clients using the server.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void TakeOffline();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the online status of the server.
    ///
    /// \return
    /// True for online, False for offline.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    bool IsOnline();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the information properties for the server.
    ///
    /// \return
    /// The collection of information properties.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    MgPropertyCollection* GetInformationProperties();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the site version.
    ///
    /// \return
    /// The site version.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    STRING GetSiteVersion();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the status properties for the server.
    ///
    /// \return
    /// The collection of status properties.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    MgPropertyCollection* GetSiteStatus();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Delete the specified package, if able.
    ///
    /// \param packageName
    /// The name of the package to be deleted.  Available packages can be found by
    /// using EnumeratePackages().
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgFileIoException
    /// \exception MgFileNotFoundException
    ///
    void DeletePackage(CREFSTRING packageName);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the packages available in the package directory.
    ///
    /// \return
    /// An MgStringCollection containing a list of packages in the packages directory
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgFileNotFoundException
    /// \exception MgFileIoException
    ///
    MgStringCollection* EnumeratePackages();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets current log of the specified package
    ///
    /// \param packageName
    /// The name of the package to get the status for.  Available packages can be
    /// found by using EnumeratePackages().
    ///
    /// \return
    /// An MgByteReader containing the contents of the package's log.
    ///
    /// \exception MgFileNotFoundException
    /// \exception MgFileIoException
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetPackageLog(CREFSTRING packageName);

    ///////////////////////////////////////////////////////////////////////////
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
    MgPackageStatusInformation* GetPackageStatus(CREFSTRING packageName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Loads the specified resource package into the repository.
    ///
    /// \remarks
    /// This method only works on "Library" repository.
    ///
    /// \param packageName
    /// The name of the package to be loaded.  Available packages can be found by
    /// using EnumeratePackages().
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgFileNotFoundException
    /// \exception MgPathTooLongException
    /// \exception MgFileIoException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgOutOfMemoryException
    ///
    void LoadPackage(CREFSTRING packageName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a package from the specified resource, and then saves it into
    /// the specified name.
    ///
    /// \remarks
    /// This method only works on "Library" repository.
    ///
    /// \param resource
    /// Resource identifier of a folder containing permissible resources to be
    /// packaged.
    ///
    /// \param packageName
    /// The name of the package to be made. The serverconfig.ini file has the
    /// "PackagesPath" property which will be used for path. The file name will
    /// automatically be appended with the ".mgp" extension (case sensitive)
    /// if it is not specified.
    ///
    /// \param packageDescription
    /// The description of the package.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgDuplicateFileException
    /// \exception MgPathTooLongException
    /// \exception MgFileIoException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgOutOfMemoryException
    ///
    void MakePackage(MgResourceIdentifier* resource, CREFSTRING packageName,
        CREFSTRING packageDescription);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the maximum size in kilobytes for the log files.  When the maximum
    /// size is exceeded, the current log will be archived, and a new log will be created.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void SetMaximumLogSize(INT32 size);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the delimiter to use for separating the data fields in the logs.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void SetLogDelimiter(CREFSTRING delimiter);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enables/disables maximum log file size restriction.  Maximum log file size is
    /// set with MgServerAdmin::SetMaximumLogSize().
    ///
    /// \param useMaxSize
    /// Log file size restriction is enforced if useMaxSize = true.
    /// using EnumeratePackages().
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void EnableMaximumLogSize(bool useMaxSize);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if log file size is restricted to the maximum size
    /// set with MgServerAdmin::SetMaximumLogSize().
    ///
    /// \return
    /// The status of the use of the maximum log size restriction
    ///
    /// \exception MgConnectionNotOpenException
    ///
    bool IsMaximumLogSizeEnabled();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the contents of the specified document.
    ///
    /// \param identifier
    /// The document to retrieve.
    ///
    /// \return
    /// The document contents in a ByteReader.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullReferenceException
    /// \exception MgOutOfMemoryException
    ///
    MgByteReader* GetDocument(CREFSTRING identifier);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the contents of the specified document.
    ///
    /// \param identifier
    /// The document to set.
    /// \param data
    /// The data to set the document contents to.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    /// \exception MgNullReferenceException
    /// \exception MgOutOfMemoryException
    ///
    void SetDocument(CREFSTRING identifier, MgByteReader* data);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Registers services on the specified servers.
    ///
    /// \param serverInfoList
    /// List of server information such as name, description, IP address, and
    /// available services.
    ///
    /// \return
    /// List of server information from all other servers within a site.
    /// The returned list may be NULL.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    ///
    ///

    MgSerializableCollection* RegisterServicesOnServers(
        MgSerializableCollection* serverInfoList);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Un-registers services on the specified servers.
    ///
    /// \param serverInfoList
    /// List of server information such as name, description, IP address, and
    /// available services.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    ///
    void UnregisterServicesOnServers(MgSerializableCollection* serverInfoList);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Send a resource change notification.
    ///
    /// \param resources
    /// MgResourceIdentifier list of changed resources.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    ///
    void NotifyResourcesChanged(MgSerializableCollection* resources);

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId(); // { return m_cls_id; }

    /// Unimplemented Constructors/Methods
    MgServerAdmin(const MgServerAdmin& object);
    MgServerAdmin& operator=(const MgServerAdmin& object);

private:

    void SetWarning(MgWarnings* warning);

    Ptr<MgWarnings> m_warnings;
    Ptr<MgConnectionProperties> m_connProp;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_ServerAdmin;

};
/// \endcond

#endif // MGSERVERADMIN_H_
