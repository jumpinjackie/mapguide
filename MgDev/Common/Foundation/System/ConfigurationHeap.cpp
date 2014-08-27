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

#include "Foundation.h"

MgConfigurationHeap::MgConfigurationHeap()
{
    m_rootSection = new MgConfigurationSection();
    m_sectionCollection = new MgConfigurationSectionCollection();
}


MgConfigurationHeap::~MgConfigurationHeap()
{
}


bool MgConfigurationHeap::ImportConfig(CREFSTRING fileName)
{
    if (fileName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgConfigImpExp.ImportConfig",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

#ifdef _WIN32
    FILE* inFile = _wfopen(fileName.c_str(), L"r,ccs=UTF-8");
#else
    FILE* inFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(fileName), ACE_TEXT("r"));
#endif
    if (!inFile)
    {
        return false;
    }

    MG_TRY()

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgConfigurationSection> currentSection = SAFE_ADDREF((MgConfigurationSection*)m_rootSection);

    ACE_TCHAR buffer[4096];
    ACE_Configuration_Section_Key sectionKey = m_aceConfigurationHeap.root_section();
    while (ACE_OS::fgets(buffer, 4095, inFile))
    {
        wstring mbLine = Trim(MG_TCHAR_TO_WCHAR(buffer)); // remove extra spaces at beginning and end of line

        // Blank line
        if (mbLine.size() <= 0 || mbLine[0] == L'\0')
        {
            continue;
        }

        // Comment
        if (mbLine[0] == L'#' || mbLine[0] == L';')
        {
            currentSection->AddComment(mbLine);
            continue;
        }

        // New section is here
        if (mbLine[0] == L'[')
        {
            // Seperate out the section name
            size_t pos = mbLine.find(L']');
            if (pos == string::npos)
            {
                ACE_OS::fclose(inFile);
                return false;
            }
            mbLine.erase(pos,1);
            mbLine.erase(0,1);

            wstring sectionPath = Trim(mbLine);

            currentSection = AddSection(sectionPath, sectionKey);
            if (currentSection == NULL)
            {
                ACE_OS::fclose(inFile);
                return false;
            }
            continue;
        }

        // Must be a property if it gets to here
        size_t pos = mbLine.find(L'=');
        if (pos == string::npos)
        {
            // No equal sign, so something is wrong with file format
            ACE_OS::fclose(inFile);
            return false;
        }

        // Seperate out the property name and the value
        wstring propertyName = Trim(mbLine.substr(0, pos).c_str());
        wstring value = Trim(mbLine.substr(pos+1, (mbLine.size()-pos-1)).c_str());
        // Remove any quotes that may be present at ends of value
        if (value.size() > 0 && value[0] == L'\"' && value[value.length()-1] == L'\"')
        {
            value = value.substr(1,value.length()-2);
        }

        //currentSection->AddSectionInformation(MgUtil::MultiByteToWideChar(propertyName));
        SetStringValue(sectionKey, MG_WCHAR_TO_TCHAR(propertyName), MG_WCHAR_TO_TCHAR(value));
    }

    ACE_OS::fclose(inFile);

    MG_CATCH(L"MgConfigImpExp.ImportConfig")
    if (mgException != NULL)
    {
        ACE_OS::fclose(inFile);
        (*mgException).AddRef();
        mgException->Raise();
    }

    return true;
}


