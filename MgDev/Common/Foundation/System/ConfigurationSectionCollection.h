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

#ifndef _MG_CONFIGURATION_SECTION_COLLECTION_H
#define _MG_CONFIGURATION_SECTION_COLLECTION_H

class MgConfigurationSection;

/// \cond INTERNAL
class MgConfigurationSectionCollection : public MgDisposable
{
    DECLARE_CLASSNAME(MgConfigurationSectionCollection)

INTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty collection of Configuration section objects.
    ///
    MgConfigurationSectionCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the collection and the object contained in it.
    ///
    ~MgConfigurationSectionCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    void Add(MgConfigurationSection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    void Insert(INT32 index, MgConfigurationSection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns true if successful.
    ///
    bool Remove(MgConfigurationSection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    void SetItem(INT32 index, MgConfigurationSection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    MgConfigurationSection* GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// \return
    /// Returns the number of items in the collection.
    ///
    INT32 GetCount() const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all items from the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Clear();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    bool Contains(const MgConfigurationSection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the configuration section object with the specified path or -1 if the item does not exist.
    ///
    /// \param value
    /// Section Path
    ///
    /// \return
    /// Returns the index of the configuration section object with the specified path or -1 if the item does not exist.
    ///
    INT32 IndexOf(CREFSTRING sectionPath) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    INT32 IndexOf(const MgConfigurationSection* value) const;

protected:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    virtual void Dispose();

private:
    Ptr<MgDisposableCollection> m_collection;
};
/// \endcond

#endif // _MG_CONFIGURATION_SECTION_COLLECTION_H
