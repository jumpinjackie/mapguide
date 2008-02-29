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

#include "PlatformBase.h"

//////////////////////////////////////////////////////////////////
// Construct an MgChangeList object
//
MgChangeList::MgChangeList()
{
    m_changes = new MgDisposableCollection();
}

MgChangeList::MgChangeList(CREFSTRING objectId, bool isLayer)
{
    m_changes = new MgDisposableCollection();
    m_objectId = objectId;
    m_isLayer = isLayer;
}

//////////////////////////////////////////////////////////////////
// Get the object id
//
STRING MgChangeList::GetObjectId()
{
    return m_objectId;
}

//////////////////////////////////////////////////////////////////
// Get the name for this change
//
STRING MgChangeList::GetName()
{
    // use the object Id
    return m_objectId;
}

//////////////////////////////////////////////////////////////////
// Tells if this object is a layer (if not, it's a group)
//
bool MgChangeList::IsLayer()
{
    return m_isLayer;
}

//////////////////////////////////////////////////////////////////
// Add a change for this object
//
void MgChangeList::AddChange(MgObjectChange *change)
{
    if (change->GetType() == MgObjectChange::added ||
        change->GetType() == MgObjectChange::removed)
    {
        //if an object is added or removed, it does not matter what other changes happened for this object
        //only keep this last change
        m_changes->Clear();
    }
    else
    {
        if(m_changes->GetCount() > 0)
        {
            //if the action already exists in this changelist, keep only the latest one
            MgObjectChange::ChangeType type = change->GetType();
            for(INT32 i = 0; i < m_changes->GetCount(); i++)
            {
                Ptr<MgObjectChange> prevChange = (MgObjectChange*)m_changes->GetItem(i);
                if(prevChange->GetType() == type)
                {
                    prevChange->SetParam(change->GetParam());
                    return;
                }
            }
        }
    }
    m_changes->Add(change);
}

//////////////////////////////////////////////////////////////////
// Get the number of changes for this object
//
int MgChangeList::GetChangeCount()
{
    return m_changes->GetCount();
}

//////////////////////////////////////////////////////////////////
// Get the type of change at the specified index
//
MgObjectChange* MgChangeList::GetChangeAt(INT32 index)
{
    return (MgObjectChange*)m_changes->GetItem(index);  //addref by collection
}

//////////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
//
void MgChangeList::Serialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgChangeList.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
//
void MgChangeList::Deserialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgChangeList.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgChangeList::CanSetName()
{
    return false;
}

//////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgChangeList::Dispose()
{
    delete this;
}
