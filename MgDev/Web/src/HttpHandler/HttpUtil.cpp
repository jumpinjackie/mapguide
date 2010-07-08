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

#include "HttpHandler.h"
#include "HttpUtil.h"

void MgHttpUtil::LogException(MgException* exception)
{
    if(NULL != exception)
    {
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance()));

        MgConfiguration* cfg = MgConfiguration::GetInstance();

        // Is log enabled?
        bool bLogEnabled = false;
        cfg->GetBoolValue(MgConfigProperties::AgentPropertiesSection, MgConfigProperties::AgentErrorLogEnabled, bLogEnabled, MgConfigProperties::DefaultAgentErrorLogEnabled);

        if(bLogEnabled)
        {
            // Get the logs path
            STRING path = L"";
            cfg->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyLogsPath, path, MgConfigProperties::DefaultGeneralPropertyLogsPath);

            // Check if path ends with a '/' if not, add one if needed
            MgFileUtil::AppendSlashToEndOfPath(path);

            STRING filename = L"";
            cfg->GetStringValue(MgConfigProperties::AgentPropertiesSection, MgConfigProperties::AgentErrorLogFilename, filename, MgConfigProperties::DefaultAgentErrorLogFilename);
            filename = path + filename;

            FILE* fp = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("a+"));
            if (fp)
            {
                MgDateTime currentTime;
                STRING strCurrentTime = currentTime.ToXmlString(false);

                STRING message = exception->GetExceptionMessage();
                STRING stackTrace = exception->GetStackTrace();
                ACE_OS::fprintf(fp, ACE_TEXT("<%s>\n"), MG_WCHAR_TO_TCHAR(strCurrentTime));
                ACE_OS::fprintf(fp, ACE_TEXT(" Error: %s\n"), MG_WCHAR_TO_TCHAR(message));

                // Add the stack trace
                // Do not log empty stack traces
                if (!stackTrace.empty())
                {
                    STRING entry = L"";
                    entry += L" " + MgResources::StackTrace + L":";

                    size_t size = stackTrace.size();

                    if (size > 0)
                    {
                        STRING trace = stackTrace.c_str();

                        // Get rid of extra \n at the end of the stack trace
                        if (trace[size-1] == L'\n')
                        {
                            trace.erase((size-1), 1);
                        }

                        // Make it look "pretty"
                        trace = MgUtil::ReplaceString(trace, L"\n", L"\n  ");
                        entry += L"\n  " + trace + L"\n";
                    }

                    ACE_OS::fprintf(fp, ACE_TEXT("%s"), MG_WCHAR_TO_TCHAR(entry));
                }

                ACE_OS::fclose(fp);
            }
        }
    }
}
