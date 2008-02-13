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

#ifndef _MG_GEOMETRYCOMPONENTCOLLECTION_H_
#define _MG_GEOMETRYCOMPONENTCOLLECTION_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometryComponentCollection provides support for defining an ordered set
/// of geometry components.
class MgGeometryComponentCollection : public MgDisposable
{
    DECLARE_CLASSNAME(MgGeometryComponentCollection)

EXTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of objects in the collection.
    ///
    /// \return
    /// Returns number of objects in the collection.
    ///
    virtual INT32 GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the object in the collection at the specified index. Throws
    /// an invalid argument exception if the index is out of range.
    ///
    /// \param index
    /// Index of the object to retrieve.
    ///
    /// \return
    /// Returns the object in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgGeometryComponent* GetItem(INT32 index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the object in the collection at the specified index to the
    /// specified value. Throws an invalid argument exception if the index is
    /// out of range.
    ///
    /// \param index
    /// Index of the object to set.
    /// \param value
    /// Object to place into the collection.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgGeometryComponent* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified object to the end of the collection.
    ///
    /// \param value
    /// Object to add to the collection.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Add(MgGeometryComponent* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified object at the specified index within the
    /// collection. Items following the insertion point are moved down to
    /// accommodate the new item. Throws an invalid argument exception if the
    /// specified index is out of range.
    ///
    /// \param index
    /// Index at which to insert the object.
    /// \param value
    /// Object to insert.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgGeometryComponent* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all objects from the collection.
    ///
    virtual void Clear();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified object from the collection.
    ///
    /// \param value
    /// Object reference to remove.
    ///
    /// \return
    /// Returns true if successful.
    ///
    virtual bool Remove(const MgGeometryComponent* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the object at the specified index from the collection.
    /// Throws an invalid argument exception if the index does not exist within
    /// the collection.
    ///
    /// \param index
    /// Index of the object to remove.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified object, false
    /// otherwise.
    ///
    /// \param value
    /// Object reference to check containment of.
    ///
    /// \return
    /// True if the collection contains the specified object, false
    /// otherwise.
    ///
    virtual bool Contains(const MgGeometryComponent* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified object in the collection or -1
    /// if the object does not exist.
    ///
    /// \param value
    /// Object reference to find the index of.
    ///
    /// \return
    /// The index of the specified object in the collection or -1 if the
    /// coordinate does not exist.
    ///
    virtual INT32 IndexOf(const MgGeometryComponent* value) const;
};
/// \endcond

#endif
