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

#ifndef MGSERVERADMINDEFS_H_
#define MGSERVERADMINDEFS_H_

/// \cond INTERNAL

//////////////////////////////////////////////////////////////////
/// \brief
/// Types of log files
class MG_MAPGUIDE_API MgLogFileType
{
EXTERNAL_API:
    /// Logs all requests to the MapGuide server.  A record is created
    /// for each request after it has been completed.
    static const STRING Access;         /// value("AccessLog")

    /// Logs all administrative operations (server status changes, configuration changes, etc.).
    static const STRING Admin;          /// value("AdminLog")

    /// Logs the success or failure of all authentication operations.
    static const STRING Authentication; /// value("AuthenticationLog")

    /// Logs all errors that occur during the MapGuide server's operation.
    static const STRING Error;          /// value("ErrorLog")

    /// Logs state information for each connection to the MapGuide server,
    /// such as connection ID, connection, duration, user name, operations received,
    /// and operations processed.
    static const STRING Session;        /// value("SessionLog")

    /// Logs the details for each request (logged in the access log).  For example,
    /// each request can include data from several maps or feature sets, and a detailed
    /// record would be created for each one.
    static const STRING Trace;          /// value("TraceLog")
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Package API names
///
class MG_MAPGUIDE_API MgPackageApiName
{
INTERNAL_API:
    static const STRING LoadPackage;    /// value("LoadPackage")
    static const STRING MakePackage;    /// value("MakePackage")
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Status codes for packages
///
class MG_MAPGUIDE_API MgPackageStatusCode
{
EXTERNAL_API:
    /// Indicates a package was successfully loaded/made.
    static const STRING Succeeded;      /// value("Succeeded")

    /// Indicates an error occurred while loading/making the package.
    static const STRING Failed;         /// value("Failed")

    /// Indicates a package is currently being loaded/made.
    static const STRING InProgress;     /// value("InProgress")

    /// Indicates the package has not started.
    static const STRING NotStarted;     /// value("NotStarted")

    /// Indicates the status of the package is not known.
    static const STRING Unknown;        /// value("Unknown")
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Extensions for file types
///
class MG_MAPGUIDE_API MgFileExtension
{
INTERNAL_API:
    static const STRING Log;            /// Extension for log files
    static const STRING Mgp;            /// Extension for resource package files
    static const STRING Xml;            /// Extension for XML files
};


//////////////////////////////////////////////////////////////////
/// \brief
/// ServerAdmin properties.  Note that this is not a complete list of the properties which may be set
/// for a server.  Values stored in the server's config file are also properties of the server.  These
/// properties are defined in MgConfigProperties.h.
class MG_MAPGUIDE_API MgServerInformationProperties
{
EXTERNAL_API:

    /// INFO PROPERTIES SECTION ----------------------------------------------------------------------------------------

    /// The server's version
    static const STRING ServerVersion;              /// value("ServerVersion")

    /// The API version
    static const STRING ApiVersion;                 /// value("ApiVersion")

    /// STATISTIC PROPERTIES SECTION -----------------------------------------------------------------------------------

    /// Gets the # of operations in the administrative queue
    static const STRING AdminOperationsQueueCount;  /// value("AdminOperationsQueueCount")

    /// Gets the # of operations in the client queue
    static const STRING ClientOperationsQueueCount; /// value("ClientOperationsQueueCount")

    /// Gets the # of operations in the site queue
    static const STRING SiteOperationsQueueCount;   /// value("SiteOperationsQueueCount")

    /// Gets the average time to process an operation
    static const STRING AverageOperationTime;       /// value("AverageOperationTime")

    /// Gets the total amount of time used by the Server to process operations
    static const STRING TotalOperationTime;         /// value("TotalOperationTime")

    /// Gets the current CPU utilization in percent
    static const STRING CpuUtilization;             /// value("CpuUtilization")

