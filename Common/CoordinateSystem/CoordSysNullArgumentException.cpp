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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"

const STRING CNullArgumentException::MessageText = L"Argument is null: %ls";

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a CNullArgumentException with the specified method name, line number and file name.
/// </summary>
CNullArgumentException::CNullArgumentException(CREFSTRING methodName, int lineNumber, CREFSTRING fileName, CREFSTRING reason) throw() :
    CException(methodName, lineNumber, fileName, reason)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
CNullArgumentException::~CNullArgumentException() throw()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Retrieve the formatted exception message.
/// </summary>
STRING CNullArgumentException::GetMessage() throw()
{
    wchar_t message[255];
    swprintf(message, 255, CNullArgumentException::MessageText.c_str(), m_reason.c_str());
    return message;
}
