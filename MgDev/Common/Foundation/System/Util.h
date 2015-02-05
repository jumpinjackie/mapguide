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

#ifndef MGUTIL_H_
#define MGUTIL_H_


#include "LinuxMemoryStatus.h"
#include <sstream>
#include <iostream>

// Math-related macros

#define DoubleMaxValue  DBL_MAX
#define DoubleMinValue  DBL_MIN
#define ROUND(x) (int)(floor(x+0.5))

#define DoubleNegativeInfinity numeric_limits<double>::infinity()
#define DoublePositiveInfinity numeric_limits<double>::infinity()

// STL-related structs

/// \cond INTERNAL
struct MgWcsICmpAscending
{
    bool operator()(const wstring& s1, const wstring& s2) const
    {
        return (_wcsicmp(s1.c_str(), s2.c_str()) < 0);
    }
/// \endcond
};

/// \cond INTERNAL
struct MgWcsICmpDescending
{
    bool operator()(const wstring& s1, const wstring& s2) const
    {
        return (_wcsicmp(s1.c_str(), s2.c_str()) > 0);
    }
/// \endcond
};

class MgConfiguration;

/// \cond INTERNAL
//////////////////
/// \brief
/// A utility class containing methods needed for other part of
/// code
class MG_FOUNDATION_API MgUtil
{
    DECLARE_CLASSNAME(MgUtil)

INTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a character to hex value
    ///
    /// \param ch
    /// character to convert
    ///
    /// \return
    /// String containing hex value
    ///
    static string Char2Hex(char ch);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a reader from string
    ///
    /// \param s
    /// String to convert
    ///
    /// \return
    /// ByteReader
    ///
    static MgByteReader* GetByteReader(string &s, STRING* mimeType = NULL);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a String from bytereader
    ///
    /// \param byteReader
    /// ByteReader from which string to be returned
    ///
    /// \return
    /// String formed from ByteReader in hexa decimal
    ///
    static string GetStringFromReader(MgByteReader* byteReader);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a Text string from bytereader
    ///
    /// \param byteReader
    /// ByteReader from which string to be returned
    ///
    /// \return
    /// String formed from ByteReader
    ///
    static string GetTextFromReader(MgByteReader* byteReader);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a wide character string to a multibyte character string.
    ///
    /// \param wcStr
    /// Wide character string.
    ///
    /// \return
    /// Multibyte character string.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    ///
    static string WideCharToMultiByte(CREFSTRING wcStr);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a wide character string to a multibyte character string.
    ///
    /// \param wcStr
    /// Wide character string to convert.
    /// \param mbStr
    /// Multibyte character string to receive.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    ///
    static void WideCharToMultiByte(CREFSTRING wcStr, string& mbStr);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a wide character buffer to a multibyte character buffer.
    ///
    /// \param wcBuf
    /// Wide character buffer.
    ///
    /// \return
    /// Multibyte character buffer.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    /// \note
    /// The caller is responsible to de-allocate the returned buffer.
    ///
    static char* WideCharToMultiByte(const wchar_t* wcBuf);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a multibyte character string to a wide character string.
    ///
    /// \param mbStr
    /// Multibyte character string.
    ///
    /// \return
    /// Wide character string.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    ///
    static STRING MultiByteToWideChar(const string& mbStr);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a multibyte character string to a wide character string
    ///
    /// \param mbStr
    /// Multibyte character string to convert.
    /// \param wcStr
    /// Wide character string to receive.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    ///
    static void MultiByteToWideChar(const string& mbStr, REFSTRING wcStr);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a multibyte character buffer to a wide character buffer.
    ///
    /// \param mbBuf
    /// Multibyte character buffer.
    ///
    /// \return
    /// Wide character buffer.
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgOutOfMemoryException
    ///
    /// \note
    /// The caller is responsible to de-allocate the returned buffer.
    ///
    static wchar_t* MultiByteToWideChar(const char* mbBuf);
    //////////////////////////////////////////////
    /// \brief
    /// TrimRight()
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static STRING TrimRight(CREFSTRING source, CREFSTRING t = L" ");

