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

#include "Foundation.h"


MgConfigurationSectionCollection::MgConfigurationSectionCollection()
{
    m_collection = new MgDisposableCollection();
}


MgConfigurationSectionCollection::~MgConfigurationSectionCollection()
{
}


void MgConfigurationSectionCollection::Dispose()
{
    delete this;
}


void MgConfigurationSectionCollection::Add(MgConfigurationSection* value)
{
    m_collection->Add(value);
}


void MgConfigurationSectionCollection::Insert(INT32 index, MgConfigurationSection* value)
{
    m_collection->Insert(index, value);
}


bool MgConfigurationSectionCollection::Remove(MgConfigurationSection* value)
{
    bool removed = true;
    try
    {
        m_collection->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}


void MgConfigurationSectionCollection::SetItem(INT32 index, MgConfigurationSection* value)
{
    m_collection->SetItem(index, value);
}


MgConfigurationSection* MgConfigurationSectionCollection::GetItem(INT32 index) const
{
    return (MgConfigurationSection*)m_collection->GetItem(index);
}


INT32 MgConfigurationSectionCollection::GetCount() const
{
    return m_collection->GetCount();
}


void MgConfigurationSectionCollection::Clear()
{
    m_collection->Clear();
}


void MgConfigurationSectionCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}


bool MgConfigurationSectionCollection::Contains(const MgConfigurationSection* value)
{
    return m_collection->Contains(value);
}


INT32 MgConfigurationSectionCollection::IndexOf(CREFSTRING sectionPath) const
{
    MG_TRY()

    Ptr<MgConfigurationSection> section;
    INT32 numSections = m_collection->GetCount();

    INT32 i;
    for (i = 0; i < numSections; i++)
    {
        section = this->GetItem(i);
        if (sectionPath == section->GetSectionPath())
        {
            return i;
        }
    }

    MG_CATCH_AND_THROW(L"MgConfigurationSectionCollection.IndexOf")

    return -1;
}


INT32 MgConfigurationSectionCollection::IndexOf(const MgConfigurationSection* value) const
{
    return m_collection->IndexOf(value);
}
