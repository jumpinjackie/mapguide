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

#include "GeometryCommon.h"
#include "GeometryConsoleTest.h"

//=============================================================================

// Default values for all command line arguments.
#ifdef _WIN32
    wchar_t MgGctTestParameters::m_cOptionChar = L'/';
    wchar_t MgGctTestParameters::m_cDirectorySeparator = L'\\';
#else
    wchar_t MgGctTestParameters::m_cOptionChar = L'-';
    wchar_t MgGctTestParameters::m_cDirectorySeparator = L'/';
#endif

//const INT32 MgGctTestParameters::m_nMaxPositionalArgumentCount = 8;
bool MgGctTestParameters::m_bDefaultVerbose = false;
bool MgGctTestParameters::m_bDefaultSilent = false;
bool MgGctTestParameters::m_bDefaultBackground = false;
bool MgGctTestParameters::m_bDefaultRadiiCheck = false;
INT32 MgGctTestParameters::m_nDefaultDuration = 100UL;
wchar_t MgGctTestParameters::m_cDefaultLocale [128] = L"C";
wchar_t MgGctTestParameters::m_cDefaultTestSpecification [128] = L"12345678987654321";
wchar_t MgGctTestParameters::m_cDefaultConfigurationURL [1024] = L"../../../bin/TestData";
wchar_t MgGctTestParameters::m_cDefaultTestDataFolder [1024] = L"../../../Oem/CsMap/Dictionaries";
wchar_t MgGctTestParameters::m_cDefaultCrsTestDataFile [128] = L"CrsTestData.csv";
wchar_t MgGctTestParameters::m_cDefaultRegressionTestDataFile [128] = L"CsMap-13.01.csv";

MgGctTestParameters::MgGctTestParameters (void)
{
    InitializeToDefaultValues ();
}

MgGctTestParameters::MgGctTestParameters (const MgGctTestParameters& source)
                                               :
                                          m_nPositionalArgumentCount (source.m_nPositionalArgumentCount),
                                          m_bVerbose                 (source.m_bVerbose),
                                          m_bSilent                  (source.m_bSilent),
                                          m_bBackground              (source.m_bBackground),
                                          m_bRadiiCheck              (source.m_bRadiiCheck),
                                          m_nDuration                (source.m_nDuration),
                                          m_nSeed                    (source.m_nSeed)
{
    wcsncpy (m_cLocale,source.m_cLocale,wcCount (m_cLocale));
    m_cLocale [wcCount (m_cLocale) - 1] = L'\0';
    wcsncpy (m_cTestSpecification,source.m_cTestSpecification,wcCount (m_cTestSpecification));
    m_cTestSpecification [wcCount (m_cTestSpecification) - 1] = L'\0';
    wcsncpy (m_cConfigurationURL,source.m_cConfigurationURL,wcCount (m_cConfigurationURL));
    m_cConfigurationURL [wcCount (m_cConfigurationURL) - 1] = L'\0';
    wcsncpy (m_cTestDataFolder,source.m_cTestDataFolder,wcCount (m_cTestDataFolder));
    m_cTestDataFolder [wcCount (m_cTestDataFolder) - 1] = L'\0';
    wcsncpy (m_cCrsTestDataFile,source.m_cCrsTestDataFile,wcCount (m_cCrsTestDataFile));
    m_cCrsTestDataFile [wcCount (m_cCrsTestDataFile) - 1] = L'\0';
    wcsncpy (m_cRegressionTestDataFile,source.m_cRegressionTestDataFile,wcCount (m_cRegressionTestDataFile));
    m_cRegressionTestDataFile [wcCount (m_cRegressionTestDataFile) - 1] = L'\0';
    wcsncpy (m_cReason,source.m_cReason,wcCount (m_cReason));
    m_cReason [wcCount (m_cReason) - 1] = L'\0';
}
MgGctTestParameters::~MgGctTestParameters (void)
{
    // Nothing to do here, yet!
}
MgGctTestParameters& MgGctTestParameters::operator= (const MgGctTestParameters& rhs)
{
    if (&rhs != this)
    {
        m_nPositionalArgumentCount = rhs.m_nPositionalArgumentCount;
        m_bVerbose                 = rhs.m_bVerbose;
        m_bSilent                  = rhs.m_bSilent;
        m_bBackground              = rhs.m_bBackground;
        m_bRadiiCheck              = rhs.m_bRadiiCheck;
        m_nDuration                = rhs.m_nDuration;
        m_nSeed                    = rhs.m_nSeed;
        
        wcsncpy (m_cLocale,rhs.m_cLocale,wcCount (m_cLocale));
        m_cLocale [wcCount (m_cLocale) - 1] = L'\0';
        wcsncpy (m_cTestSpecification,rhs.m_cTestSpecification,wcCount (m_cTestSpecification));
        m_cTestSpecification [wcCount (m_cTestSpecification) - 1] = L'\0';
        wcsncpy (m_cConfigurationURL,rhs.m_cConfigurationURL,wcCount (m_cConfigurationURL));
        m_cConfigurationURL [wcCount (m_cConfigurationURL) - 1] = L'\0';
        wcsncpy (m_cTestDataFolder,rhs.m_cTestDataFolder,wcCount (m_cTestDataFolder));
        m_cTestDataFolder [wcCount (m_cTestDataFolder) - 1] = L'\0';
        wcsncpy (m_cCrsTestDataFile,rhs.m_cCrsTestDataFile,wcCount (m_cCrsTestDataFile));
        m_cCrsTestDataFile [wcCount (m_cCrsTestDataFile) - 1] = L'\0';
        wcsncpy (m_cReason,rhs.m_cReason,wcCount (m_cReason));
        m_cReason [wcCount (m_cReason) - 1] = L'\0';
    }
    return *this;
}
bool MgGctTestParameters::AddArgument (const char* argument)
{
    bool ok (false);

    wchar_t wcTemp [1024];

    mbstowcs (wcTemp,argument,wcCount (wcTemp));
    if (*argument == m_cOptionChar)
    {
        ok = ProcessOption (argument);
    }
    else
    {
        ok = ProcessPositional (argument);
    }
    return ok;
}

