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

// Constructs an empty MgLayerCollection object.
//
MgReadOnlyLayerCollection::MgReadOnlyLayerCollection()
{
    m_layers = new MgDisposableCollection();
}

//////////////////////////////////////////////////
/// <summary>
/// Adds a layer to the end of the collection.
/// </summary>
void MgReadOnlyLayerCollection::Add(MgLayerBase* value)
{
    m_layers->Add(value);
}

///<summary>
/// Destruct a MgLayerCollection object
///</summary>
///
///<returns>Nothing</returns>
MgReadOnlyLayerCollection::~MgReadOnlyLayerCollection()
{
    for (int i=0; i<m_layers->GetCount(); i++)
    {
        m_layers->SetItem(i, NULL);
    }
}

////////////////////////////////////////////////
/// <summary>
/// Gets the number of layers in the collection.
/// </summary>
INT32 MgReadOnlyLayerCollection::GetCount()
{
    return m_layers->GetCount();
}

//////////////////////////////////////////////////////////
/// <summary>
/// Gets the layer at the specified index.
/// </summary>
MgLayerBase* MgReadOnlyLayerCollection::GetItem(INT32 index)
{
    MG_TRY()
    return dynamic_cast<MgLayerBase*>(m_layers->GetItem(index));
    MG_CATCH_AND_THROW(L"MgReadOnlyLayerCollection.GetItem");
    return NULL;
}

////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the collection contains the specified layer.
/// </summary>
bool MgReadOnlyLayerCollection::Contains(MgLayerBase* value)
{
    MG_TRY()
    return m_layers->Contains(value);
    MG_CATCH_AND_THROW(L"MgReadOnlyLayerCollection.Contains");
    return false;
}

/////////////////////////////////////////////////////////////
/// <summary>
/// Gets the index of the specified layer in the collection.
/// </summary>
INT32 MgReadOnlyLayerCollection::IndexOf(MgLayerBase* value)
{
    MG_TRY()
    return m_layers->IndexOf(value);
    MG_CATCH_AND_THROW(L"MgReadOnlyLayerCollection.IndexOf");
    return -1;
}
