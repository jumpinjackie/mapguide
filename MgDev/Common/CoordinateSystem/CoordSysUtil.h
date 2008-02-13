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

#ifndef _CCOORDINATESYSTEMUTIL_H_
#define _CCOORDINATESYSTEMUTIL_H_

#include <algorithm>

#ifdef _WIN32
#include "Windows.h"
// To undefine GetMessage macro defined in windows
#undef GetMessage
#endif

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

inline STRING TrimLeft(CREFSTRING source , CREFSTRING t = L" ")
{
    STRING str = source;
    return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
}

inline STRING TrimRight(CREFSTRING source, CREFSTRING t = L" ")
{
    STRING str = source;
    return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
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

inline STRING ToUpper( CREFSTRING source )
{
    STRING up = source;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return up;
}

#endif //_CCOORDINATESYSTEMUTIL_H_