    /// Gets the total physical memory in bytes
    static const STRING TotalPhysicalMemory;        /// value("TotalPhysicalMemory")

    /// Gets the available physical memory in bytes
    static const STRING AvailablePhysicalMemory;    /// value("AvailablePhysicalMemory")

    /// Gets the total virtual memory in bytes
    static const STRING TotalVirtualMemory;         /// value("TotalVirtualMemory")

    /// Gets the available virtual memory in bytes
    static const STRING AvailableVirtualMemory;     /// value("AvailableVirtualMemory")

    /// Gets the server uptime
    static const STRING Uptime;                     /// value("Uptime")

    /// Gets the server online/offline status
    static const STRING Status;                     /// value("Status")

    /// Gets the server display name
    static const STRING DisplayName;                /// value("DisplayName")

    /// Gets the server machine Ip
    static const STRING MachineIp;                  /// value("MachineIp")

    /// Gets the total operations received
    static const STRING TotalReceivedOperations;    /// value("TotalReceivedOperations")

    /// Gets the total operations processed
    static const STRING TotalProcessedOperations;   /// value("TotalProcessedOperations")

    /// Gets the total # of connections created on the server
    static const STRING TotalConnections;           /// value("TotalConnections")

    /// Gets the total # of active connections on the server
    static const STRING TotalActiveConnections;     /// value("TotalActiveConnections")

    /// Gets the operating system version
    static const STRING OperatingSystemVersion;     /// value("OperatingSystemVersion")

    /// Gets the process working set memory
    static const STRING WorkingSet;                 /// value("WorkingSet")

    /// Gets the process virtual memory
    static const STRING VirtualMemory;              /// value("VirtualMemory")

    /// Gets the current size of the server cache
    static const STRING CacheSize;                  /// value("CacheSize")

    /// Gets the number of dropped cache entries
    static const STRING CacheDroppedEntries;        /// value("CacheDroppedEntries")
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for ServerAdmin Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgServerAdminServiceOpId
{
INTERNAL_API:
    static const int GetConfigurationProperties     = 0x1111EA01;
    static const int SetConfigurationProperties     = 0x1111EA02;
    static const int ClearLog                       = 0x1111EA03;
    static const int GetLog                         = 0x1111EA04;
    static const int BringOnline                    = 0x1111EA05;
    static const int TakeOffline                    = 0x1111EA06;
    static const int IsOnline                       = 0x1111EA07;
    static const int GetInformationProperties       = 0x1111EA08;
    static const int RegisterServicesOnServers      = 0x1111EA09;
    static const int UnregisterServicesOnServers    = 0x1111EA0A;
    static const int EnumerateLogs                  = 0x1111EA0B;
    static const int RenameLog                      = 0x1111EA0C;
    static const int DeleteLog                      = 0x1111EA0D;
    static const int DeletePackage                  = 0x1111EA0E;
    static const int EnumeratePackages              = 0x1111EA0F;
    static const int GetPackageLog                  = 0x1111EA10;
    static const int GetPackageStatus               = 0x1111EA11;
    static const int LoadPackage                    = 0x1111EA12;
    static const int GetLogFile                     = 0x1111EA13;
    static const int SetMaximumLogSize              = 0x1111EA14;
    static const int SetLogsDelimiter               = 0x1111EA15;
    static const int EnableMaximumLogSize           = 0x1111EA16;
    static const int IsMaximumLogSizeEnabled        = 0x1111EA17;
    static const int GetDocument                    = 0x1111EA18;
    static const int SetDocument                    = 0x1111EA19;
    static const int NotifyResourcesChanged         = 0x1111EA1A;
    static const int MakePackage                    = 0x1111EA1B;
    static const int RemoveConfigurationProperties  = 0x1111EA1C;
    static const int GetSiteVersion                 = 0x1111EA1D;
    static const int GetSiteStatus                  = 0x1111EA1E;
};

/// \endcond

#endif