bool MgConfigurationHeap::ExportConfig(CREFSTRING fileName)
{
    if (fileName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgConfigImpExp.ImportConfig",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    FILE* outFile = NULL;

    MG_TRY()

#ifdef _WIN32
    outFile = _wfopen(fileName.c_str(), L"w,ccs=UTF-8");
#else
    outFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(fileName), ACE_TEXT("w"));
#endif
    if (!outFile)
    {
        return false;
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    ExportSection(m_rootSection, outFile);

    Ptr<MgConfigurationSection> section;
    INT32 count = m_sectionCollection->GetCount();
    INT32 i;
    for (i = 0; i < count; i++)
    {
        section = m_sectionCollection->GetItem(i);
        if (!ExportSection(section, outFile))
        {
            ACE_OS::fclose(outFile);
            return false;
        }
    }

    ACE_OS::fclose(outFile);

    MG_CATCH(L"MgConfigurationHeap.ExportConfig")

    if (mgException != NULL)
    {
        if (outFile)
        {
            ACE_OS::fclose(outFile);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return true;
}


// This (private) method is NOT thread-safe - callers are responsible
// for serializing access.
bool MgConfigurationHeap::ExportSection(MgConfigurationSection* section, FILE* outFile)
{
    wstring sectionPath;
    wstring line;

    sectionPath = section->GetSectionPath();

    ACE_Configuration_Section_Key key;
    if (sectionPath.length() > 0)
    {
        line = L"\n[" + sectionPath + L"]\n";
        ACE_OS::fputs(MG_WCHAR_TO_TCHAR(line), outFile);

        if (m_aceConfigurationHeap.open_section(m_aceConfigurationHeap.root_section(), MG_WCHAR_TO_TCHAR(sectionPath), 0, key) != 0)
        {
            return false;
        }
    }
    else
    {
        // section being exported is root section
        key = m_aceConfigurationHeap.root_section();
    }

    // Write out section Comments
    INT32 count = section->GetCommentCount();
    INT32 i;
    for (i = 0; i < count; i++)
    {
        line = section->GetComment(i);
        if (line[0] != L'#')
        {
            line = L"#" + line;
        }
        line += L"\n";
        ACE_OS::fputs(MG_WCHAR_TO_TCHAR(line), outFile);
    }

    i = 0;
    ACE_TString name;
    ACE_Configuration::VALUETYPE type;
    ACE_TCHAR int_value[32];
    ACE_TString string_value;
    while (!m_aceConfigurationHeap.enumerate_values(key, i, name, type))
    {
        line = MG_TCHAR_TO_WCHAR(name.c_str());
        line += L" = ";
        switch (type)
        {
        case ACE_Configuration::INTEGER:
            {
                u_int value;
                if (m_aceConfigurationHeap.get_integer_value(key, name.fast_rep(), value))
                {
                    return false;
                }
                ACE_OS::sprintf(int_value, ACE_TEXT("%08x"), value);
                line += L"dword:";
                line += MG_TCHAR_TO_WCHAR(int_value);
                break;
            }
        case ACE_Configuration::STRING:
            {
                if (m_aceConfigurationHeap.get_string_value(key, name.fast_rep(), string_value))
                {
                    return false;
                }
                line += MG_TCHAR_TO_WCHAR(string_value.c_str());
                break;
            }
#ifdef _WIN32
        case ACE_Configuration::INVALID:
            break;
#endif
        default:
            return false;
        }
        line += L"\n";
        if (ACE_OS::fputs(MG_WCHAR_TO_TCHAR(line), outFile) < 0)
        {
            return false;
        }
        i++;
    }

    return true;
}


wstring MgConfigurationHeap::Trim(const wstring& line)
{
    wstring str = line;

    size_t pos = str.find_last_not_of(L" \t\r\n");

    // No non-whitespace characters were found
    if (pos == string::npos)
    {
        str = L"";
    }
    else
    {
        str.erase(pos+1);
        pos = str.find_first_not_of(L" \t\r\n");
        if (pos != string::npos || pos != (str.size()-1))
        {
            str.erase(0, pos);
        }
    }

    return str;
}


bool MgConfigurationHeap::OpenSection(CREFSTRING sectionPath, INT32 create, ACE_Configuration_Section_Key& key)
{
    Ptr<MgConfigurationSection> section;

    MG_TRY()

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    INT32 index = m_sectionCollection->IndexOf(sectionPath);
    if (create && index == -1)
    {
        section = AddSection(sectionPath, key);
    }
    else
    {
        if (m_aceConfigurationHeap.open_section(m_aceConfigurationHeap.root_section(), MG_WCHAR_TO_TCHAR(sectionPath), create, key) != 0)
        {
            return false;
        }
        section = m_sectionCollection->GetItem(index);
    }

    MG_CATCH_AND_THROW(L"MgConfigurationHeap.OpenSection");

    return (section != NULL);
}


// This (private) method is NOT thread-safe - callers are responsible
// for serializing access.
MgConfigurationSection* MgConfigurationHeap::AddSection(CREFSTRING sectionPath, ACE_Configuration_Section_Key& key)
{
    Ptr<MgConfigurationSection> section;

    MG_TRY()

    if (m_aceConfigurationHeap.open_section(m_aceConfigurationHeap.root_section(), MG_WCHAR_TO_TCHAR(sectionPath), 1, key) != 0)
    {
        // Error retrieving section from ACE_Configuration_Heap object
        return NULL;
    }

    INT32 index = m_sectionCollection->IndexOf(sectionPath);
    if (index == -1)
    {
        section = new MgConfigurationSection(sectionPath);
        if (section == NULL)
        {
            throw new MgOutOfMemoryException(L"MgConfigurationHeap.AddSection", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        m_sectionCollection->Add(section);
    }
    else
    {
        section = m_sectionCollection->GetItem(index);
    }

    MG_CATCH_AND_THROW(L"MgConfigurationHeap.AddSection");

    return SAFE_ADDREF((MgConfigurationSection*)section);
}


bool MgConfigurationHeap::RemoveSection(CREFSTRING sectionPath, int recursive)
{
    bool result = true;

    MG_TRY()

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    if (m_aceConfigurationHeap.remove_section(m_aceConfigurationHeap.root_section(), MG_WCHAR_TO_TCHAR(sectionPath), recursive) != 0)
    {
        result = false;
    }

    INT32 index = m_sectionCollection->IndexOf(sectionPath);
    if (index != -1)
    {
        m_sectionCollection->RemoveAt(index);
    }

    if (recursive)
    {
        Ptr<MgConfigurationSection> section;
        section = NULL;

        INT32 i;
        INT32 count = m_sectionCollection->GetCount();
        for (i = 0; i < count; i++)
        {
            section = m_sectionCollection->GetItem(i);
            // Remove any sections that have the path of sectionPath/subsection or sectionPath\subsection
            if ((section->GetSectionPath()).find(sectionPath+L"/") == 0 ||
                (section->GetSectionPath()).find(sectionPath+L"\\") == 0)
            {
                m_sectionCollection->RemoveAt(i);
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgConfigurationHeap.RemoveSection");

    return true;
}


bool MgConfigurationHeap::Open(int default_map_size)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.open(default_map_size) == 0);
}


bool MgConfigurationHeap::EnumerateSections(const ACE_Configuration_Section_Key& key, int index, ACE_TString& name)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.enumerate_sections(key, index, name) == 0);
}


bool MgConfigurationHeap::EnumerateValues(const ACE_Configuration_Section_Key& key, int index, ACE_TString& name, ACE_Configuration::VALUETYPE& type)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.enumerate_values(key, index, name, type) == 0);
}


bool MgConfigurationHeap::SetStringValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name, const ACE_TString& value)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.set_string_value(key, name, value) == 0);
}


bool MgConfigurationHeap::GetStringValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name, ACE_TString& value)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.get_string_value(key, name, value) == 0);
}


bool MgConfigurationHeap::RemoveValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));
    return (m_aceConfigurationHeap.remove_value(key, name) == 0);
}
