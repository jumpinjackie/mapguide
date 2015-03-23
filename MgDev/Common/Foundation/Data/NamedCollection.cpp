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
#include <map>
#include <algorithm>


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgNamedCollection.  Collection is initially empty.
/// </summary>
/// <param name="caseSensitive">Indicates whether names are case sensitive</param>
MgNamedCollection::MgNamedCollection(bool caseSensitive)
{
    m_dCollection = new MgDisposableCollection();

    m_bAllowDuplicate = false;
    m_bCheckDuplicate = true;
    m_bCaseSensitive = caseSensitive;
    m_pNameMap = NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgNamedCollection.  Collection is initially empty.
/// </summary>
/// <param name="allowDuplicate">Indicates whether duplicate names are allowed</param>
/// <param name="caseSensitive">Indicates whether names are case sensitive</param>
MgNamedCollection::MgNamedCollection(bool allowDuplicate, bool caseSensitive)
{
    m_dCollection = new MgDisposableCollection();

    m_bAllowDuplicate = allowDuplicate;
    m_bCheckDuplicate = true;
    m_bCaseSensitive = caseSensitive;
    m_pNameMap = NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the releases the contained MgNamedSerializable objects.
/// </summary>
MgNamedCollection::~MgNamedCollection()
{
    delete m_pNameMap;
    m_pNameMap = NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgNamedCollection::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns number of items in the collection.
/// </summary>
INT32 MgNamedCollection::GetCount() const
{
    return m_dCollection->GetCount();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
MgNamedSerializable* MgNamedCollection::GetItem(INT32 index) const
{
    MgDisposable* item = m_dCollection->GetItem(index);
    return (MgNamedSerializable*)item;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection with the specified name. Throws an exception if the item is not found.
/// </summary>
/// <param name="name">Input item name</param>
/// <returns>
/// Returns the item in the collection with the specified name.
/// </returns>
MgNamedSerializable* MgNamedCollection::GetItem(CREFSTRING name)
{
    MgNamedSerializable* item = FindItem(name);
    if (!item)
    {
        MgStringCollection args;
        args.Add(name);
        throw new MgObjectNotFoundException(L"GetItem", __LINE__, __WFILE__, NULL, L"MgNoNameForObject", &args); // EXC::Create(Exception::NLSGetMessage(NLSID(38_ITEMNOTFOUND),name));
    }

    return item;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Finds the item in the collection with the specified name.
/// </summary>
/// <param name="name">Input item name</param>
/// <returns>
/// Returns the item in the collection with the specified name.
/// Returns NULL if the item was not found.
/// </returns>
MgNamedSerializable* MgNamedCollection::FindItem(CREFSTRING name)
{
    // trigger the building of a map when the collection reaches the threshold size.
    InitMap();

    Ptr<MgNamedSerializable> obj;

    if (m_pNameMap)
    {
        // Accessing the map is faster for large collections, so use it if built.
        obj = GetMap(name);

        // If the object name can't be modified then we're done searching.
        // Otherwise, there's a chance the object name was modified,
        // meaning that it can be in the collection but not the map,
        // or in the wrong place in the map.
        if (obj != NULL)
        {
            if (obj->CanSetName())
            {
                // The name can be set.

                // If the found object's name is the same as the given name
                // then we're done with searching. Otherwise, this object's
                // name has changed and a linear search is needed to find
                // the requested object.
                if (Compare(obj->GetName(), name) != 0)
                {
                    // The names do not match so we need to do a linear search
                    // to find it because the collection and the map don't match.
                    obj = NULL;
                }
            }
        }
    }

    // No map so do linear search or linear search is required because the list and the map don't match.
    if (obj == NULL)
    {
        INT32 size = GetCount();
        for (INT32 i = 0; i < size; i++)
        {
            Ptr<MgNamedSerializable> temp = GetItem(i);
            if (temp != NULL)
            {
                if (Compare(name, temp->GetName()) == 0)
                {
                    // Found it
                    obj = temp;
                    break;
                }
            }
        }
    }

    return obj.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index to the specified value. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::SetItem(INT32 index, MgNamedSerializable* value)
{
    CheckDuplicate(value, index);

    // Remove the old item from the map
    if (m_pNameMap)
        RemoveMapAt(index);

    // Add the new item to the map
    if (m_pNameMap && value)
        InsertMap(value);

    // Set the new item in the list
    m_dCollection->SetItem(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing
/// </returns>
void MgNamedCollection::Add(MgNamedSerializable* value)
{
    CheckDuplicate(value, -1);

    // Insert the new item in the map
    if (m_pNameMap && value)
        InsertMap(value);

    // Add it to the list
    m_dCollection->Add(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Inserts the specified item at the specified index within the collection.
/// Items following the insertion point are moved down to accommodate the new item.
/// Throws an invalid argument exception if the specified index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::Insert(INT32 index, MgNamedSerializable* value)
{
    CheckDuplicate(value, -1);

    // Insert the new item in the map
    if (m_pNameMap)
        InsertMap(value);

    // Insert it in the list
    return m_dCollection->Insert(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::Clear()
{
    // Clear the map
    delete m_pNameMap;
    m_pNameMap = NULL;

    // Clear the list
    m_dCollection->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful.</returns>
bool MgNamedCollection::Remove(const MgNamedSerializable* value)
{
    bool removed = true;
    try
    {
        // Remove the item from the map
        if (m_pNameMap)
            RemoveMap(value);

        // Remove it from the list
        removed = m_dCollection->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection. Throws an invalid argument exception if the item does not exist within the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::RemoveAt(INT32 index)
{
    // Remove the item from the map
    if (m_pNameMap)
        RemoveMapAt(index);

    // Remove it from the list
    m_dCollection->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgNamedCollection::Contains(const MgNamedSerializable* value)
{
    // trigger the building of a map when the collection reaches the threshold size
    InitMap();

    if (m_pNameMap)
    {
        // If map is built, use it since it is faster.
        Ptr<MgNamedSerializable> item = GetMap( ((MgNamedSerializable*)value)->GetName() );
        return(item  != NULL );
    }

    // otherwise, linear search
    return m_dCollection->Contains(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgNamedCollection::Contains(CREFSTRING name)
{
    Ptr<MgNamedSerializable> item = FindItem(name);
    return (item != NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgNamedCollection::IndexOf(const MgNamedSerializable* value) const
{
    return m_dCollection->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item (by name) in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgNamedCollection::IndexOf(CREFSTRING name) const
{
    if (name.length() == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgNamedCollection.IndexOf",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL); // EXC::Create(Exception::NLSGetMessage(NLSID(2_BADPARAMETER)));
    }

    INT32 size = GetCount();
    for (INT32 i = 0; i < size; i++)
    {
        Ptr<MgNamedSerializable> pitem = MgNamedCollection::GetItem(i);
        if (pitem != NULL)
        {
            STRING str = pitem->GetName();
            if (!str.empty() && Compare(name, str) == 0)
            {
                return i;
            }
        }
    }

    return -1;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgNamedCollection::ToXml()
{
    throw new MgNotImplementedException(L"MgNamedCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::Serialize(MgStream* stream)
{
    INT32 size = GetCount();
    stream->WriteInt32(size);

    for (INT32 i = 0; i < size; ++i)
    {
        Ptr<MgNamedSerializable> obj = GetItem(i);
        stream->WriteObject(obj);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgNamedCollection::Deserialize(MgStream* stream)
{
    INT32 size = 0;
    stream->GetInt32(size);

    for (INT32 i = 0; i < size; ++i)
    {
        Ptr<MgNamedSerializable> obj = (MgNamedSerializable*)stream->GetObject();
        Add(obj);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgNamedCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
int MgNamedCollection::Compare(CREFSTRING str1, CREFSTRING str2) const
{
    if (m_bCaseSensitive)
        return wcscmp(str1.c_str(), str2.c_str());

    // Try case-insensitive comparison.
    return _wcsicmp(str1.c_str(), str2.c_str());
}


//////////////////////////////////////////////////////////////////
void MgNamedCollection::CheckDuplicate(MgNamedSerializable* item, INT32 index)
{
    if (!m_bAllowDuplicate && m_bCheckDuplicate)
    {
        Ptr<MgNamedSerializable> foundItem1 = FindItem(item->GetName());
        Ptr<MgNamedSerializable> foundItem2;

        if (index >= 0)
            foundItem2 = GetItem(index);

        if ((foundItem1 != NULL) && (foundItem1.p != foundItem2.p))
        {
            MgStringCollection args;
            args.Add(item->GetName());
            throw new MgDuplicateObjectException(L"CheckDuplicate", __LINE__, __WFILE__, NULL, L"MgDuplicateObject", &args); // EXC::Create(Exception::NLSGetMessage(NLSID(45_ITEMINCOLLECTION),(String*) item->GetName()));
        }
    }
}


//////////////////////////////////////////////////////////////////
void MgNamedCollection::InitMap()
{
    // Build the map if not already built and the collection has hit the threshold
    if (!m_pNameMap && (GetCount() > COLL_MAP_THRESHOLD))
    {
        m_pNameMap = new std::map<STRING, MgNamedSerializable* >();

        // Put all the current elements into the map
        INT32 size = GetCount();
        for (INT32 i = size-1; i >= 0; i--)
        {
            Ptr<MgNamedSerializable> item = GetItem(i);
            InsertMap(item);
        }
    }
}


//////////////////////////////////////////////////////////////////
void MgNamedCollection::InsertMap( MgNamedSerializable* value )
{
    // Add an element to the map. Elements are keyed by name, which may or may not be case sensitive.
    // Case insensitive names are stored in lower case.
    if (m_bCaseSensitive)
        m_pNameMap->insert( std::pair<STRING, MgNamedSerializable* >(value->GetName(), value) );
    else
        m_pNameMap->insert( std::pair<STRING, MgNamedSerializable* >(Lower(value->GetName()), value) );
}


//////////////////////////////////////////////////////////////////
// Remove the element at the specified index, from the map
void MgNamedCollection::RemoveMapAt(INT32 index)
{
    // Remove the old item from the map
    Ptr<MgNamedSerializable> pItem = GetItem(index);

    if (pItem)
        RemoveMap(pItem);
}


//////////////////////////////////////////////////////////////////
// Remove the given element from the map.
void MgNamedCollection::RemoveMap(const MgNamedSerializable* value)
{
    // handle the current case sensitivity of the element name.
    if (m_bCaseSensitive)
        m_pNameMap->erase( ((MgNamedSerializable*)value)->GetName() );
    else
        m_pNameMap->erase( Lower(((MgNamedSerializable*)value)->GetName()) );
}


//////////////////////////////////////////////////////////////////
// Get the named element from the map. Returns NULL if element not in map.
MgNamedSerializable* MgNamedCollection::GetMap(CREFSTRING name) const
{
    MgNamedSerializable* pItem = NULL;

    std::map<STRING, MgNamedSerializable* >::const_iterator iter;

    if (m_bCaseSensitive)
        iter = m_pNameMap->find( name );
    else
        // Case sensitive names are stored in lower case.
        iter = m_pNameMap->find(Lower(name));

    if (iter != m_pNameMap->end())
    {
        pItem = iter->second;
        SAFE_ADDREF(pItem);
    }

    return pItem;
}


//////////////////////////////////////////////////////////////////
STRING MgNamedCollection::Lower(STRING str) const
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}


//////////////////////////////////////////////////////////////////
bool MgNamedCollection::GetAllowDuplicate()
{
    return m_bAllowDuplicate;
}


//////////////////////////////////////////////////////////////////
bool MgNamedCollection::GetCaseSenstive()
{
    return m_bCaseSensitive;
}

//////////////////////////////////////////////////////////////////
void MgNamedCollection::SetCheckForDuplicates(bool bDup)
{
    m_bCheckDuplicate = bDup;
}
