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
#include "PackageLogReader.h"

static const int MG_PACKAGE_LOG_MAX_ROWS    = 256;
static const int MG_PACKAGE_LOG_BUFFER_SIZE = 1024;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPackageLogReader::MgPackageLogReader(CREFSTRING packagePathname) :
    MgPackageLogHandler(packagePathname)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPackageLogReader::~MgPackageLogReader()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets an entry from the log file.
///
bool MgPackageLogReader::SetEntry(CREFSTRING name, CREFSTRING value)
{
    bool keepReading = true;

    if (MgPackageLogEntry::ApiName == name)
    {
        m_statusInfo.SetApiName(value);
    }
    else if (MgPackageLogEntry::StatusCode == name)
    {
        m_statusInfo.SetStatusCode(value);
    }
    else if (MgPackageLogEntry::PackageName == name)
    {
        m_statusInfo.SetPackageName(value);
    }
    else if (MgPackageLogEntry::PackageDescription == name)
    {
        m_statusInfo.SetPackageDescription(value);
    }
    else if (MgPackageLogEntry::PackageDate == name)
    {
        Ptr<MgDateTime> packageDate = new MgDateTime(
            MgUtil::WideCharToMultiByte(value));
        m_statusInfo.SetPackageDate(packageDate);
    }
    else if (MgPackageLogEntry::PackageSize == name)
    {
        m_statusInfo.SetPackageSize(MgUtil::StringToInt64(value));
    }
    else if (MgPackageLogEntry::UserName == name)
    {
        m_statusInfo.SetUserName(value);
    }
    else if (MgPackageLogEntry::ServerName == name)
    {
        m_statusInfo.SetServerName(value);
    }
    else if (MgPackageLogEntry::ServerAddress == name)
    {
        m_statusInfo.SetServerAddress(value);
    }
    else if (MgPackageLogEntry::StartTime == name)
    {
        Ptr<MgDateTime> startTime = new MgDateTime(
            MgUtil::WideCharToMultiByte(value));
        m_statusInfo.SetStartTime(startTime);
    }
    else if (MgPackageLogEntry::EndTime == name)
    {
        Ptr<MgDateTime> endTime = new MgDateTime(
            MgUtil::WideCharToMultiByte(value));
        m_statusInfo.SetEndTime(endTime);
    }
    else if (MgPackageLogEntry::OperationsFailed == name)
    {
        m_statusInfo.SetOperationsFailed(MgUtil::StringToInt32(value));
    }
    else if (MgPackageLogEntry::OperationsReceived == name)
    {
        m_statusInfo.SetOperationsReceived(MgUtil::StringToInt32(value));
    }
    else if (MgPackageLogEntry::AverageOperationTime == name)
    {
        // Do nothing as this value is computed at runtime.
    }
    else if (MgPackageLogEntry::ErrorMessage == name)
    {
        m_statusInfo.SetErrorMessage(value);
    }
    else if (MgPackageLogEntry::StackTrace == name)
    {
        m_statusInfo.SetStackTrace(value);
    }
    else if (MgPackageLogEntry::OperationDetails == name) // end of status information
    {
        keepReading = false;
    }
    else if (name.empty())  // empty line?
    {
        if (!value.empty()) // invalid entry
        {
            throw new MgFileIoException(
                L"MgPackageLogReader.SetEntry",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else if (STRING::npos != name.find_first_of(sm_reservedCharacters)) // timestamped entry
    {
        keepReading = false;
    }
    else
    {
        // Do nothing as this is possibly an unknown entry from newer log file versions.
    }

    return keepReading;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Reads the package status from the log file.
///
void MgPackageLogReader::ReadStatus()
{
    FILE* file = NULL;
    STRING actualPackageName;
    Ptr<MgDateTime> actualPackageDate;
    INT64 actualPackageSize = 0;

    MG_TRY()

    if (!MgFileUtil::PathnameExists(m_packagePathname))
    {
        MgStringCollection arguments;
        arguments.Add(m_packagePathname);

        throw new MgFileNotFoundException(
            L"MgPackageLogReader.ReadStatus",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Get the actual package modification time and size.
    actualPackageName = MgPackageManager::GetPackageName(m_packagePathname);
    actualPackageDate = new MgDateTime(MgFileUtil::GetFileModificationTime(m_packagePathname));
    actualPackageSize = MgFileUtil::GetFileSize(m_packagePathname);

    if (!MgFileUtil::PathnameExists(m_logPathname))
    {
        MgStringCollection arguments;
        arguments.Add(m_logPathname);

        throw new MgFileNotFoundException(
            L"MgPackageLogReader.ReadStatus",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // If a log file exists, the status should be recorded within it.
    file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(m_logPathname), ACE_TEXT("rb"));

    if (NULL == file)
    {
        MgStringCollection arguments;
        arguments.Add(m_logPathname);

        throw new MgLogOpenFailedException(
            L"MgPackageLogReader.GetLogInformation",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    char buf[MG_PACKAGE_LOG_BUFFER_SIZE];
    STRING str;
    STRING name, value;
    INT32 columnCount = 0;
    INT32 rowCount = 0;
    bool keepReading = true;

    do
    {
        ::memset(buf, 0, MG_PACKAGE_LOG_BUFFER_SIZE);
        ACE_OS::fread(buf, sizeof(char), (MG_PACKAGE_LOG_BUFFER_SIZE-1)*sizeof(char), file);

        str.clear();
        MgUtil::MultiByteToWideChar(string(buf), str);

        size_t length = str.length();

        if (0 == length)
        {
            keepReading = false;
            break;
        }

        for (size_t i = 0; i < length; ++i)
        {
            if (L'\t' == str[i])                            // end of column
            {
                ACE_ASSERT(value.empty());
                ++columnCount;
            }
            else if (L'\r' == str[i] || L'\n' == str[i])    // end of row
            {
                keepReading = SetEntry(name, value);

                if (keepReading)
                {
                    // Reset the entry name and value pair.
                    name.clear();
                    value.clear();
                    columnCount = 0;
                    ++rowCount;

                    if (rowCount > MG_PACKAGE_LOG_MAX_ROWS) // invalid file format?
                    {
                        keepReading = false;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            else if (0 == columnCount)
            {
                name += str[i];
            }
            else if (1 == columnCount)
            {
                value += str[i];
            }
            else
            {
                throw new MgFileIoException(
                    L"MgPackageLogReader.ReadStatus",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    } while (keepReading);

    // Validate the current package status information.
    STRING packageName = m_statusInfo.GetPackageName();
    Ptr<MgDateTime> packageDate = m_statusInfo.GetPackageDate();
    INT64 packageSize = m_statusInfo.GetPackageSize();

    if (packageName != actualPackageName
     || packageSize != actualPackageSize
     || packageDate == NULL
     || actualPackageDate == NULL
     || *packageDate != *actualPackageDate)
    {
        throw new MgInvalidLogEntryException(
            L"MgPackageLogReader.ReadStatus",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH(L"MgPackageLogReader.ReadStatus")

    if (NULL != file)
    {
        ACE_OS::fclose(file);
    }

    if (mgException != NULL)
    {
        // Reset the status with actual/useful information if possible.
        m_statusInfo.SetStatusCode(MgPackageStatusCode::Unknown);
        m_statusInfo.SetPackageName(actualPackageName);
        m_statusInfo.SetPackageSize(actualPackageSize);

        if (actualPackageDate != NULL)
        {
            m_statusInfo.SetPackageDate(actualPackageDate);
        }

        // Remove the invalid log file.
        MgFileUtil::DeleteFile(m_logPathname);
    }

    MG_THROW()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the package status information.
///
MgPackageStatusInformation* MgPackageLogReader::GetStatus()
{
    Ptr<MgPackageStatusInformation> statusInfo;

    MG_TRY()

    // Read the status information to verify if the log file content is valid.
    ReadStatus();

    // If something went wrong with the process, exception will be ignored and
    // the last determined status will be returned.
    MG_CATCH_AND_RELEASE();

    statusInfo = new MgPackageStatusInformation(m_statusInfo);

    return statusInfo.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the package log file.
///
MgByteReader* MgPackageLogReader::GetLog()
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    // Read the status information to verify if the log file content is valid.
    ReadStatus();

    Ptr<MgByteSource> byteSource = new MgByteSource(m_logPathname);

    byteSource->SetMimeType(MgMimeType::Text);
    byteReader = byteSource->GetReader();

    MG_CATCH_AND_THROW(L"MgPackageLogReader.GetLog");

    return byteReader.Detach();
}
