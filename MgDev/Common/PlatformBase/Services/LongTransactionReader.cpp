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
#include "LongTransactionData.h"
#include "LongTransactionReader.h"

MG_IMPL_DYNCREATE(MgLongTransactionReader)

//////////////////////////////////////////////////////////////
STRING MgLongTransactionReader::GetName()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->GetName();
}

//////////////////////////////////////////////////////////////
STRING MgLongTransactionReader::GetDescription()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->GetDescription();
}

//////////////////////////////////////////////////////////////
bool MgLongTransactionReader::IsActive()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->IsActive();
}

//////////////////////////////////////////////////////////////
bool MgLongTransactionReader::ReadNext()
{
    bool moreAvailable = false;
    INT32 cnt = m_longTransactionCol.GetCount();
    m_currPos++;

    if (cnt > 0 && m_currPos < cnt)
    {
        moreAvailable = true;
    }

    return moreAvailable;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionReader::Close()
{
    m_longTransactionCol.Clear();
}

//////////////////////////////////////////////////////////////
INT32 MgLongTransactionReader::AddLongTransactionData(MgLongTransactionData* data)
{
    m_longTransactionCol.Add(data);
    return m_longTransactionCol.GetCount();
}

//////////////////////////////////////////////////////////////
MgLongTransactionReader::MgLongTransactionReader() : m_currPos(-1)
{
}

//////////////////////////////////////////////////////////////
MgLongTransactionReader::~MgLongTransactionReader()
{
    this->Close();
}

//////////////////////////////////////////////////////////////
void MgLongTransactionReader::Serialize(MgStream* stream)
{
    INT32 cnt = m_longTransactionCol.GetCount();
    stream->WriteInt32(cnt);
    stream->WriteString(m_providerName);
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(i);
        stream->WriteObject(sData);
    }
}

//////////////////////////////////////////////////////////////
void MgLongTransactionReader::Deserialize(MgStream* stream)
{
    INT32 cnt = 0;
    stream->GetInt32(cnt);
    stream->GetString(m_providerName);
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)stream->GetObject();
        m_longTransactionCol.Add(sData);
    }
}

//////////////////////////////////////////////////////////////
void MgLongTransactionReader::SetProviderName(CREFSTRING providerName)
{
    m_providerName = providerName;
}

//////////////////////////////////////////////////////////////
void MgLongTransactionReader::ToXml(string& str)
{
    // this XML follows the FdoLongTransactionList-1.0.0.xsd schema
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    str += "<FdoLongTransactionList>";
    str += "<ProviderName>" + MgUtil::WideCharToMultiByte(m_providerName) + "</ProviderName>";

    while (this->ReadNext())
    {
        Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
        sData->ToXml(str);
    }

    str += "</FdoLongTransactionList>";
}

//////////////////////////////////////////////////////////////
MgByteReader* MgLongTransactionReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    byteSource->SetMimeType(MgMimeType::Xml);

    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    return SAFE_ADDREF((MgByteReader*)byteReader);
}

//////////////////////////////////////////////////////////////
MgLongTransactionReader* MgLongTransactionReader::GetChildren()
{
    return NULL;
}

//////////////////////////////////////////////////////////////
/// <summary>Provides access to the direct ascendant long transactions for
/// the long transaction being currently read. The caller can use the
/// returned reader to get to the next level of ascendants if required.
/// The returned reader is empty if there are no ascendants for the long
/// transaction</summary>
/// <returns>Returns a long transaction reader with the direct ascendants
/// for the long transaction currently being read.</returns>
MgLongTransactionReader* MgLongTransactionReader::GetParents()
{
    return NULL;
}

//////////////////////////////////////////////////////////////
/// <summary>Retrieves the owner of the long transaction currently being
/// read.</summary>
/// <returns>Returns the owner name.</returns>
STRING MgLongTransactionReader::GetOwner()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->GetOwner();
}

//////////////////////////////////////////////////////////////
/// <summary>Retrieves the creation date of the long transaction currently
/// being read.</summary>
/// <returns>Returns the date when the long transaction was created.</returns>
MgDateTime* MgLongTransactionReader::GetCreationDate()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->GetCreationDate();
}

//////////////////////////////////////////////////////////////
/// <summary>Returns true if the long transaction currently being read is
/// frozen.</summary>
/// <returns>Returns true if the long transaction is frozen.</returns>
bool MgLongTransactionReader::IsFrozen()
{
    Ptr<MgLongTransactionData> sData = (MgLongTransactionData*)m_longTransactionCol.GetItem(m_currPos);
    return sData->IsFrozen();
}
