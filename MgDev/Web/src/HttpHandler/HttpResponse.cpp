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

#include "HttpHandler.h"

/// <summary>
/// Constructor. Initialize member variables.
/// </summary>
MgHttpResponse::MgHttpResponse()
{
    m_header = new MgHttpHeader();
    m_result = new MgHttpResult();
}

/// <summary>
/// Makes available the pointer to header.
/// User will use header class instance to retrieve
/// header information for a response.
/// </summary>
/// <returns>Pointer to MgHttpHeader class</returns>
MgHttpHeader* MgHttpResponse::GetHeader()
{
    return SAFE_ADDREF((MgHttpHeader*)m_header);
}

/// <summary>
/// Makes available the pointer to MgHttpResult.
/// User will retrieve the actual contents
/// from this result instance.
/// </summary>
/// <returns>Pointer to MgHttpResult or NULL</returns>
MgHttpResult* MgHttpResponse::GetResult()
{
    return SAFE_ADDREF((MgHttpResult*)m_result);
}

void MgHttpResponse::Dispose()
{
    delete this;
}

INT32 MgHttpResponse::GetClassId()
{
    return m_cls_id;
}

MgHttpResponse::~MgHttpResponse()
{
}
