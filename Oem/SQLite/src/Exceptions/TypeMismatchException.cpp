//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "SQLiteExceptions.h"

IMPLEMENT_EXCEPTION_DEFAULTS(TypeMismatchException, SqliteException)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a TypeMismatchException with the specified method name, line number and file name.
/// </summary>
TypeMismatchException::TypeMismatchException(CREFSTRING methodName, UINT32 lineNumber, CREFSTRING fileName, CREFSTRING message) throw():
    SqliteException(methodName, lineNumber, fileName, message)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
TypeMismatchException::~TypeMismatchException() throw()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Retrieve the formatted exception message for the specified locale.
/// <param name="nLocale">The locale identifier to format the message text.</param>
/// </summary>
///
STRING TypeMismatchException::GetMessage() throw()
{
    STRING wsMessage = L"Type mismatch.\n";
    return wsMessage;
}
