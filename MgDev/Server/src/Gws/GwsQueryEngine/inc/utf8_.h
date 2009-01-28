//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

//converts utf8 string to unicode
//returns length of unicode string (excluding null character)
//returns -1 if invalid character code was encountered
int ut_utf8_to_unicode(const char* str_in, size_t in_size, wchar_t* Wtext, size_t out_size);

//converts unicode string to utf8
//returns number of bytes in output utf8 string (excluding trailing null char)
//returns -1 if invalid character encountered
int ut_utf8_from_unicode(const wchar_t* Wtext, size_t in_size, char* str_out, int out_size);

char* gws_mbscat(char *strDestination, const char *strSource);
