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

#ifndef _MGGEOMETRYCOMPONENT_H_
#define _MGGEOMETRYCOMPONENT_H_

class MgGeometryComponent;
template class MG_GEOMETRY_API Ptr<MgGeometryComponent>;

/// \defgroup MgGeometryComponent MgGeometryComponent
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometryComponent is the base class for all of the geometry
/// component types, namely, MgArcSegment, MgCurveRing,
/// MgLinearRing, and MgLinearSegment.
///
/// \remarks
/// Geometry components are
/// used to build MgGeometry objects. Geometry components are
/// constructed by calls to non-static methods on an
/// MgGeometryFactory object and are immutable once constructed.
///
class MG_GEOMETRY_API MgGeometryComponent : public MgGeometricEntity
{
PUBLISHED_API:
    /////////////////////////////////////////////
    /// \brief
    /// Gets the type of this geometry component.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetComponentType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetComponentType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetComponentType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns one of the values defined in
    /// MgGeometryComponentType.
    ///
    virtual INT32 GetComponentType() = 0;  /// __get

INTERNAL_API:

    virtual INT32 GetEntityType() { return MgGeometryEntityType::GeometryComponent; }

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
    static const INT32 m_cls_id = Geometry_GeometryComponent;
};
/// \}

#endif //_MGGEOMETRYCOMPONENT_H_
