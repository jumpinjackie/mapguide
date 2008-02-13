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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"

// To undefine GetMessage macro defined in windows
#ifdef _WIN32
    #undef GetMessage
#endif

const STRING CException::CExceptionGetDetails1 = L"Exception occurred in method ";
const STRING CException::CExceptionGetDetails2 = L" at line ";
const STRING CException::CExceptionGetDetails3 = L" in file ";

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
CException::~CException() throw()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a CException with the specified message identifer, method name, line number, file name and reason.
/// </summary>
CException::CException(CREFSTRING methodName, int lineNumber, CREFSTRING fileName, CREFSTRING reason) throw() :
    m_methodName(methodName),
    m_lineNumber(lineNumber),
    m_fileName(fileName),
    m_reason(reason)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Retrieve the formatted exception message details.
/// </summary>
STRING CException::GetDetails() throw()
{
    // Example exception details:
    //   Argument is null.
    //   Exception occurred in method CEnvelope::ExpandToInclude() at line 27 in file Envelope.cpp

    STRING message;

    // Retrieve the exception message.
    wchar_t wsLineNumber[20];
    swprintf(wsLineNumber, 20, L"%i", m_lineNumber);

    message = GetMessage() + L"\n";

    message += CException::CExceptionGetDetails1 + m_methodName;
    message += CException::CExceptionGetDetails2 + wsLineNumber;
    message += CException::CExceptionGetDetails3 + m_fileName;

    message += L"\n";

    return message;
}