    //////////////////////////////////////////////
    /// \brief
    /// TrimLeft
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static STRING TrimLeft(CREFSTRING source , CREFSTRING t = L" ");

    //////////////////////////////////////////////
    /// \brief
    /// TrimLeft and TrimRight
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static STRING Trim(CREFSTRING source , CREFSTRING t = L" ");

    //////////////////////////////////////////////
    /// \brief
    /// TrimRight()
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static string TrimRight(const string& source, const string& t = " ");

    //////////////////////////////////////////////
    /// \brief
    /// TrimLeft
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static string TrimLeft(const string& source, const string& t = " ");

    //////////////////////////////////////////////
    /// \brief
    /// TrimLeft and TrimRight
    ///
    /// \param source
    /// source string to trim
    /// \param t
    /// char needed for trim (default is space)
    ///
    /// \return
    /// trimmed string
    ///
    static string Trim(const string& source, const string& t = " ");

    //////////////////////////////////////////////
    /// \brief
    /// Check for the reserved characters
    ///
    /// \exception MgInvalidArgumentException
    ///
    static void CheckReservedCharacters(CREFSTRING str, CREFSTRING reservedChars, bool anyOne = true);
    //////////////////////////////////////////////
    /// \brief
    /// Check for spaces at beginning and end of string
    ///
    /// \exception MgInvalidArgumentException
    ///
    static void CheckSpacesAtBeginEnd(CREFSTRING str);
    //////////////////////////////////////////////
    /// \brief
    /// Check for slashes at beginning and end of string
    ///
    /// \exception MgInvalidArgumentException
    ///
    static void CheckSlashAtBeginEnd(CREFSTRING str);
    //////////////////////////////////////////////
    /// \brief
    /// Check for characters at beginning and end of string
    ///
    /// \exception MgInvalidArgumentException
    ///
    static void CheckBeginEnd(CREFSTRING str, CREFSTRING sChar);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Perform Cross Site Scripting Attack validations against the specified
    /// string.
    ///
    static void CheckXss(CREFSTRING str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Perform some encoding on the specified string to prevent Cross Site
    /// Scripting attacks.
    ///
    static STRING EncodeXss(CREFSTRING str);

    //////////////////////////////////////////////
    /// \brief
    /// Creates an object based on its class Id
    ///
    /// \exception MgNullReferenceException
    ///
    static MgObject* CreateMgObject(INT32 classId);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a string to a boolean.
    ///
    /// \return
    /// returns boolean value.
    ///
    static bool StringToBoolean(CREFSTRING str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a string to a 32-bit integer.
    ///
    /// \return
    /// returns INT32 value.
    ///
    static INT32 StringToInt32(CREFSTRING str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a string to a 64-bit integer.
    ///
    /// \return
    /// returns INT64 value.
    ///
    static INT64 StringToInt64(CREFSTRING str);

    //////////////////////////////////////////////
    /// \brief
    /// Converts a string to a 64-bit floating-point number.
    ///
    /// \return
    /// returns double value.
    ///
    static double StringToDouble(CREFSTRING str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a string to a boolean.
    ///
    /// \return
    /// returns boolean value.
    ///
    static bool StringToBoolean(const string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an Universal Unique IDentifier.
    ///
    /// \param uuid
    /// Generated UUID
    ///
    /// \exception MgOutOfMemoryException
    ///
    ///
    static void GenerateUuid(REFSTRING uuid);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Replace an existing text pattern with a new text pattern in a string
    ///
    /// \param str
    /// Source string
    /// \param oldValue
    /// Text pattern to be replaced
    /// \param newValue
    /// Text pattern to replace with
    ///
    /// \return
    /// Modified string
    ///
    static STRING ReplaceString(STRING str, const wchar_t* oldValue, const wchar_t* newValue);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Replace an existing text pattern with a new text pattern in a string.
    ///
    /// \param oldValue
    /// Text pattern to be replaced
    /// \param newValue
    /// Text pattern to replace with
    /// \param str
    /// String to be modified
    /// \param replacements
    /// Number of replacements needed
    /// If it is less than zero (e.g. -1), then all possible replacements will be done.
    ///
    /// \return
    /// Number of modifications
    ///
    static INT32 ReplaceString(CREFSTRING oldValue, CREFSTRING newValue, REFSTRING str, INT32 replacements);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Replace an existing text pattern with a new text pattern in a string.
    ///
    /// \param oldValue
    /// Text pattern to be replaced
    /// \param newValue
    /// Text pattern to replace with
    /// \param str
    /// String to be modified
    /// \param replacements
    /// Number of replacements needed
    /// If it is less than zero (e.g. -1), then all possible replacements will be done.
    ///
    /// \return
    /// Number of modifications
    ///
    static INT32 ReplaceString(const string& oldValue, const string& newValue, string& str, INT32 replacements);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Replaces &, >, <, ", ' to &amp;,&gt;,&lt;,&quot;,&apos; for xml
    ///
    /// \param str
    /// str - String value for xml element
    ///
    /// \return
    /// Converted string
    ///
    static STRING ReplaceEscapeCharInXml(CREFSTRING str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Unescape of url-encoded text strings
    ///
    /// \param pszBuf
    /// Escaped string
    /// \param dest
    /// Output string for unescaped result
    ///
    /// \return
    /// Nothing
    ///
    static void UnEscapeUrl(char* pszBuf, string& dest);

    //////////////////////////////////////////////
    /// \brief
    /// Convert a string to lowercase
    ///
    /// \param source
    /// source string to convert
    ///
    /// \return
    /// string converted to lowercase
    ///
    /// Be cautious using this function with localized strings.
    ///
    static STRING ToLower(CREFSTRING source);

    //////////////////////////////////////////////
    /// \brief
    /// Convert a string to uppercase
    ///
    /// \param source
    /// source string to convert
    ///
    /// \return
    /// string converted to uppercase
    ///
    /// Be cautious using this function with localized strings.
    ///
    static STRING ToUpper(CREFSTRING source);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an Universal Unique IDentifier.
    ///
    /// \param uuid
    /// Generated UUID
    ///
    /// \exception MgOutOfMemoryException
    ///
    ///
    static void GenerateUuid(string& uuid);
    static STRING GetResourceMessage(CREFSTRING section, CREFSTRING messageId);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the memory status on Linux OS.
    ///
    /// \param pMemoryStatus
    /// pMemoryStatus - Pointer to MgLinuxMemoryStatus structure
    ///
    /// \return
    /// true on success, false otherwise
    ///
    /// \exception MgFileNotFoundException
    /// \exception MgPlatformNotSupportedException
    ///
    ///
    static bool GetLinuxMemoryStatus(MgLinuxMemoryStatus* pMemoryStatus);

    static void PadLeft(REFSTRING str, const size_t numChars, wchar_t ch);

    static void Int32ToLocaleSpecificString(INT32 val, string& str);

    static void Int32ToString(INT32 val, string& str);
    static void Int32ToString(INT32 val, STRING& str);
    static void Int32ToHexString(INT32 val, string& str);
    static void UInt32ToString(UINT32 val, string& str);
    static void UInt32ToString(UINT32 val, STRING& str);
    static void Int64ToString(INT64 val, string& str);
    static void Int64ToString(INT64 val, STRING& str);
    static void SingleToString(float val, string& str);
    static void SingleToString(float val, STRING& str);
    static void DoubleToString(double val, string& str);
    static void DoubleToString(double val, STRING& str);

    static bool ValuesEqual(double value1, double value2,
        double tolerance = MgUtil::DefaultCompareTolerance,
        bool output = true);

    static void TrimEndingZeros(string& str);
    static void TrimEndingZeros(STRING& str);

    static void FormatQualifiedClassName(CREFSTRING schemaName, CREFSTRING className, REFSTRING qualifiedClassName);
    static void ParseQualifiedClassName(CREFSTRING qualifiedClassName, REFSTRING schemaName, REFSTRING className);

    static const double DefaultCompareTolerance;
    static const STRING LinuxFilesystemMeminfo;
    static const STRING LinuxFilesystemStat;

    static const STRING sm_xssReservedCharacters;

    static const STRING sm_classNameQualifier;
    static const STRING sm_classPropertyQualifier;

private:

    static bool StaticInitialize();
    static void InitializeUuidGenerator();

    typedef void*    (*MgStandardObjectCreateFunc)(int);
    static bool uuidInitialized;
/// \endcond
};


// Defines a macro for NULL check which would throw exception if value is null
#define CHECKNULL(pointer, methodname)      \
if (pointer == NULL)     \
{                        \
    MgStringCollection args; \
    args.Add(L###pointer); \
    throw new MgNullReferenceException(methodname, \
                                       __LINE__, __WFILE__, NULL, L"MgNullPointer", &args); \
}

#define CHECKARGUMENTNULL(pointer, methodname)      \
if (pointer == NULL)     \
{                        \
    MgStringCollection args; \
    args.Add(L###pointer); \
    throw new MgNullArgumentException(methodname, \
                                       __LINE__, __WFILE__, NULL, L"MgNullArgument", &args); \
}

#define CHECKARGUMENTEMPTYSTRING(str, methodname)      \
if (str.empty())     \
{                        \
    MgStringCollection args; \
    args.Add(L###str); \
    throw new MgInvalidArgumentException(methodname, \
                                         __LINE__, __WFILE__, NULL, L"MgEmptyStringArgument", &args); \
}

#define MG_CHECK_RANGE(value, min, max, methodName)                           \
    if (value < min || value > max)                                           \
    {                                                                         \
        if (NULL != methodName)                                               \
        {                                                                     \
            std::wostringstream minStr;                                       \
            minStr << min;                                                    \
            std::wostringstream maxStr;                                       \
            maxStr << max;                                                    \
            MgStringCollection args;                                          \
            args.Add(minStr.str());                                           \
            args.Add(maxStr.str());                                           \
            args.Add(L###value);                                                \
            throw new MgArgumentOutOfRangeException(                          \
                methodName, __LINE__, __WFILE__, NULL, L"MgArgumentOutOfRange", &args); \
        }                                                                     \
        else if (value < min)                                                 \
        {                                                                     \
            value = min;                                                      \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            value = max;                                                      \
        }                                                                     \
    }                                                                         \

#define MG_FILE_IO_EXCEPTION_ADD_ERRNO()                  \
    MgStringCollection errNoArgs;                         \
    std::string mbErrNoDesc;                              \
    mbErrNoDesc += ACE_OS::strerror(errno);               \
    STRING wErrNoDesc;                                    \
    MgUtil::MultiByteToWideChar(mbErrNoDesc, wErrNoDesc); \
    STRING wErrNo;                                        \
    MgUtil::Int32ToString(errno, wErrNo);                 \
    errNoArgs.Add(wErrNo);                                \
    errNoArgs.Add(wErrNoDesc)

#ifdef _WIN32
#define MG_TCHAR_TO_WCHAR(x) ((wstring)x).c_str()
#define MG_TCHAR_TO_CHAR(x) MgUtil::WideCharToMultiByte((wstring)x).c_str()
#define MG_WCHAR_TO_TCHAR(x) ((wstring)x).c_str()
#else
#define MG_TCHAR_TO_WCHAR(x) MgUtil::MultiByteToWideChar((string)x).c_str()
#define MG_TCHAR_TO_CHAR(x) x
#define MG_WCHAR_TO_TCHAR(x) MgUtil::WideCharToMultiByte((wstring)x).c_str()
#endif

#define MG_WCHAR_TO_CHAR(x) MgUtil::WideCharToMultiByte((wstring)x).c_str()

#endif // MGUTIL_H_