bool MgGctTestParameters::Initialize (int argc,const char* argv[])
{
    bool ok (true);        // until we know different

    int idx;
    const char *currentArg;

    InitializeToDefaultValues ();

    for (idx = 0;ok && idx < argc;idx += 1)
    {
        currentArg = argv [idx];
        if (*currentArg == m_cOptionChar)
        {
            ok = ProcessOption (currentArg);
        }
        else
        {
            ok = ProcessPositional (currentArg);
        }
    }
    return ok;
}
INT32 MgGctTestParameters::GetTestCount ()
{
    INT32 testCount = static_cast<INT32>(wcslen (m_cTestSpecification));
    return testCount;
}

wchar_t MgGctTestParameters::GetTestChar (INT32 index)
{
    INT32 testCount = static_cast<INT32>(wcslen (m_cTestSpecification));
    if (index < 0 || index > testCount)
    {
        index = testCount;
    }
    return m_cTestSpecification [index];
}
INT32 MgGctTestParameters::GetPositionalCount (void)
{
    return m_nPositionalArgumentCount;
}
const wchar_t* MgGctTestParameters::GetPositionalArgument (INT32 index)
{
    const wchar_t* rtnValue (0);

    if (index < 0 || index >= m_nPositionalArgumentCount)
    {
        swprintf (m_cReason,wcCount (m_cReason),L"Positional argument number %d not provided on command line.",index + 1);
    }
    else
    {
        rtnValue = m_cPositionals [index];
    }
    return rtnValue;
}

