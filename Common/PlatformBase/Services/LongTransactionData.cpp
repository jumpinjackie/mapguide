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
//

#include "PlatformBase.h"
#include "LongTransactionData.h"

MG_IMPL_DYNCREATE(MgLongTransactionData)

//////////////////////////////////////////////////////////////
MgLongTransactionData::MgLongTransactionData()
{
    m_name = L"";
    m_desc = L"";
    m_owner = L"";
    m_dateTime = NULL;
    m_isActive = false;
    m_isFrozen = false;
}

//////////////////////////////////////////////////////////////
MgLongTransactionData::~MgLongTransactionData()
{
}

//////////////////////////////////////////////////////////////
STRING MgLongTransactionData::GetName()
{
    return m_name;
}

//////////////////////////////////////////////////////////////
STRING MgLongTransactionData::GetDescription()
{
    return m_desc;
}

//////////////////////////////////////////////////////////////
STRING MgLongTransactionData::GetOwner()
{
    return m_owner;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetOwner(CREFSTRING owner)
{
    m_owner = owner;
}

//////////////////////////////////////////////////////////////
MgDateTime* MgLongTransactionData::GetCreationDate()
{
    return SAFE_ADDREF((MgDateTime*)m_dateTime);
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetCreationDate(MgDateTime* dateTime)
{
    m_dateTime = SAFE_ADDREF(dateTime);
}

//////////////////////////////////////////////////////////////
bool MgLongTransactionData::IsFrozen()
{
    return m_isFrozen;
}

//////////////////////////////////////////////////////////////
bool MgLongTransactionData::IsActive()
{
    return m_isActive;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetName(CREFSTRING name)
{
    m_name = name;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetDescription(CREFSTRING desc)
{
    m_desc = desc;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetActiveStatus(bool active)
{
    m_isActive = active;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::SetFrozenStatus(bool frozen)
{
    m_isFrozen = frozen;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::Serialize(MgStream* stream)
{
    stream->WriteString(m_name);
    stream->WriteString(m_desc);
    stream->WriteString(m_owner);

    stream->WriteObject(m_dateTime);

    stream->WriteBoolean(m_isActive);
    stream->WriteBoolean(m_isFrozen);
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::Deserialize(MgStream* stream)
{
    stream->GetString(m_name);
    stream->GetString(m_desc);
    stream->GetString(m_owner);

    m_dateTime = (MgDateTime*)stream->GetObject();

    stream->GetBoolean(m_isActive);
    stream->GetBoolean(m_isFrozen);
}

//////////////////////////////////////////////////////////////
void MgLongTransactionData::ToXml(string& str)
{
    str += "<LongTransaction IsActive=";

    // Whether it is active or not
    if (this->m_isActive)
    {
        str += "\"true\"";
    }
    else
    {
        str += "\"false\"";
    }

    // Whether it is active or not
    if (this->m_isFrozen)
    {
        str += " IsFrozen=\"true\">";
    }
    else
    {
        str += " IsFrozen=\"false\">";
    }

    str += "<Name>" + MgUtil::WideCharToMultiByte(m_name) + "</Name>";
    str += "<Description>" + MgUtil::WideCharToMultiByte(m_desc) + "</Description>";
    str += "<Owner>" + MgUtil::WideCharToMultiByte(m_owner) + "</Owner>";
    str += "<CreationDate>";
    if (m_dateTime != NULL)
        str += MgUtil::WideCharToMultiByte(m_dateTime->ToString());
    str += "</CreationDate>";

    str += "</LongTransaction>";
}

//////////////////////////////////////////////////////////////
bool MgLongTransactionData::CanSetName()
{
    return true;
}
