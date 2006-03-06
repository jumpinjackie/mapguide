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
#include "PackageStatusLogManager.h"

MgPackageStatusLogManager::MgPackageStatusLogManager()
{
}

MgPackageStatusLogManager::MgPackageStatusLogManager(CREFSTRING status, CREFSTRING packageName, CREFSTRING date, INT32 size) :
    m_packageSize(size),
    m_packageError(L"")
{
    MG_TRY()

    if (status.empty() || packageName.empty() || date.empty())
    {
        throw new MgNullArgumentException(L"MgPackageStatusLogManager.MgPackageStatusLogManager", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_packageStatus = status;
    m_packageName = packageName;
    m_packageDate = date;

    m_entries = new MgStringCollection();
    if (m_entries == NULL)
    {
        throw new MgOutOfMemoryException(L"MgPackageStatusLogManager.MgPackageStatusLogManager", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // If package is blah.mgp, log is blah.mgp.log
    m_logName = m_packageName + MgFileExtensions::Log;

    UpdateLog();

    MG_CATCH_AND_THROW(L"MgPackageStatusLogManager.MgPackageStatusLogManager")
}

MgPackageStatusLogManager::~MgPackageStatusLogManager()
{
}

void MgPackageStatusLogManager::Dispose()
{
    delete this;
}

void MgPackageStatusLogManager::SetStatus(CREFSTRING status)
{
    m_packageStatus = status;
}

void MgPackageStatusLogManager::SetError(CREFSTRING error)
{
    m_packageError = error;
}

void MgPackageStatusLogManager::SetErrorDetails(CREFSTRING details)
{
    m_packageErrorDetails = details;
}

void MgPackageStatusLogManager::AddDetailsEntry(CREFSTRING entry)
{
    MG_TRY()

    STRING timeStampEntry;
    timeStampEntry = GetTimeStamp();
    timeStampEntry += L" " + entry;

    m_entries->Add(timeStampEntry);

    // Do not want failure to log information to
    // result in load package failing
    MG_CATCH(L"MgPackageStatusLogManager.AddDetailsEntry")
}

void MgPackageStatusLogManager::UpdateLog()
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    FILE* file = NULL;

    MG_TRY()

    file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(m_logName), ACE_TEXT("wb"));
    if (file == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(m_logName);
        throw new MgLogOpenFailedException(L"MgPackageStatusLogManager.UpdateLog", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    STRING entry;

    //----------------------------------------------------------
    // Write status with timestamp of when this file was updated
    entry = MgUtil::GetResourceMessage(MgResources::ResourceService, L"MgPackageStatusLogManagerStatus");
    if (entry.empty())
    {
        entry = MgResources::MgPackageStatusLogManagerStatus;
    }
    WriteLineToFile(entry, file);

    entry = GetTimeStamp();
    entry += L" " + m_packageStatus;
    WriteLineToFile(entry, file);
    //----------------------------------------------------------

    WriteLineToFile(L"", file);

    //----------------------------------------------------------
    // Write out the package information
    entry = MgUtil::GetResourceMessage(MgResources::ResourceService, L"MgPackageStatusLogManagerPackageInfo");
    if (entry.empty())
    {
        entry = MgResources::MgPackageStatusLogManagerPackageInfo;
    }
    WriteLineToFile(entry, file);

    WriteLineToFile(m_packageName, file); // File name of the package
    WriteLineToFile(m_packageDate, file); // Date the package was last modified

    wchar_t buffer[512];
    ACE_OS::itoa(m_packageSize, buffer, 10);
    STRING size = buffer;
    WriteLineToFile(size, file); // Size of the file
    //----------------------------------------------------------


    //----------------------------------------------------------
    // Write out error if there was one
    if (!m_packageError.empty())
    {
        entry = MgUtil::GetResourceMessage(MgResources::ResourceService, L"MgPackageStatusLogManagerError");
        if (entry.empty())
        {
            entry = MgResources::MgPackageStatusLogManagerError;
        }
        WriteLineToFile(entry, file);

        WriteLineToFile(L"", file);
        WriteLineToFile(m_packageError, file);
    }
    // Write out error details if they are given
    if (!m_packageErrorDetails.empty())
    {
#ifdef _WIN32
        entry = MgUtil::ReplaceString(m_packageErrorDetails, L"\n", L"\r\n");
#else
        entry = m_packageErrorDetails.c_str();
#endif
        ACE_OS::fputs(MG_WCHAR_TO_TCHAR(entry), file);
    }
    //----------------------------------------------------------

    WriteLineToFile(L"", file);

    //----------------------------------------------------------
    // Write out the details
    entry = MgUtil::GetResourceMessage(MgResources::ResourceService, L"MgPackageStatusLogManagerDetails");
    if (entry.empty())
    {
        entry = MgResources::MgPackageStatusLogManagerDetails;
    }
    WriteLineToFile(entry, file);

    INT32 i;
    for (i = 0; i < m_entries->GetCount(); i++)
    {
        entry = m_entries->GetItem(i);
        WriteLineToFile(entry, file);
    }
    //----------------------------------------------------------

    ACE_OS::fclose(file);
    file = NULL;

    MG_CATCH(L"MgPackageStatusLogManager.UpdateLog")
    if (mgException != NULL)
    {
        if (file != NULL)
        {
            ACE_OS::fclose(file);
            file = NULL;
        }
        (*mgException).AddRef();
        mgException->Raise();
    }
}

STRING MgPackageStatusLogManager::GetTimeStamp()
{
    ACE_TCHAR day_and_time[35];
    ACE::timestamp(day_and_time, sizeof day_and_time);

    return MG_TCHAR_TO_WCHAR(day_and_time);
}

void MgPackageStatusLogManager::WriteLineToFile(CREFSTRING entry, FILE* file)
{
    STRING line;
    line += entry.c_str();
#ifdef _WIN32
    line += L"\r\n";
#else
    line += L"\n";
#endif
    fputs(MgUtil::WideCharToMultiByte(line).c_str(), file);
}
