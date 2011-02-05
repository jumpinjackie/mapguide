//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "PackageLogHandler.h"
#include "PackageManager.h"

const STRING MgPackageLogEntry::ApiName                 = L"API_NAME:";
const STRING MgPackageLogEntry::StatusCode              = L"STATUS_CODE:";
const STRING MgPackageLogEntry::PackageName             = L"PACKAGE_NAME:";
const STRING MgPackageLogEntry::PackageDescription      = L"PACKAGE_DESC:";
const STRING MgPackageLogEntry::PackageDate             = L"PACKAGE_DATE:";
const STRING MgPackageLogEntry::PackageSize             = L"PACKAGE_SIZE:";
const STRING MgPackageLogEntry::UserName                = L"USER_NAME:";
const STRING MgPackageLogEntry::ServerName              = L"SERVER_NAME:";
const STRING MgPackageLogEntry::ServerAddress           = L"SERVER_ADDR:";
const STRING MgPackageLogEntry::StartTime               = L"START_TIME:";
const STRING MgPackageLogEntry::EndTime                 = L"END_TIME:";
const STRING MgPackageLogEntry::OperationsFailed        = L"OPS_FAILED:";
const STRING MgPackageLogEntry::OperationsReceived      = L"OPS_RECEIVED:";
const STRING MgPackageLogEntry::AverageOperationTime    = L"AVRG_OP_TIME:";
const STRING MgPackageLogEntry::ErrorMessage            = L"ERROR_MESSAGE:";
const STRING MgPackageLogEntry::StackTrace              = L"STACK_TRACE:";
const STRING MgPackageLogEntry::OperationDetails        = L"OP_DETAILS:";

const STRING MgPackageLogHandler::sm_columnDelimiter    = L"\t";
#ifdef _WIN32
const STRING MgPackageLogHandler::sm_rowDelimiter       = L"\r\n";
#else
const STRING MgPackageLogHandler::sm_rowDelimiter       = L"\n";
#endif
const STRING MgPackageLogHandler::sm_reservedCharacters = L"<>";

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPackageLogHandler::MgPackageLogHandler(CREFSTRING packagePathname) :
    m_packagePathname(packagePathname)
{
    MG_TRY()

    if (m_packagePathname.empty())
    {
        throw new MgNullArgumentException(
            L"MgPackageLogHandler.MgPackageLogHandler",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_logPathname = MgPackageManager::GetLogPathname(m_packagePathname);

    m_statusInfo.SetPackageName(MgPackageManager::GetPackageName(m_packagePathname));
    m_statusInfo.SetStatusCode(MgPackageStatusCode::InProgress);

    MG_CATCH_AND_THROW(L"MgPackageLogHandler.MgPackageLogHandler")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPackageLogHandler::~MgPackageLogHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgPackageLogHandler::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the current time that conforms to the XML standard.
///
STRING MgPackageLogHandler::GetCurrentTimestamp() const
{
    MgDateTime currentTime;

    return currentTime.ToXmlString(false);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Replaces reserved characters in the specified string with spaces.
///
void MgPackageLogHandler::ReplaceReservedCharacters(REFSTRING str)
{
    str = MgUtil::ReplaceString(str, L"\t", L" ");
    str = MgUtil::ReplaceString(str, L"\r\n", L" ");
    str = MgUtil::ReplaceString(str, L"\n", L" ");
}
