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

#include "PlatformBase.h"
#include <assert.h>


MG_IMPL_DYNCREATE(MgWarnings);


MgWarnings::MgWarnings()
{
    m_arguments = new MgStringCollection();
}

 void MgWarnings::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Adds collection of messages to existing messages
///<param name="arguments">Collection of messgages for warning</param>
///</summary>
///
void MgWarnings::AddMessages(MgStringCollection* pArguments)
{
    if (pArguments)
    {
        INT32 nIndex = 0;
        for (nIndex = 0; nIndex < pArguments->GetCount(); nIndex++)
        {
            m_arguments->Add(pArguments->GetItem(nIndex));
        }
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Adds collection of messages to existing messages
///<param name="arguments">Collection of messgages for warning</param>
///</summary>
///
 void MgWarnings::AddMessage(CREFSTRING message)
{
    m_arguments->Add(message);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Adds collection of messages to existing messages
///<param name="arguments">Collection of messgages for warning</param>
///</summary>
///
MgStringCollection* MgWarnings::GetMessages()
{
    return m_arguments;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgWarnings::~MgWarnings()
{
}

INT32 MgWarnings::GetCount()
{
    return m_arguments->GetCount();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgWarnings::Serialize(MgStream* stream)
{
    m_arguments->Serialize(stream);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgWarnings::Deserialize(MgStream* stream)
{
    m_arguments->Deserialize(stream);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
 INT32 MgWarnings::GetClassId() { return m_cls_id; }
