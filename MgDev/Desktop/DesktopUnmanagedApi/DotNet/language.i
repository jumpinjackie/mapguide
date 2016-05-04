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

%typemap(csclassmodifiers) SWIGTYPE "public partial"

///////////////////////////////////////////////////////////
// STRINGPARAM "in" typemap
// Marshal a C++ style string to a wstring
// Allocate temporary memory only when required, otherwise
// use a buffer on the stack
//
%typemap(in) STRINGPARAM
{
    $1 = (STRINGPARAM) ($input ? $input : L"");
}

///////////////////////////////////////////////////////////
// STRING "out" typemap
// Marshal a string returned by C++ to CSharp
//
%typemap(out) STRING
{
    $result = SWIG_csharp_string_callback((int)(result.length()+1)*sizeof(wchar_t));
    wcscpy((wchar_t*)$result, result.c_str());
}

///////////////////////////////////////////////////////////
// BYTE_ARRAY_OUT "cstype" typemap
// Type substitution in .NET and proxy code
//
%typemap(cstype) BYTE_ARRAY_OUT "Byte[]"
%typemap(imtype) BYTE_ARRAY_OUT "Byte[]"
%typemap(ctype)  BYTE_ARRAY_OUT "unsigned char*"

///////////////////////////////////////////////////////////
// BYTE_ARRAY_IN "cstype" typemap
// Type substitution in .NET and proxy code
//
%typemap(cstype) BYTE_ARRAY_IN "Byte[]"
%typemap(imtype) BYTE_ARRAY_IN "Byte[]"
%typemap(ctype)  BYTE_ARRAY_IN "unsigned char*"

