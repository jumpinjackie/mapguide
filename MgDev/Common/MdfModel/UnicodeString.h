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

#ifndef UNICODESTRING_H_
#define UNICODESTRING_H_

#include "MdfModel.h"
#include <string>
#include <cstring>
#include <cstdio>

using namespace std;

// Define character type for UTF-16.  This is used by Xerces C++.
#ifdef _NATIVE_WCHAR_T_DEFINED
    typedef wchar_t  XMLCh;
#else
    typedef unsigned short  XMLCh;
#endif

// Define character type for UTF-32.  This is the same as the wchar_t type
// under Linux.  "L" stands for "long" character.  Not a great name...
typedef unsigned int LCh;


// This is a modification of standard char_traits<_Char> template class so
// that it behaves relatively correctly for character types other than
// char and wchar_t.
namespace std
{
template <class _Char>
struct unicode_traits
{
    typedef _Char       char_type;
    typedef int         int_type;
    typedef streampos   pos_type;
    typedef streamoff   off_type;
    typedef mbstate_t   state_type;

    static void
    assign(char_type& __c1, const char_type& __c2)
    { __c1 = __c2; }

    static bool
    eq(const char_type& __c1, const char_type& __c2)
    { return __c1 == __c2; }

    static bool
    lt(const char_type& __c1, const char_type& __c2)
    { return __c1 < __c2; }

    static int
    compare(const char_type* __s1, const char_type* __s2, size_t __n)
    { return memcmp(__s1, __s2, __n * sizeof(char_type)); }

    static size_t
    length(const char_type* __s)
    { size_t len = 0;
      const char_type* __end = __s;
      while (__end != NULL && *__end != 0) ++__end;
      if (__end != __s)
      {
          len = (size_t) (__end - __s);
      }
      return len;
    }

    static const char_type*
    find(const char_type* __s, size_t __n, const char_type& __a)
    {
      const char_type* __end = __s;
      const char_type* __found = NULL;
      while (__end != NULL && *__end != 0 && __end < __s+__n)
      {
          if (*__end == __a) break;
          ++__end;
      }

      if (__end != NULL && *__end == __a)
      {
          __found = __end;
      }

      return __found;
    }

    static char_type*
    move(char_type* __s1, const char_type* __s2, size_t __n)
    { return static_cast<char_type*>(memmove(__s1, __s2, __n*sizeof(char_type))); }

    static char_type*
    copy(char_type* __s1, const char_type* __s2, size_t __n)
    {  return static_cast<char_type*>(memcpy(__s1, __s2, __n*sizeof(char_type))); }

    static size_t 
    _Copy_s(char_type* __dest, size_t __dest_count, const char_type* _off, size_t _Count)
    {
        copy(__dest, _off, _Count);
        return length(__dest);
    }
   
    static char_type*
    assign(char_type* __s, size_t __n, char_type __a)
    {
      for (size_t i=0; i<__n; i++) __s[i] = __a;
      return __s;
    }

    static char_type
    to_char_type(const int_type& __c)
    { return static_cast<char_type>(__c); }

    // To keep both the byte 0xff and the eof symbol 0xffffffff
    // from ending up as 0xffffffff.
    static int_type
    to_int_type(const char_type& __c)
    { return static_cast<int_type>(static_cast<unsigned char>(__c)); }

    static bool
    eq_int_type(const int_type& __c1, const int_type& __c2)
    { return __c1 == __c2; }

    static int_type
    eof() { return static_cast<int_type>(EOF); }

    static int_type
    not_eof(const int_type& __c)
    { return (__c == eof()) ? 0 : __c; }
};
}

// The generic char_traits template may not work correctly for arbitrary character
// types.  This is why char_traits is specialized for char and wchar_t.  To get around this,
// we use the unicode_traits template defined above.  It should be more correct than the
// default stl template expansion.
//
// xstring is a string template class for UTF-16.  UTF-16 is used internally by xerces,
// hence "x"string.  This is similar to wstring on Windows
//
// lstring is a string template for UTF-32.  This should be similar to wstring on Linux.
// Ideally, no one should ever have to use lstring.  But it is here for completeness.


typedef basic_string<XMLCh, unicode_traits<XMLCh> > xstring;
typedef basic_string<LCh, unicode_traits<LCh> > lstring;

// Unicode byte code conversion class.  This class contains static methods to convert
// between UTF-8, UTF-16, and UTF-32.  It requires no additional libraries to function and
// uses stl::basic_string functionality w/ help from the template defined above.
// These routine are thread safe but will probably not work on big endian platforms.
// TODO:  Modify ConvertUTF.c to work on big endian platforms
class MDFMODEL_API UnicodeString
{
public:

    // Platform neutral conversion routines.  Compile time adjustments for Linux and Windows
    static void WideCharToMultiByte(const wchar_t* wideStr, string& mbStr);
    static void MultiByteToWideChar(const char* mbStr, wstring& wideStr);

    static void UTF16toUTF8(const  XMLCh* wideStr, string& mbStr);
    static void UTF8toUTF16(const char* mbStr, xstring& wideStr);
    static xstring UTF8toUTF16(const char* mbStr);

    static void UTF32toUTF8(const LCh* wideStr, string& mbStr);
    static void UTF8toUTF32(const char* mbStr, lstring& wideStr);

    static void UTF32toUTF16(const LCh* wideStr, xstring& mbStr);
    static void UTF16toUTF32(const  XMLCh*, lstring& wideStr);
    static xstring UTF32toUTF16(const LCh* wideStr);
    static lstring UTF16toUTF32(const  XMLCh* wideStr);

    static const int ConversionError = 0xBADCBADC;
};

// Define shortcuts to convert to/from wchar_t strings to XMLCh strings on
// each platform.  Since Windows and Linux have different wchar_t representations,
// these macros are platform dependent
#ifdef _WIN32
// On Windows, wchar_t and XMLCh are both UTF-16.  No conversion is necessary
#   define W2X(str) (const XMLCh*) str
#   define X2W(str) (const wchar_t*) str
#else
// On Linux, wchar_t is UTF-32.  Do the appropriate conversion
#   define W2X(oldstr) (XMLCh*) (UnicodeString::UTF32toUTF16((const LCh*) oldstr).c_str())
#   define X2W(oldstr) (const wchar_t*) UnicodeString::UTF16toUTF32(oldstr).c_str()
#endif

#define X(str) UnicodeString::UTF8toUTF16(str).c_str()

#endif
