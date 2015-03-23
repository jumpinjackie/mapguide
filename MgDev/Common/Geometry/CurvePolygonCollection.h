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

#ifndef _MG_CURVEPOLYGONCOLLECTION_H_
#define _MG_CURVEPOLYGONCOLLECTION_H_

class MgCurvePolygonCollection;
template class MG_GEOMETRY_API Ptr<MgCurvePolygonCollection>;

/// \defgroup MgCurvePolygonCollection MgCurvePolygonCollection
/// \ingroup Collections_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgCurvePolygonCollection provides support for defining an ordered set of
/// curve polygons.
class MG_GEOMETRY_API MgCurvePolygonCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCurvePolygonCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of curve polygons in the collection.
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
    /// Returns number of curve polygons in the collection.
    ///
    virtual INT32 GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the curve polygon in the collection at the specified index. Throws
    /// an invalid argument exception if the index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurvePolygon GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurvePolygon GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurvePolygon GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the curve polygon to retrieve.
    ///
    /// \return
    /// Returns the curve polygon in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgCurvePolygon* GetItem(INT32 index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the curve polygon in the collection at the specified index to the
    /// specified value. Throws an invalid argument exception if the index is
    /// out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the coordinate to set.
    /// \param value (MgCurvePolygon)
    /// curve polygon to place into the collection.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgCurvePolygon* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified coordinate to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurvePolygon)
    /// curve polygon to add to the collection.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Add(MgCurvePolygon* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified curve polygon at the specified index within the
    /// collection. Items following the insertion point are moved down to
    /// accommodate the new item. Throws an invalid argument exception if the
    /// specified index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index at which to insert the coordinate.
    /// \param value (MgCurvePolygon)
    /// curve polygon to insert.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgCurvePolygon* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all curve polygons from the collection.
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
    /// Removes the specified curve polygon from the collection.  Returns false if
    /// the curve polygon could not be removed.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(const MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(const MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(const MgCurvePolygon value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurvePolygon)
    /// curve polygon reference to remove.
    ///
    /// \return
    /// Returns true if the curve polygon was removed
    ///
    virtual bool Remove(const MgCurvePolygon* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the curve polygon at the specified index from the collection.
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
    /// Index of the curve polygon to remove.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified curve polygon, false
    /// otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurvePolygon)
    /// curve polygon reference to check containment of.
    ///
    /// \return
    /// True if the collection contains the specified curve polygon, false
    /// otherwise.
    ///
    virtual bool Contains(const MgCurvePolygon* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified curve polygon in the collection or -1
    /// if the curve polygon does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(const MgCurvePolygon value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurvePolygon)
    /// curve polygon reference to find the index of.
    ///
    /// \return
    /// The index of the specified curve polygon in the collection or -1 if the
    /// curve polygon does not exist.
    ///
    virtual INT32 IndexOf(const MgCurvePolygon* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgCurvePolygonCollection object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCurvePolygonCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCurvePolygonCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCurvePolygonCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgCurvePolygonCollection();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// MgCurvePolygonCollection destructor
    ///
    virtual ~MgCurvePolygonCollection();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this collection
    ///
    MgCurvePolygonCollection* Copy();

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
    void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly);

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_CurvePolygonCollection;

private:

    Ptr<MgDisposableCollection> m_collection;
};
/// \}

#endif // _MG_CURVEPOLYGONCOLLECTION_H_
