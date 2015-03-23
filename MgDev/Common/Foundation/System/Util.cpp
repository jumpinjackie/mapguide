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
#include "UnicodeString.h"
#include "SetLocale.h"
#include <math.h>

#include <algorithm>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
static HMODULE  m_hFactoryDll;
#else
static void* m_hFactoryLib;
#endif

ACE_Utils::UUID_Generator uuidGenerator;

bool MgUtil::uuidInitialized = false;
const double MgUtil::DefaultCompareTolerance = 0.01; // 1% margin of error

// Linux process information pseudo-filesystem accessors
const STRING MgUtil::LinuxFilesystemMeminfo              = L"/proc/meminfo";     // NOXLATE
const STRING MgUtil::LinuxFilesystemStat                 = L"/proc/stat";        // NOXLATE

const STRING MgUtil::sm_xssReservedCharacters = L"<>&"; // NOXLATE

const STRING MgUtil::sm_classNameQualifier     = L":"; // NOXLATE
const STRING MgUtil::sm_classPropertyQualifier = L"."; // NOXLATE

void MgUtil::InitializeUuidGenerator()
{
    if (false == uuidInitialized)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
        if (false == uuidInitialized)
        {
            uuidGenerator.init();
            uuidInitialized = true;
        }
    }
}


MgObject* MgUtil::CreateMgObject(INT32 classId)
{
    MgClassFactory* factory = MgClassFactory::GetInstance();
    CHECKNULL(factory, L"MgUtil.CreateMgObject");

    MgObject* obj = factory->CreateMgObject(classId);
    CHECKNULL(obj, L"MgUtil.CreateMgObject");

    return obj;
}

string MgUtil::Char2Hex(char a)
{
    string str = "";
    int n = a;
    for (int i=2*sizeof(char) - 1; i>=0; i--)
    {
        str += "0123456789ABCDEF"[((n >> i*4) & 0xF)];
    }
    return str;
}

MgByteReader* MgUtil::GetByteReader(string &s, STRING* mimeType)
{
    BYTE_ARRAY_IN bytes = (BYTE_ARRAY_IN)s.c_str();
    Ptr<MgByteSource> byteSource = new MgByteSource(bytes, (INT32)s.length());
    if(mimeType != NULL)
        byteSource->SetMimeType(*mimeType);
    return byteSource->GetReader();
}

string MgUtil::GetStringFromReader(MgByteReader* byteReader)
{
    string strTemp;

    if ( byteReader == NULL)
        return strTemp;

    unsigned char buf[1024];

    INT32 len = byteReader->Read(((BYTE_ARRAY_OUT)&buf[0]),1024);

    while (len != 0)
    {
        for (int i=0; i < len; i++)
            strTemp += MgUtil::Char2Hex(buf[i]);

        len = byteReader->Read(buf,1024);
    }

    return strTemp;
}

