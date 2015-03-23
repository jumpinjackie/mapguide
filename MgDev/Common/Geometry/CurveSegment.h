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

#ifndef _MGCURVESEGMENT_H_
#define _MGCURVESEGMENT_H_

class MgCurveSegment;
template class MG_GEOMETRY_API Ptr<MgCurveSegment>;

/// \defgroup MgCurveSegment MgCurveSegment
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// MgCurveSegment is an abstract base class.
///
/// \remarks
/// The concrete
/// classes are MgLinearSegment and MgArcSegment. Collections of
/// curve segments are used to define MgCurveSting geometries and
/// MgCurveRing geometry components. MgCurveRing objects are used
/// to define MgCurvePolygon objects.
///
class MG_GEOMETRY_API MgCurveSegment : public MgGeometryComponent
{
PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the starting coordinate of the curve segment.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate GetStartCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate GetStartCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate GetStartCoordinate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgCoordinate that defines the starting coordinate.
    ///
    virtual MgCoordinate* GetStartCoordinate() = 0;  /// __get

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets the ending coordinate of the curve segment.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinate GetEndCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinate GetEndCoordinate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinate GetEndCoordinate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgCoordinate that defines the ending coordinate.
    ///
    virtual MgCoordinate* GetEndCoordinate() = 0;  /// __get

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether this geometry component is spatially equal to another geometry component.
    ///
    virtual bool Equals(MgGeometryComponent* other) = 0;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates() = 0;

CLASS_ID:
    static const INT32 m_cls_id = Geometry_CurveSegment;
};
/// \}

#endif //_MGCURVESEGMENT_H_