//=============================================================================
// Private Member Functions
//
void MgGctTestParameters::InitializeToDefaultValues (void)
{
    m_nPositionalArgumentCount = 0;
    m_bVerbose                 = m_bDefaultVerbose;
    m_bSilent                  = m_bDefaultSilent;
    m_bBackground              = m_bDefaultBackground;
    m_bRadiiCheck              = m_bDefaultRadiiCheck;
    m_nDuration                = m_nDefaultDuration;

    memset (m_cLocale,'\0',sizeof (m_cLocale));
    memset (m_cTestSpecification,'\0',sizeof (m_cTestSpecification));
    memset (m_cConfigurationURL,'\0',sizeof (m_cConfigurationURL));
    memset (m_cTestDataFolder,'\0',sizeof (m_cTestDataFolder));
    memset (m_cCrsTestDataFile,'\0',sizeof (m_cCrsTestDataFile));
    memset (m_cRegressionTestDataFile,'\0',sizeof (m_cRegressionTestDataFile));
    memset (m_cReason,'\0',sizeof (m_cReason));

    wcsncpy (m_cLocale,m_cDefaultLocale,wcCount (m_cLocale));
    m_cLocale [wcCount (m_cLocale)] = L'\0';
    wcsncpy (m_cTestSpecification,m_cDefaultTestSpecification,wcCount (m_cTestSpecification));
    m_cTestSpecification [wcCount (m_cTestSpecification)] = L'\0';
    wcsncpy (m_cTestDataFolder,m_cDefaultTestDataFolder,wcCount (m_cTestDataFolder));
    m_cTestDataFolder [wcCount (m_cTestDataFolder)] = L'\0';
    wcsncpy (m_cCrsTestDataFile,m_cDefaultCrsTestDataFile,wcCount (m_cCrsTestDataFile));
    m_cCrsTestDataFile [wcCount (m_cCrsTestDataFile)] = L'\0';
    wcsncpy (m_cRegressionTestDataFile,m_cDefaultRegressionTestDataFile,wcCount (m_cRegressionTestDataFile));
    m_cRegressionTestDataFile [wcCount (m_cRegressionTestDataFile)] = L'\0';

    // The default random number seed value is a random number.
    m_nSeed = static_cast<UINT32>(time ((time_t*)0)) & 0x3FFF;
}
bool MgGctTestParameters::ProcessOption (const char* option)
{
    bool ok (false);
    char optionChar;
    size_t len;
    char *nullPtr;
    const char* optionPtr;
    wchar_t wrkArray [64];

    optionPtr = option;
    if (*optionPtr++ == m_cOptionChar)
    {
        optionChar = *optionPtr++;
        switch (optionChar) {
        case 'v':
            m_bVerbose = true;
            ok = true;
            break;
        case 's':
            m_bSilent = true;
            ok = true;
            break;
        case 'b':
            m_bBackground = true;
            ok = true;
            break;
        case 'e':
            m_bRadiiCheck = true;
            ok = true;
            break;
        case 'p':
            m_nDuration = strtoul (optionPtr,&nullPtr,10);
            ok = (*nullPtr == '\0' && nullPtr != optionPtr && m_nDuration != ULONG_MAX);
            if (!ok)
            {
                mbstowcs (wrkArray,optionPtr,wcCount (wrkArray));
                wrkArray [wcCount (wrkArray) - 1] = L'\0';
                swprintf (m_cReason,wcCount (m_cReason),L"Value of duration option (option 'p') is invalid [%s]",wrkArray);
            }
            if (ok)
            {
                if (m_nDuration < 1 || m_nDuration > 1000)
                {
                    ok = false;
                    swprintf (m_cReason,wcCount (m_cReason),L"Value of duration option (option 'p') is extreme. [%ld]",m_nDuration);
                }
            }
            break;
        case 'r':
            m_nSeed = strtoul (optionPtr,&nullPtr,10);
            ok = (*nullPtr == '\0' && nullPtr != optionPtr && m_nSeed != ULONG_MAX);
            if (!ok)
            {
                mbstowcs (wrkArray,optionPtr,wcCount (wrkArray));
                wrkArray [wcCount (wrkArray) - 1] = L'\0';
                swprintf (m_cReason,wcCount (m_cReason),L"Value of random number seed option (option 'r') is invalid [%s]",wrkArray);
            }
            if (ok)
            {
                if (m_nSeed < 1 || m_nSeed > 0x3FFF)
                {
                    ok = false;
                    swprintf (m_cReason,wcCount (m_cReason),L"Value of random number seed option (option 'r') is extreme. [%ld]",m_nSeed);
                }
            }
            ok = true;
            break;
        case 't':
            len = strlen (optionPtr);
            ok = (len > 0) && (len < (wcCount (m_cTestSpecification)));
            if (!ok)
            {
                mbstowcs (wrkArray,optionPtr,wcCount (wrkArray));
                wrkArray [wcCount (wrkArray) - 1] = L'\0';
                swprintf (m_cReason,wcCount (wrkArray),L"Test specification option (option 't') is empty or too long [%s...].",wrkArray);
            }
            else
            {
                mbstowcs (m_cTestSpecification,optionPtr,wcCount (m_cTestSpecification));
                m_cTestSpecification [wcCount (m_cTestSpecification) - 1] = L'\0';
                ok = true;
            }
            break;
        case 'd':
            len = strlen (optionPtr);
            ok = (len > 0) && (len < (wcCount (m_cConfigurationURL)));
            if (!ok)
            {
                mbstowcs (wrkArray,optionPtr,wcCount (wrkArray));
                wrkArray [wcCount (wrkArray) - 1] = L'\0';
                swprintf (m_cReason,wcCount (wrkArray),L"Configuration specification option (option 'd') is empty or too long [%s...].",wrkArray);
            }
            else
            {
                mbstowcs (m_cConfigurationURL,optionPtr,wcCount (m_cConfigurationURL));
                m_cConfigurationURL [wcCount (m_cConfigurationURL) - 1] = L'\0';
                ok = true;
            }
            break;
        case 'w':
            len = strlen (optionPtr);
            ok = (len > 0) && (len < (sizeof (m_cTestDataFolder)));
            if (!ok)
            {
                mbstowcs (wrkArray,optionPtr,wcCount (wrkArray));
                wrkArray [wcCount (wrkArray) - 1] = L'\0';
                swprintf (m_cReason,wcCount (wrkArray),L"Test data folder specification option (option 'w') is empty or too long [%s...].",wrkArray);
            }
            else
            {
                mbstowcs (m_cTestDataFolder,optionPtr,wcCount (m_cTestDataFolder));
                m_cTestDataFolder [wcCount (m_cTestDataFolder) - 1] = L'\0';
                ok = true;
            }
            break;
        default:
            ok = false;
            swprintf (m_cReason,wcCount (wrkArray),L"Unrecognized option specification (%C) encountered.",optionChar);
            break;
        }
    }
    return ok;
}
bool MgGctTestParameters::ProcessPositional (const char* argument)
{
    bool ok;
    if (m_nPositionalArgumentCount >= m_nMaxPositionalArgumentCount)
    {
        ok = false;
        swprintf (m_cReason,wcCount (m_cReason),L"Too many positional arguments, only %d positional arguments are supported.",m_nMaxPositionalArgumentCount);
    }
    else
    {
        mbstowcs (m_cPositionals [m_nMaxPositionalArgumentCount],argument,m_nMaxPositionalArgumentSize);
        m_cPositionals [m_nPositionalArgumentCount++][m_nMaxPositionalArgumentSize - 1] = L'\0';
        ok = true;
    }
    return ok;
}
void MgGctTestParameters::Usage (bool batch)
{
    wprintf (L"Usage: TEST [%cb] [%cddir] [%cp30] [%cr1234] [%cs] [%ct123] [%cv] [test_file]\n",
            m_cOptionChar,m_cOptionChar,m_cOptionChar,m_cOptionChar,m_cOptionChar,m_cOptionChar,m_cOptionChar);
    wprintf (L"\t       %cb  batch mode, supresses acknowledgement before exit\n",m_cOptionChar);
    wprintf (L"\t       %cd  specifies Dictionary/Configuration directory\n",m_cOptionChar);
    wprintf (L"\t       %cl  test environement locale name\n",m_cOptionChar);
    wprintf (L"\t       %cp  test duration parameter\n",m_cOptionChar);
    wprintf (L"\t       %cr  specify random number seed value\n",m_cOptionChar);
    wprintf (L"\t       %cs  run in silent mode\n",m_cOptionChar);
    wprintf (L"\t       %ct  specifies tests, and sequence\n",m_cOptionChar);
    wprintf (L"\t       %cv  run in verbose mode\n",m_cOptionChar);
    wprintf (L"\ttest_file  name of test data file\n\n");
    if (!batch)
    {
        wprintf (L"Press any key to continue: ");
        wcout.flush ();
        getwchar ();
        wprintf (L"\n");
    }
}
