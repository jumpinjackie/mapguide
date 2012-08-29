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

#ifndef _MG_LINESTRINGCOLLECTION_H_
#define _MG_LINESTRINGCOLLECTION_H_

class MgLineStringCollection;
template class MG_GEOMETRY_API Ptr<MgLineStringCollection>;

/// \defgroup MgLineStringCollection MgLineStringCollection
/// \ingroup Collections_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgLineStringCollection provides support for defining an ordered set of
/// line strings.
class MG_GEOMETRY_API MgLineStringCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgLineStringCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of line strings in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns number of line strings in the collection.
    ///
    virtual INT32 GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the line string in the collection at the specified index. Throws
    /// an invalid argument exception if the index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLineString GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLineString GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLineString GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the line string to retrieve.
    ///
    /// \return
    /// Returns the line string in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgLineString* GetItem(INT32 index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the line string in the collection at the specified index to the
    /// specified value. Throws an invalid argument exception if the index is
    /// out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the coordinate to set.
    /// \param value (MgLineString)
    /// line string to place into the collection.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgLineString* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified coordinate to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int Add(MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int Add(MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int Add(MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLineString)
    /// line string to add to the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Add(MgLineString* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified line string at the specified index within the
    /// collection. Items following the insertion point are moved down to
    /// accommodate the new item. Throws an invalid argument exception if the
    /// specified index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index at which to insert the coordinate.
    /// \param value (MgLineString)
    /// line string to insert.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgLineString* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all line strings from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual void Clear();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified line string from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Remove(const MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(const MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(const MgLineString value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLineString)
    /// line string reference to remove.
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    virtual bool Remove(const MgLineString* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the line string at the specified index from the collection.
    /// Throws an invalid argument exception if the index does not exist within
    /// the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the line string to remove.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified line string, false
    /// otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLineString)
    /// line string reference to check containment of.
    ///
    /// \return
    /// True if the collection contains the specified line string, false
    /// otherwise.
    ///
    virtual bool Contains(const MgLineString* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified line string in the collection or -1
    /// if the line string does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(const MgLineString value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLineString)
    /// line string reference to find the index of.
    ///
    /// \return
    /// The index of the specified line string in the collection or -1 if the
    /// line string does not exist.
    ///
    virtual INT32 IndexOf(const MgLineString* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgLineStringCollection object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLineStringCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLineStringCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLineStringCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgLineStringCollection();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// MgLineStringCollection destructor
    ///
    virtual ~MgLineStringCollection();


INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this collection
    ///
    MgLineStringCollection* Copy();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly);

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_LineStringCollection;

private:
    Ptr<MgDisposableCollection> m_collection;
};
/// \}

#endif // _MG_LINESTRINGCOLLECTION_H_
