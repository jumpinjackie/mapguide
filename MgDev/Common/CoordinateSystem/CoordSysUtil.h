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

#ifndef _CCOORDINATESYSTEMUTIL_H_
#define _CCOORDINATESYSTEMUTIL_H_

#define __CPP__ 1
#ifdef min
#undef min
#endif
#include "cs_map.h"
#include "cs_NameMapper.hpp"
#include "cs_WktObject.hpp"
#include "cs_wkt.h"
#include "CsmapVersion.h"

#include <algorithm>

#ifdef _WIN32
#include "windows.h"
#endif

// Mentor specific information
extern "C" struct cs_Prjtab_ cs_Prjtab[];   // Mentor projection table
extern "C" struct cs_Unittab_ cs_Unittab[]; // Mentor unit table

namespace CSLibrary
{
const double ZERO_SCALE = 1e-9;  //-- unit scale smaller than this is considered zero
const double ZERO_VALUE = 1e-12; //-- value smaller than this is considered zero
}

// Represent NAN for double
#define DoubleNan std::numeric_limits<double>::quiet_NaN()

// Checks whether value is NAN or not
#ifdef _WIN32
#define IsDoubleNan(x)  _isnan(x)
#else
#define IsDoubleNan(x)  isnan(x)
#endif

#ifndef _WIN32
#define wmemcpy memcpy
#endif

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified string (e.g. pathname) ends with a forward or
/// backward slash.
/// </summary>
///----------------------------------------------------------------------------
inline bool EndsWithSlash(CREFSTRING str)
{
    size_t index = str.find_last_not_of(L"\\/");

    return ((str.length() - 1) != index);
}

///----------------------------------------------------------------------------
/// <summary>
/// Appends a forward slash to the end of the specified path if it does exist.
/// </summary>
///----------------------------------------------------------------------------
inline void AppendSlashToEndOfPath(REFSTRING path)
{
    if (!EndsWithSlash(path))
    {
        path.append(L"/");
    }
}

inline char *
Convert_Wide_To_UTF8 (const wchar_t *wstr)
{
  // Short circuit null pointer case
  if (wstr == 0)
    return 0;

  return MgUtil::WideCharToMultiByte(wstr);
}

inline wchar_t *
Convert_UTF8_To_Wide (const char *str)
{
  // Short circuit null pointer case
  if (str == 0)
    return 0;

  // Check input, discard invalid charactors
  size_t len = strlen(str);
  char* tempstr = new char[len+1];
  size_t pos = 0;
  for (size_t i = 0; i < len; i++)
  {
      if (str[i] > 0)
          tempstr[pos++] = str[i];
  }
  tempstr[pos] = '\0';

  wchar_t* ret = MgUtil::MultiByteToWideChar(tempstr);
  delete []tempstr;
  return ret;
}

inline char *
Convert_Wide_To_Ascii (const wchar_t *wstr)
{
  // Short circuit null pointer case
  if (wstr == 0)
    return 0;

# if defined (_WIN32)
  UINT cp = GetACP ();
  int len = ::WideCharToMultiByte (cp,
                                   0,
                                   wstr,
                                   -1,
                                   0,
                                   0,
                                   0,
                                   0);
# else  /* _WIN32 */
  int len = ::wcslen (wstr) + 1;
# endif /* _WIN32 */

  char *str = new char[len];

# if defined (_WIN32)
  ::WideCharToMultiByte (cp, 0, wstr, -1, str, len, 0, 0);
# else /* _WIN32 */
  for (int i = 0; i < len; i++)
    {
      wchar_t *t = const_cast<wchar_t *> (wstr);
      str[i] = static_cast<char> (*(t + i));
    }
# endif /* _WIN32 */
  return str;
}

inline wchar_t *
Convert_Ascii_To_Wide (const char *str)
{
  // Short circuit null pointer case
  if (str == 0)
    return 0;

# if defined (_WIN32)
  UINT cp = GetACP ();
  int len = ::MultiByteToWideChar (cp, 0, str, -1, 0, 0);
# else /* _WIN32 */
  int len = strlen (str) + 1;
# endif /* _WIN32 */

  wchar_t *wstr = new wchar_t[len];

# if defined (_WIN32)
  ::MultiByteToWideChar (cp, 0, str, -1, wstr, len);
# else /* _WIN32 */
  for (int i = 0; i < len; i++)
    {
      char *t = const_cast<char*> (str);
      wstr[i] = static_cast<wchar_t> (*(t + i));
    }
# endif /* _WIN32 */
  return wstr;
}

inline STRING ToLower( CREFSTRING source )
{
    STRING low = source;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    return low;
}

inline STRING ToUpper( CREFSTRING source )
{
    STRING up = source;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return up;
}

//Tests a string for legality, based on size.  Returns false
//if the string is a null pointer, or if it is greater than
//nMaxSize bytes (including terminating null) when converted
//to a multibyte character string.
bool IsLegalString(const wchar_t* kpStr, UINT32 unMaxSize);

//Tests two floating-point numbers to see if they're "equal"
//to within a very tiny value.  Returns true if so.  This
//function is mainly used in asserts, to verify that certain
//calculated results are what we expect them to be.
//
//The default value for epsilon is somewhat arbitrary.  The intent
//is "equal to within the limits of floating-point accuracy."
//The values this function is used with are generally no
//bigger than 1e+4 and no smaller than 1e-4, so this seems
//to be a reasonable value.
bool FloatEqual(double d1, double d2, double dEpsilon=1.0E-12);

enum CsDictionaryOpenMode
{
    Closed = 0,
    Read = 1,
    Write = 2
};

//Gets the mode and protection codes with which to open a file,
//given a INT32 value.
void GetFileModes(CsDictionaryOpenMode nOpenMode, char* szMode);

enum EFileValidity
{
    kFileIsValid,
    kFileInvalidEmptyString,
    kFileInvalidPath,
    kFileInvalidDoesNotExist,
    kFileInvalidNotADir,
    kFileInvalidIsADir,
    kFileInvalidCantWrite,
    kFileInvalidCantRead
};
bool ValidateFile(
    const wchar_t *kpFile, //the file name to validate
    bool bExists,       //if true, function fails if file doesn't exist
    bool bIsDir,        //if true, function fails if file isn't a dir
    bool bCanWrite,     //if true, function fails if file isn't writable
    EFileValidity *pReason = NULL); //if not NULL store result code

bool IsValidDictionaryName(CREFSTRING sName);
void ThrowFileError(const wchar_t* wszMethodName, CREFSTRING sPathName, EFileValidity reason);

bool CanWriteToDir(const wchar_t *kpDir);
bool CanWriteToFile(const wchar_t *kpFile);
bool CanReadFromDir(const wchar_t *kpFile);
bool CanReadFromFile(const wchar_t *kpFile);

//*****************************************************************************
class CCsNumericDotLocale
{
public:
    CCsNumericDotLocale();
    ~CCsNumericDotLocale();

private:
    char *m_pszCurLocale;
};

// Returns approximate remaining memory for the current process
INT64 GetAvailableMemory();

#endif //_CCOORDINATESYSTEMUTIL_H_
