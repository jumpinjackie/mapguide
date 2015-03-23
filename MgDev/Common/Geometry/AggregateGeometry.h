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

#ifndef _MGAGGREGATEGEOMETRY_H_
#define _MGAGGREGATEGEOMETRY_H_

class MgAggregateGeometry;
template class MG_GEOMETRY_API Ptr<MgAggregateGeometry>;

/// \defgroup MgAggregateGeometry MgAggregateGeometry
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgAggregateGeometry is an abstract base class for all of the
/// geometry types that are composed of a set of aggregated
/// geometry objects.
///
/// \remarks
/// The concrete class derived from
/// MgAggregateGeometry are:
/// <ul>
///   <li>MgMultiCurvePolygon</li>
///   <li>MgMultiCurveString</li>
///   <li>MgMultiGeometry</li>
///   <li>MgMultiLineString</li>
///   <li>MgMultiPoint</li>
///   <li>MgMultiPolygon</li>
/// </ul>
///

class MG_GEOMETRY_API MgAggregateGeometry : public MgGeometry
{
PUBLISHED_API:
    ////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of geometries in this aggregate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The number of geometries.
    ///
    virtual INT32 GetCount() = 0;  /// __get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry at the specified position
    ///
    virtual MgGeometry* GetGeometryAt(INT32 index) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a geometry collection, copy of the collection held by this aggregate geometry
    ///
    MgGeometryCollection* CopyCollection();

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
    static const INT32 m_cls_id = Geometry_AggregateGeometry;
};
/// \}

#endif //_MGAGGREGATEGEOMETRY_H_
