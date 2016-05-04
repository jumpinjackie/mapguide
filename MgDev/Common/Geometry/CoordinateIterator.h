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

#ifndef _MGCOORDINATEITERATOR_H_
#define _MGCOORDINATEITERATOR_H_

class MgCoordinateIterator;
template class MG_GEOMETRY_API Ptr<MgCoordinateIterator>;

/// \defgroup MgCoordinateIterator MgCoordinateIterator
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// The class MgCoordinateIterator supports iteration over the
/// coordinates of a geometry or path instruction.
///
/// \remarks
/// MgCoordinateIterator only
/// supports read access to the underlying coordinates. Initially, the
/// iterator is positioned before the first coordinate. Reset also brings
/// the iterator back to this position. At this position, calling
/// GetCurrent throws an exception. Therefore, you must call MoveNext to
/// advance the iterator to the first coordinate before calling GetCurrent.
///
class MG_GEOMETRY_API MgCoordinateIterator : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateIterator)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current coordinate. After an iterator is created or after a
    /// Reset, MoveNext must be called to advance the iterator to the first
    /// coordinate before calling GetCurrent; otherwise, the current position
    /// is undefined and an exception is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate GetCurrent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate GetCurrent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate GetCurrent();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The coordinate at the current position.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgCoordinate* GetCurrent();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the iterator to the next coordinate. After an iterator is
    /// created or after a call to Reset, an iterator is positioned before the
    /// first coordinate, and the first call to MoveNext moves the iterator
    /// over the first coordinate. After the last coordinate is passed,
    /// subsequent calls to MoveNext return false until Reset is called.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool MoveNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean MoveNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool MoveNext();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// True if the iterator was successfully advanced to the next coordinate;
    /// false if the iterator has passed the last coordinate.
    ///
    virtual bool MoveNext();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the iterator to its initial position prior to the first
    /// coordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Reset();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Reset();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Reset();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual void Reset();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an object of type MgCoordinateIterator
    ///
    MgCoordinateIterator(MgCoordinateCollection* collection);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// MgCoordinateIterator destructor
    ///
    virtual ~MgCoordinateIterator();

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
    /// Get the number of elements
    ///
    virtual INT32 GetCount();

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty object of type MgCoordinateIterator (for Ptr<>)
    ///
    MgCoordinateIterator();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_CoordinateIterator;

private:
    INT32 m_currPos;
    Ptr<MgCoordinateCollection> m_coordinateCollection;
};
/// \}

#endif //_MGCOORDINATEITERATOR_H_
