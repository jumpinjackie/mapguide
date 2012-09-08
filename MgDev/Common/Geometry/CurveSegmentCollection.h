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

#ifndef _MG_CURVESEGMENTCOLLECTION_H_
#define _MG_CURVESEGMENTCOLLECTION_H_

class MgCurveSegmentCollection;
template class MG_GEOMETRY_API Ptr<MgCurveSegmentCollection>;

/// \defgroup MgCurveSegmentCollection MgCurveSegmentCollection
/// \ingroup Collections_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgCurveSegmentCollection provides support for defining an ordered set of
/// curve segments.
class MG_GEOMETRY_API MgCurveSegmentCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCurveSegmentCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of curve segments in the collection.
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
    /// Returns number of curve segments in the collection.
    ///
    virtual INT32 GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the curve segment in the collection at the specified index. Throws
    /// an invalid argument exception if the index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveSegment GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveSegment GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveSegment GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the curve segment to retrieve.
    ///
    /// \return
    /// Returns the curve segment in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgCurveSegment* GetItem(INT32 index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the curve segment in the collection at the specified index to the
    /// specified value. Throws an invalid argument exception if the index is
    /// out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the coordinate to set.
    /// \param value (MgCurveSegment)
    /// Curve segment to place into the collection.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgCurveSegment* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified coordinate to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveSegment)
    /// Curve segment to add to the collection.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Add(MgCurveSegment* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified curve segment at the specified index within the
    /// collection. Items following the insertion point are moved down to
    /// accommodate the new item. Throws an invalid argument exception if the
    /// specified index is out of range.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index at which to insert the coordinate.
    /// \param value (MgCurveSegment)
    /// Curve segment to insert.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgCurveSegment* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all curve segments from the collection.
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
    /// Removes the specified curve segment from the collection. Returns true
    /// if the curve segment was successfully removed.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(const MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(const MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(const MgCurveSegment value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveSegment)
    /// Curve segment reference to remove.
    ///
    /// \return
    /// Returns true if curve segment was successfully removed.
    ///
    virtual bool Remove(const MgCurveSegment* value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the curve segment at the specified index from the collection.
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
    /// Index of the curve segment to remove.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified curve segment, false
    /// otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveSegment)
    /// Curve segment reference to check containment of.
    ///
    /// \return
    /// True if the collection contains the specified curve segment, false
    /// otherwise.
    ///
    virtual bool Contains(const MgCurveSegment* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified curve segment in the collection or -1
    /// if the curve segment does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(const MgCurveSegment value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgCurveSegment)
    /// Curve segment reference to find the index of.
    ///
    /// \return
    /// The index of the specified curve segment in the collection or -1 if the
    /// curve segment does not exist.
    ///
    virtual INT32 IndexOf(const MgCurveSegment* value) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgCurveSegmentCollection object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCurveSegmentCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCurveSegmentCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCurveSegmentCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgCurveSegmentCollection();

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// MgCurveSegmentCollection destructor
    ///
    virtual ~MgCurveSegmentCollection();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a copy of this collection
    ///
    MgCurveSegmentCollection* Copy();

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
    static const INT32 m_cls_id = Geometry_CurveSegmentCollection;

private:

    Ptr<MgDisposableCollection> m_collection;
};
/// \}

#endif // _MG_CURVESEGMENTCOLLECTION_H_
