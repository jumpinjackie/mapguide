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

#include "MapGuideCommon.h"
#include "PackageLogWriter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPackageLogWriter::MgPackageLogWriter(CREFSTRING packageApiName,
    CREFSTRING packagePathname) :
    MgPackageLogHandler(packagePathname)
{
    m_statusInfo.SetApiName(packageApiName);
    m_operations.reserve(256);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPackageLogWriter::~MgPackageLogWriter()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the error based on the specified exception.
///
void MgPackageLogWriter::SetError(MgException* except)
{
    if (except == NULL)
    {
        m_statusInfo.SetStatusCode(MgPackageStatusCode::Succeeded);
        m_statusInfo.SetErrorMessage(L"");
        m_statusInfo.SetStackTrace(L"");
    }
    else
    {
        m_statusInfo.SetStatusCode(MgPackageStatusCode::Failed);
        m_statusInfo.SetErrorMessage(except->GetMessage());
        m_statusInfo.SetStackTrace(except->GetStackTrace());
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds an operation to the collection.
///
void MgPackageLogWriter::AddOperation(CREFSTRING operation)
{
    STRING entry;

    entry += L"<";
    entry += GetCurrentTimestamp();
    entry += L">";
    entry += sm_columnDelimiter;
    entry += operation;
    entry += sm_rowDelimiter;

    m_operations.push_back(entry);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates the package log file.
///
void MgPackageLogWriter::UpdateLog()
{
    FILE* file = NULL;

    MG_TRY()

    file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(m_logPathname), ACE_TEXT("wb"));

    if (NULL == file)
    {
        MgStringCollection arguments;
        arguments.Add(m_logPathname);

        throw new MgLogOpenFailedException(
            L"MgPackageLogWriter.UpdateLog",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    Ptr<MgDateTime> packageDate = m_statusInfo.GetPackageDate();
    Ptr<MgDateTime> startTime   = m_statusInfo.GetStartTime();
    Ptr<MgDateTime> endTime     = m_statusInfo.GetEndTime();

    WriteEntry(file, MgPackageLogEntry::ApiName, m_statusInfo.GetApiName());
    WriteEntry(file, MgPackageLogEntry::StatusCode, m_statusInfo.GetStatusCode());
    WriteEntry(file, MgPackageLogEntry::PackageName, m_statusInfo.GetPackageName());
    WriteEntry(file, MgPackageLogEntry::PackageDescription, m_statusInfo.GetPackageDescription(), true);
    WriteEntry(file, MgPackageLogEntry::PackageDate, packageDate);
    WriteEntry(file, MgPackageLogEntry::PackageSize, m_statusInfo.GetPackageSize());
    WriteEntry(file, MgPackageLogEntry::UserName, m_statusInfo.GetUserName());
    WriteEntry(file, MgPackageLogEntry::ServerName, m_statusInfo.GetServerName());
    WriteEntry(file, MgPackageLogEntry::ServerAddress, m_statusInfo.GetServerAddress());
    WriteEntry(file, MgPackageLogEntry::StartTime, startTime);
    WriteEntry(file, MgPackageLogEntry::EndTime, endTime);

    if (m_statusInfo.GetOperationsReceived() > 0)
    {
        WriteEntry(file, MgPackageLogEntry::OperationsFailed, m_statusInfo.GetOperationsFailed());
        WriteEntry(file, MgPackageLogEntry::OperationsReceived, m_statusInfo.GetOperationsReceived());
        WriteEntry(file, MgPackageLogEntry::AverageOperationTime, m_statusInfo.GetAverageOperationTime());
    }

    if (!m_statusInfo.GetErrorMessage().empty())
    {
        WriteEntry(file, MgPackageLogEntry::ErrorMessage, m_statusInfo.GetErrorMessage(), true);
        WriteEntry(file, MgPackageLogEntry::StackTrace, m_statusInfo.GetStackTrace(), true);
    }

    if (m_operations.size() > 0)
    {
        WriteEntry(file, MgPackageLogEntry::OperationDetails, L"");

        WriteLine(file);

        for (size_t i = 0; i < m_operations.size(); ++i)
        {
            ::fputs(MgUtil::WideCharToMultiByte(m_operations[i]).c_str(), file);
        }
    }

    MG_CATCH(L"MgPackageLogWriter.UpdateLog")

    if (NULL != file)
    {
        ACE_OS::fclose(file);
    }

    MG_THROW()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes a string to the log file.
///
void MgPackageLogWriter::WriteEntry(FILE* file, CREFSTRING name,
    CREFSTRING value, bool replaceReservedCharacters)
{
    ACE_ASSERT(NULL != file);

    STRING entry = name;
    entry += sm_columnDelimiter;

    if (replaceReservedCharacters)
    {
        STRING str = value;

        ReplaceReservedCharacters(str);
        entry += str;
    }
    else
    {
        entry += value;
    }

    entry += sm_rowDelimiter;

    ::fputs(MgUtil::WideCharToMultiByte(entry).c_str(), file);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes an INT32 value to the log file.
///
void MgPackageLogWriter::WriteEntry(FILE* file, CREFSTRING name,
    INT32 value)
{
    STRING str;

    MgUtil::Int32ToString(value, str);
    WriteEntry(file, name, str);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes an INT64 value to the log file.
///
void MgPackageLogWriter::WriteEntry(FILE* file, CREFSTRING name,
    INT64 value)
{
    STRING str;

    MgUtil::Int64ToString(value, str);
    WriteEntry(file, name, str);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes a double value to the log file.
///
void MgPackageLogWriter::WriteEntry(FILE* file, CREFSTRING name,
    double value)
{
    STRING str;

    MgUtil::DoubleToString(value, str);
    WriteEntry(file, name, str);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes a date time to the log file.
///
void MgPackageLogWriter::WriteEntry(FILE* file, CREFSTRING name,
    MgDateTime* value)
{
    if (NULL == value)
    {
        throw new MgNullArgumentException(
            L"MgPackageLogWriter.WriteEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    WriteEntry(file, name, value->ToXmlString(false));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Writes a line to the log file.
///
void MgPackageLogWriter::WriteLine(FILE* file)
{
    WriteEntry(file, L"", L"");
}
