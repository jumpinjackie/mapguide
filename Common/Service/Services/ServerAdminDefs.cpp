//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "ServerAdminDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Log file types
///
const STRING MgLogFileType::Access          = L"AccessLog";
const STRING MgLogFileType::Admin           = L"AdminLog";
const STRING MgLogFileType::Authentication  = L"AuthenticationLog";
const STRING MgLogFileType::Error           = L"ErrorLog";
const STRING MgLogFileType::Session         = L"SessionLog";
const STRING MgLogFileType::Trace           = L"TraceLog";


///////////////////////////////////////////////////////////////////////////////
/// Types of package status return codes
///
const STRING MgPackageStatus::Success       = L"SUCCESS";
const STRING MgPackageStatus::Failed        = L"FAILED";
const STRING MgPackageStatus::Loading       = L"LOADING";
const STRING MgPackageStatus::NotStarted    = L"NOTSTARTED";
const STRING MgPackageStatus::Unknown       = L"UNKNOWN";


///////////////////////////////////////////////////////////////////////////////
/// Types of file extensions
///
const STRING MgFileExtensions::Package      = L".mgp";
const STRING MgFileExtensions::Log          = L".log";


///////////////////////////////////////////////////////////////////////////////
/// Names of server information properties
///
const STRING MgServerInformationProperties::ServerVersion               = L"ServerVersion";
const STRING MgServerInformationProperties::AdminOperationsQueueCount   = L"AdminOperationsQueueCount";
const STRING MgServerInformationProperties::ClientOperationsQueueCount  = L"ClientOperationsQueueCount";
const STRING MgServerInformationProperties::SiteOperationsQueueCount    = L"SiteOperationsQueueCount";
const STRING MgServerInformationProperties::AverageOperationTime        = L"AverageOperationTime";
const STRING MgServerInformationProperties::TotalOperationTime          = L"TotalOperationTime";
const STRING MgServerInformationProperties::CpuUtilization              = L"CpuUtilization";
const STRING MgServerInformationProperties::TotalPhysicalMemory         = L"TotalPhysicalMemory";
const STRING MgServerInformationProperties::AvailablePhysicalMemory     = L"AvailablePhysicalMemory";
const STRING MgServerInformationProperties::TotalVirtualMemory          = L"TotalVirtualMemory";
const STRING MgServerInformationProperties::AvailableVirtualMemory      = L"AvailableVirtualMemory";
const STRING MgServerInformationProperties::Uptime                      = L"Uptime";
const STRING MgServerInformationProperties::Status                      = L"Status";
const STRING MgServerInformationProperties::DisplayName                 = L"DisplayName";
const STRING MgServerInformationProperties::TotalReceivedOperations     = L"TotalReceivedOperations";
const STRING MgServerInformationProperties::TotalProcessedOperations    = L"TotalProcessedOperations";
const STRING MgServerInformationProperties::TotalConnections            = L"TotalConnections";
const STRING MgServerInformationProperties::TotalActiveConnections      = L"TotalActiveConnections";
const STRING MgServerInformationProperties::OperatingSystemVersion      = L"OperatingSystemVersion";