string MgUtil::GetTextFromReader(MgByteReader* byteReader)
{
    string strTemp;

    if ( byteReader == NULL)
        return strTemp;

    unsigned char buf[1025];

    INT32 len = byteReader->Read(((BYTE_ARRAY_OUT)&buf[0]),1024);
    buf[len]=0;
    while (len != 0)
    {
        strTemp += (char*)buf;
        len = byteReader->Read(buf,1024);
        buf[len]=0;
    }

    return strTemp;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a wide character string to a multibyte character string.
/// </summary>
///
/// <param name="wcStr">
/// Wide character string.
/// </param>
///
/// <returns>
/// Multibyte character string.
/// </returns>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------

string MgUtil::WideCharToMultiByte(CREFSTRING wcStr)
{
    string mbStr;

    WideCharToMultiByte(wcStr, mbStr);

    return mbStr;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a wide character string to a multibyte character string.
/// </summary>
///
/// <param name="wcStr">
/// Wide character string to convert.
/// </param>
/// <param name="mbStr">
/// Multibyte character string to receive.
/// </param>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------

void MgUtil::WideCharToMultiByte(CREFSTRING wcStr, string& mbStr)
{
    try
    {
        UnicodeString::WideCharToMultiByte(wcStr.c_str(), mbStr);
    }
    catch (int)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(wcStr);

        throw new MgInvalidArgumentException(L"MgUtil.WideCharToMultiByte",
            __LINE__, __WFILE__, &arguments, L"MgInvalidStringConversion", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a wide character buffer to a multibyte character buffer.
/// </summary>
///
/// <param name="wcBuf">
/// Wide character buffer.
/// </param>
///
/// <returns>
/// Multibyte character buffer.
/// </returns>
///
/// <notes>
/// The caller is responsible to de-allocate the returned buffer.
/// </notes>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------

char* MgUtil::WideCharToMultiByte(const wchar_t* wcBuf)
{
    string mbStr;

    try
    {
        UnicodeString::WideCharToMultiByte(wcBuf, mbStr);
    }
    catch (int)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add((NULL == wcBuf) ? L"" : wcBuf);

        throw new MgInvalidArgumentException(L"MgUtil.WideCharToMultiByte",
            __LINE__, __WFILE__, &arguments, L"MgInvalidStringConversion", NULL);
    }

    char* mbBuf = 0;

    if (mbStr.length() >= 0)
    {
        mbBuf = new char[mbStr.length() + 1];
        strcpy(mbBuf, mbStr.c_str());
    }

    return mbBuf;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a multibyte character string to a wide character string.
/// </summary>
///
/// <param name="mbStr">
/// Multibyte character string.
/// </param>
///
/// <returns>
/// Wide character string.
/// </returns>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------

STRING MgUtil::MultiByteToWideChar(const string& mbStr)
{
    STRING wcStr;

    MultiByteToWideChar(mbStr, wcStr);

    return wcStr;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a multibyte character string to a wide character string.
/// </summary>
///
/// <param name="mbStr">
/// Multibyte character string to convert.
/// </param>
/// <param name="wcStr">
/// Wide character string to receive.
/// </param>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------
void MgUtil::MultiByteToWideChar(const string& mbStr, REFSTRING wcStr)
{
    try
    {
        UnicodeString::MultiByteToWideChar(mbStr.c_str(), wcStr);
    }
    catch (int)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(L"const string&");

        throw new MgInvalidArgumentException(L"MgUtil.WideCharToMultiByte",
            __LINE__, __WFILE__, &arguments, L"MgInvalidStringConversion", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a multibyte character buffer to a wide character buffer.
/// </summary>
///
/// <param name="mbBuf">
/// Multibyte character buffer.
/// </param>
///
/// <returns>
/// Wide character buffer.
/// </returns>
///
/// <notes>
/// The caller is responsible to de-allocate the returned buffer.
/// </notes>
///
/// EXCEPTIONS: MgInvalidArgumentException, MgOutOfMemoryException
///----------------------------------------------------------------------------

wchar_t* MgUtil::MultiByteToWideChar(const char* mbBuf)
{
    wstring wStr;

    try
    {
        UnicodeString::MultiByteToWideChar(mbBuf, wStr);
    }
    catch (int)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(L"const char*");

        throw new MgInvalidArgumentException(L"MgUtil.WideCharToMultiByte",
            __LINE__, __WFILE__, &arguments, L"MgInvalidStringConversion", NULL);
    }

    wchar_t* wcBuf = 0;

    // We want to duplicate empty strings too.
    wcBuf = new wchar_t[wStr.length() + 1];
    wcscpy(wcBuf, wStr.c_str());

    return wcBuf;
}

//////////////////////////////////////////////
///<summary>
/// TrimRight()
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
 STRING MgUtil::TrimRight(CREFSTRING source, CREFSTRING t)
{
    STRING str = source;
    return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

//////////////////////////////////////////////
///<summary>
/// TrimLeft
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
 STRING MgUtil::TrimLeft(CREFSTRING source , CREFSTRING t)
{
    STRING str = source;
    return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
}

//////////////////////////////////////////////
///<summary>
/// TrimLeft and TrimRight
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
STRING MgUtil::Trim(CREFSTRING source , CREFSTRING t)
{
    STRING tmp(source);

    return TrimLeft(TrimRight(tmp, t), t);
}

//////////////////////////////////////////////
///<summary>
/// TrimRight()
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
 string MgUtil::TrimRight(const string& s, const string& t)
{
    string d(s);
    string::size_type i(d.find_last_not_of(t));
    if (i == string::npos)
        return "";
    else
        return d.erase(d.find_last_not_of(t) + 1);
}

//////////////////////////////////////////////
///<summary>
/// TrimLeft
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
string MgUtil::TrimLeft(const string& s, const string& t)
{
    string d(s);
    return d.erase(0, s.find_first_not_of(t));
}

//////////////////////////////////////////////
///<summary>
/// TrimLeft and TrimRight
///</summary>
///<param name="source">
/// source string to trim
///<param>
///<param name="t">
/// char needed for trim (default is space)
///<param>
///<returns>
///trimed string
///</returns>
string MgUtil::Trim(const string& source, const string& t)
{
    string tmp(source);

    return TrimLeft(TrimRight(tmp, t), t);
}

//////////////////////////////////////////////
///<summary>
///Check for the reserved characters
///</summary>
///EXCEPTIONS
///  MgInvalidArgumentException
void MgUtil::CheckReservedCharacters(CREFSTRING str, CREFSTRING reservedChars, bool anyOne)
{
    int index = -1;
    int orgLen = (int)str.length();
    if ( orgLen > 0 )
    {
        if (anyOne)
            index = (int)str.find_first_of(reservedChars);
        else
            index = (int)str.find(reservedChars);

        if ( index >= 0 )
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(str);

            MgStringCollection whyArguments;
            whyArguments.Add(reservedChars);

            throw new MgInvalidArgumentException(L"CheckReseveredCharater",
                __LINE__,  __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
        }
    }

}

//////////////////////////////////////////////
///<summary>
///Check for spaces at begining and end of string
///</summary>
///EXCEPTIONS
///  MgInvalidArgumentException
void MgUtil::CheckSpacesAtBeginEnd(CREFSTRING str)
{
    CheckBeginEnd(str, L" ");
}

//////////////////////////////////////////////
///<summary>
///Check for slashes at begining and end of string
///</summary>
///EXCEPTIONS
///  MgInvalidArgumentException
void MgUtil::CheckSlashAtBeginEnd(CREFSTRING str)
{
    CheckBeginEnd(str, L"/");
}

//////////////////////////////////////////////
///<summary>
///Check for characters at begining and end of string
///</summary>
///EXCEPTIONS
///  MgInvalidArgumentException
void MgUtil::CheckBeginEnd(CREFSTRING str, CREFSTRING sChar)
{
    int orgLen = (int)str.length();
    if ( orgLen > 0 )
    {
        if (NULL != wcschr(sChar.c_str(), str[0]) || NULL != wcschr(sChar.c_str(), str[orgLen-1]) )
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(str);

            throw new MgInvalidArgumentException(L"MgUtil.CheckBeginEnd",
                __LINE__,  __WFILE__, &arguments, L"MgInvalidStringTrim", NULL);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Perform Cross Site Scripting Attack validations against the specified
/// string.
///
void MgUtil::CheckXss(CREFSTRING str)
{
    CheckReservedCharacters(str, sm_xssReservedCharacters);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Perform some encoding on the specified string to prevent Cross Site
/// Scripting attacks.
///
STRING MgUtil::EncodeXss(CREFSTRING str)
{
    STRING encodedStr = str;

    encodedStr = ReplaceString(encodedStr, L"&", L"&amp;"); // This must be encoded first.
    encodedStr = ReplaceString(encodedStr, L"<", L"&lt;");
    encodedStr = ReplaceString(encodedStr, L">", L"&gt;");

    return encodedStr;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Replace an existing text pattern with a new text pattern in a string.
///
STRING MgUtil::ReplaceString(STRING str, const wchar_t* oldValue, const wchar_t* newValue)
{
    CHECKARGUMENTNULL(oldValue, L"MgUtil.ReplaceString");
    CHECKARGUMENTNULL(newValue, L"MgUtil.ReplaceString");

    STRING newStr = str.c_str();

    ReplaceString(oldValue, newValue, newStr, -1);

    return newStr;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Replace an existing text pattern with a new text pattern in a string.
///
INT32 MgUtil::ReplaceString(CREFSTRING oldValue, CREFSTRING newValue, REFSTRING str, INT32 replacements)
{
    INT32 count = 0;

    if (0 != replacements)
    {
        size_t index = 0;
        size_t num1 = oldValue.length();
        size_t num2 = newValue.length();

        if (num2 > num1)
        {
            str.reserve(std::min(str.length(), (size_t)::abs(int(replacements * (num2 - num1)))));
        }

        while (wstring::npos != (index = str.find(oldValue, index)))
        {
            str.replace(index, num1, newValue, 0, num2);
            index += num2;
            ++count;

            if (count == replacements)
            {
                break;
            }
        }
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Replace an existing text pattern with a new text pattern in a string.
///
INT32 MgUtil::ReplaceString(const string& oldValue, const string& newValue, string& str, INT32 replacements)
{
    INT32 count = 0;

    if (0 != replacements)
    {
        size_t index = 0;
        size_t num1 = oldValue.length();
        size_t num2 = newValue.length();

        if (num2 > num1)
        {
            str.reserve(std::min(str.length(), (size_t)::abs(int(replacements * (num2 - num1)))));
        }

        while (string::npos != (index = str.find(oldValue, index)))
        {
            str.replace(index, num1, newValue, 0, num2);
            index += num2;
            ++count;

            if (count == replacements)
            {
                break;
            }
        }
    }

    return count;
}

bool MgUtil::StringToBoolean(CREFSTRING str)
{
    STRING tmp = Trim(str, L" \t\r\n");
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

    return (tmp.compare(L"true") == 0 ? true : false);
}

INT32 MgUtil::StringToInt32(CREFSTRING str)
{
    string tmp;
    WideCharToMultiByte(Trim(str, L" \t\r\n"), tmp);

    return static_cast<INT32>(::atoi(tmp.c_str()));
}

INT64 MgUtil::StringToInt64(CREFSTRING str)
{
    // TODO: This is not accurate but is big enough for internal use for now.
    return (INT64)MgUtil::StringToDouble(str);
}

double MgUtil::StringToDouble(CREFSTRING str)
{
    // TODO: Is if safe to assume the string is in C locale?
    STRING tmp = Trim(str, L" \t\r\n");

    return ::wcstod(tmp.c_str(), NULL);
}

bool MgUtil::StringToBoolean(const string& str)
{
    string tmp = Trim(str, " \t\r\n");
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

    return (tmp.compare("true") == 0 ? true : false);
}

///----------------------------------------------------------------------------
/// <summary>
/// Generates an Universal Unique IDentifier.
/// </summary>
///
/// <param name="uuid">
/// Generated UUID
/// </param>
///
/// EXCEPTIONS: MgOutOfMemoryException
///----------------------------------------------------------------------------

void MgUtil::GenerateUuid(REFSTRING uuid)
{
    InitializeUuidGenerator();

    MG_TRY()

    string str;

    MgUtil::GenerateUuid(str);

    uuid = ACE_Ascii_To_Wide(str.c_str()).wchar_rep();

    MG_CATCH_AND_THROW(L"MgUtil.GenerateUuid")
}

///----------------------------------------------------------------------------
/// <summary>
/// Generates an Universal Unique IDentifier.
/// </summary>
///
/// <param name="uuid">
/// Generated UUID
/// </param>
///
/// EXCEPTIONS: MgOutOfMemoryException
///----------------------------------------------------------------------------

void MgUtil::GenerateUuid(string& uuid)
{
    InitializeUuidGenerator();

    MG_TRY()

    ACE_Utils::UUID uuidObject;

    uuidGenerator.generate_UUID(uuidObject);

    const ACE_CString* str = uuidObject.to_string();

    if (0 == str)
    {
        throw new MgOutOfMemoryException(L"MgUtil.GenerateUuid",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    uuid = str->c_str();

    MG_CATCH_AND_THROW(L"MgUtil.GenerateUuid")
}

STRING MgUtil::ReplaceEscapeCharInXml(CREFSTRING str)
{
    STRING newStr;
    size_t len = str.length();

    newStr.reserve(len);

    for(size_t i = 0; i < len; i++)
    {
        switch(str[i])
        {
            case '&' :
            {
                newStr += L"&amp;";
                break;
            }
            case '\'' :
            {
                newStr += L"&apos;";
                break;
            }
            case '>' :
            {
                newStr += L"&gt;";
                break;
            }
            case '<' :
            {
                newStr += L"&lt;";
                break;
            }
            case '"' :
            {
                newStr += L"&quot;";
                break;
            }
            default :
                newStr += str[i];
        }
    }
    return newStr;
}

STRING MgUtil::GetResourceMessage(CREFSTRING section, CREFSTRING messageId)
{
    STRING str;

    MG_TRY()

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);
    MgResources* resources = MgResources::GetInstance();
    assert(NULL != resources);

    if (NULL != configuration && NULL != resources)
    {
        STRING locale;

        configuration->GetStringValue(
            MgFoundationConfigProperties::GeneralPropertiesSection,
            MgFoundationConfigProperties::GeneralPropertyDefaultMessageLocale,
            locale,
            MgFoundationConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);

        str = resources->GetStringResource(locale, section, messageId);
    }

    // If no message is found, we do not fail.

    MG_CATCH_AND_RELEASE()

    return str;
}

bool MgUtil::GetLinuxMemoryStatus(MgLinuxMemoryStatus* pMemoryStatus)
{
    bool bSuccess = false;

    #ifdef _WIN32

        throw new MgPlatformNotSupportedException(L"MgUtil.GetLinuxMemoryStatus", __LINE__, __WFILE__, NULL, L"", NULL);

    #else

        assert(pMemoryStatus != 0);

        FILE* meminfo;
        char line[80];

        meminfo = fopen( ( MgUtil::WideCharToMultiByte(MgUtil::LinuxFilesystemMeminfo) ).c_str(), "r" );  //NOXLATE
        if (NULL == meminfo)
        {
            MgStringCollection arguments;
            arguments.Add(MgUtil::LinuxFilesystemMeminfo);
            throw new MgFileNotFoundException(L"MgUtil.GetLinuxMemoryStatus", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        /*

        /proc/meminfo looks like this:

        MemTotal:	    <size> kB	<-- Read this
        MemFree:	    <size> kB	<-- Read this
        Buffers:	    <size> kB	<-- Read this
        Cached:		    <size> kB	<-- Read this
        SwapCached:	    <size> kB
        Active:		    <size> kB
        Inactive:	    <size> kB
        Active(anon):	<size> kB
        Inactive(anon):	<size> kB
        Active(file):	<size> kB
        Inactive(file):	<size> kB
        Unevictable:	<size> kB
        Mlocked:	    <size> kB
        HighTotal:	    <size> kB
        HighFree:	    <size> kB
        LowTotal:	    <size> kB
        LowFree:	    <size> kB
        SwapTotal:	    <size> kB	<-- Read this
        SwapFree:	    <size> kB	<-- Read this
        ...

        Infer the used amount by (total - free)

        */

        double total;
        double used;
        double free;
        double shared;
        double buffers;
        double cached;
        double swapTotal;
        double swapUsed;
        double swapFree;
        int lineNum = 0;
        bool bComplete = false;
        //printf("Parse /proc/meminfo\n");
        //Read until all lines are accounted for or EOF
        //All values read are in KB so they have to be multiplied
        //by 1024 for the number to be in bytes (same as Windows)
        while (fgets(line, 80, meminfo) != NULL)
        {
            lineNum++;
            if (lineNum == 1) //MemTotal line
            {
                //printf("%s\n", line);
                sscanf(line, "%*9s %lf", &total);
                total *= 1024;
                //printf("Parsed: %lf\n", total);
            }
            else if (lineNum == 2) //MemFree line
            {
                //printf("%s\n", line);
                sscanf(line, "%*8s %lf", &free);
                free *= 1024;
                //printf("Parsed: %lf\n", free);
            }
            else if (lineNum == 3) //Buffers line
            {
                //printf("%s\n", line);
                sscanf(line, "%*8s %lf", &buffers);
                buffers *= 1024;
                //printf("Parsed: %lf\n", buffers);
            }
            else if (lineNum == 4) //Cached line
            {
                //printf("%s\n", line);
                sscanf(line, "%*7s %lf", &cached);
                cached *= 1024;
                //printf("Parsed: %lf\n", cached);
            }
            else if (lineNum == 18) //SwapTotal line
            {
                //printf("%s\n", line);
                sscanf(line, "%*10s %lf", &swapTotal);
                swapTotal *= 1024;
            }
            else if (lineNum == 19) //SwapFree line
            {
                //printf("%s\n", line);
                sscanf(line, "%*9s %lf", &swapFree);
                swapFree *= 1024;                
            }

            if (lineNum > 19) //Past SwapFree line
            {
                bComplete = true; //All lines accounted for
                break;	
            }
        }

        if (bComplete) 
        {
            pMemoryStatus->m_memTotal = (INT64)total;
            pMemoryStatus->m_memFree = (INT64)free;
            pMemoryStatus->m_memUsed = (INT64)(pMemoryStatus->m_memTotal - pMemoryStatus->m_memFree);
            //TODO: What value in /proc/meminfo represents this? This value isn't used at the moment so we can sort of get away with it for now
            pMemoryStatus->m_memShared = 0L;
            pMemoryStatus->m_memBuffers = (INT64)buffers;
            pMemoryStatus->m_memCached = (INT64)cached; 

            pMemoryStatus->m_swapTotal = (INT64)swapTotal;
            pMemoryStatus->m_swapFree = (INT64)swapFree;
            pMemoryStatus->m_swapUsed = (INT64)(pMemoryStatus->m_swapTotal - pMemoryStatus->m_swapFree);

            //printf("Parse complete\n");
            bSuccess = true;
        } 
        else 
        {
            //printf("Parse failed\n");
            bSuccess = false;
        }
        fclose(meminfo);

    #endif // _WIN32

    return bSuccess;
}

void MgUtil::Int32ToLocaleSpecificString(INT32 val, string& str)
{
    stringstream s;

    s << std::setprecision(0) << std::fixed;

    MgSetLocale mgSetLocale(LC_NUMERIC, ""); // set the locale to the default obtained from the OS
    locale loc( setlocale(LC_NUMERIC, NULL) );  // create an instance of locale from querying the current locale
    s.imbue(loc);  // set the locale for the stream

    s << val;
    str = s.str();
}

void MgUtil::Int32ToString(INT32 val, string& str)
{
    char buf[32] = { 0 };

    ::sprintf(buf, "%d", val);
    str = &buf[0];
}

void MgUtil::Int32ToString(INT32 val, STRING& str)
{
    wchar_t buf[32] = { 0 };

    ::swprintf(buf, 32, L"%d", val);
    str = &buf[0];
}

void MgUtil::Int32ToHexString(INT32 val, string& str)
{
    char buf[32] = { 0 };

    ::sprintf(buf, "%x", val);
    str = &buf[0];
}

void MgUtil::UInt32ToString(UINT32 val, string& str)
{
    char buf[32] = { 0 };

    ::sprintf(buf, "%u", val);
    str = &buf[0];
}

void MgUtil::UInt32ToString(UINT32 val, STRING& str)
{
    wchar_t buf[32] = { 0 };

    ::swprintf(buf, 32, L"%u", val);
    str = &buf[0];
}

void MgUtil::Int64ToString(INT64 val, string& str)
{
    char buf[64] = { 0 };

#ifdef _WIN32
    _i64toa(val, buf, 10);
#else
    ::snprintf(buf, 64, "%lli", val);
#endif
    str = &buf[0];
}

void MgUtil::Int64ToString(INT64 val, STRING& str)
{
    wchar_t buf[64] = { 0 };

#ifdef _WIN32
    _i64tow(val, buf, 10);
#else
    ::swprintf(buf, 64, L"%lli", val);
#endif
    str = &buf[0];
}

void MgUtil::SingleToString(float val, string& str)
{
    char buf[32] = { 0 };

    ::sprintf(buf, "%.8g", val);
    str = &buf[0];
}

void MgUtil::SingleToString(float val, STRING& str)
{
    wchar_t buf[32] = { 0 };

    ::swprintf(buf, 32, L"%.8g", val);
    str = &buf[0];
}

void MgUtil::DoubleToString(double val, string& str)
{
    char buf[64] = { 0 };

    ::sprintf(buf, "%.17g", val);
    str = &buf[0];
}

void MgUtil::DoubleToString(double val, STRING& str)
{
    wchar_t buf[64] = { 0 };

    ::swprintf(buf, 64, L"%.17g", val);
    str = &buf[0];
}

bool MgUtil::ValuesEqual(double value1, double value2, double tolerance, bool output)
{
    bool valuesEqual = true;
    double difference = value1 - value2;

    if (0.0 != difference)
    {
        double base = (0.0 == value1) ? value2 : value1;
        assert(0.0 != base);
        double error = difference / base;

        if (::fabs(error) > ::fabs(tolerance))
        {
            // If the values don't match and don't fall within the tolerance, then output them.
#ifdef _DEBUG   // TODO: Implement/use MG_DEBUG macro
            if(output)
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("\nMgUtil.ValuesEqual() failed!\nOriginal: \"%.17g\"\nCalculated: \"%.17g\"\n"), value1, value2));
            }
#endif
            valuesEqual = false;
        }
    }

    return valuesEqual;
}

void MgUtil::TrimEndingZeros(string& str)
{
    int len = (int)str.size();
    int index = (int)str.find(".");

    int i = 0;
    for (i = len-1; i > index+1; i--)
    {
        if (str[i] != '0')
        {
            break;
        }
    }

    str = str.substr(0,i+1);
}

void MgUtil::TrimEndingZeros(STRING& str)
{
    int len = (int)str.size();
    int index = (int)str.find(L".");

    int i = 0;
    for (i = len-1; i > index+1; i--)
    {
        if (str[i] != L'0')
        {
            break;
        }
    }

    str = str.substr(0,i+1);
}

inline char AsHex(char ch)
{
  switch(ch)
  {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
      return ch - '0';

  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
      return ch - 'a' + 10;

  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
      return ch - 'A' + 10;
  }
  return 0;
}

void MgUtil::UnEscapeUrl(char* pszBuf, string& dest)
{
    dest.reserve(strlen(pszBuf));

    while(*pszBuf != '\0') // while not end of string
    {
        char ch = *pszBuf;
        switch(ch)
        {
        case '%':               // collapse %xx notation
            pszBuf++;
            ch = AsHex(*pszBuf) << 4;
            pszBuf++;
            ch |= AsHex(*pszBuf);
            break;

        case '+':               // convert + to space
            ch = ' ';
            break;

        default:                // or just copy the character
            break;
        }

        dest += ch;
        pszBuf++;
    }
}

STRING MgUtil::ToLower(CREFSTRING source)
{
    STRING low = source;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    return low;
}

STRING MgUtil::ToUpper(CREFSTRING source)
{
    STRING up = source;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return up;
}

void MgUtil::FormatQualifiedClassName(CREFSTRING schemaName, CREFSTRING className, REFSTRING qualifiedClassName)
{
    qualifiedClassName  = schemaName;
    qualifiedClassName += sm_classNameQualifier;
    qualifiedClassName += className;
}

void MgUtil::ParseQualifiedClassName(CREFSTRING qualifiedClassName, REFSTRING schemaName, REFSTRING className)
{
    STRING::size_type index = qualifiedClassName.find(sm_classNameQualifier);

    if (STRING::npos == index)
    {
        schemaName = L"";
        className  = qualifiedClassName;
    }
    else
    {
        schemaName = qualifiedClassName.substr(0, index);
        className  = qualifiedClassName.substr(index + 1);
    }
}

void MgUtil::PadLeft(REFSTRING str, const size_t numChars, wchar_t ch)
{
    if (numChars > str.size())
        str.insert(0, numChars - str.size(), ch);
}