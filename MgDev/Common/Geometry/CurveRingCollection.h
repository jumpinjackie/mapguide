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

#ifndef _MG_CURVERINGCOLLECTION_H_
#define _MG_CURVERINGCOLLECTION_H_

class MgCurveRingCollection;
template class MG_GEOMETRY_API Ptr<MgCurveRingCollection>;

/// \defgroup MgCurveRingCollection MgCurveRingCollection
/// \ingroup Collections_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgCurveRingCollection provides support for defining an ordered set of
/// curve rings.
class MG_GEOMETRY_API MgCurveRingCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCurveRingCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of curve rings in the collection.
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
    /// Returns number of curve rings in the collection.
    ///
    virtual INT32 GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the curve ring in the collection at the specified index. Throws
    /// an invalid argument exception if the index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveRing GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveRing GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveRing GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the curve ring to retrieve.
    ///
    /// \return
    /// Returns the curve ring in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgCurveRing* GetItem(INT32 index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the curve ring in the collection at the specified index to the
    /// specified value. Throws an invalid argument exception if the index is
    /// out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the coordinate to set.
    /// \param value (MgCurveRing)
    /// curve ring to place into the collection.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgCurveRing* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified coordinate to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Add(MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Add(MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Add(MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveRing)
    /// curve ring to add to the collection.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Add(MgCurveRing* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified curve ring at the specified index within the
    /// collection. Items following the insertion point are moved down to
    /// accommodate the new item. Throws an invalid argument exception if the
    /// specified index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index at which to insert the coordinate.
    /// \param value (MgCurveRing)
    /// curve ring to insert.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgCurveRing* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all curve rings from the collection.
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
    /// Removes the specified curve ring from the collection.  Returns true if
    /// the curve ring was successfully removed.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(const MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(const MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(const MgCurveRing value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveRing)
    /// curve ring reference to remove.
    ///
    /// \return
    /// Returns true if curve ring was removed.
    ///
    virtual bool Remove(const MgCurveRing* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the curve ring at the specified index from the collection.
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
    /// Index of the curve ring to remove.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified curve ring, false
    /// otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveRing)
    /// curve ring reference to check containment of.
    ///
    /// \return
    /// True if the collection contains the specified curve ring, false
    /// otherwise.
    ///
    virtual bool Contains(const MgCurveRing* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified curve ring in the collection or -1
    /// if the curve ring does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(const MgCurveRing value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveRing)
    /// curve ring reference to find the index of.
    ///
    /// \return
    /// The index of the specified curve ring in the collection or -1 if the
    /// curve ring does not exist.
    ///
    virtual INT32 IndexOf(const MgCurveRing* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgCurveRingCollection object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCurveRingCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCurveRingCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCurveRingCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgCurveRingCollection();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// MgCurveRingCollection destructor
    ///
    virtual ~MgCurveRingCollection();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this collection
    ///
    MgCurveRingCollection* Copy();

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
    static const INT32 m_cls_id = Geometry_CurveRingCollection;

private:

    Ptr<MgDisposableCollection> m_collection;
};
/// \}

#endif // _MG_CURVERINGCOLLECTION_H_
