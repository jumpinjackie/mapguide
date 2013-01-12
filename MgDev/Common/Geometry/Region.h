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

#ifndef MGREGION_H_
#define MGREGION_H_

class MgRegion;
template class MG_GEOMETRY_API Ptr<MgRegion>;

/// \defgroup MgRegion MgRegion
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// MgRegion is an abstract base class The concrete classes are
/// MgPolygon and MgCurvePolygon.
///
/// \note
/// In OpenGIS Simple Features this is called a Surface.
///
class MG_GEOMETRY_API MgRegion : public MgGeometry
{
    DECLARE_CLASSNAME(MgRegion)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a point guaranteed to be within the region.
    ///
    /// \note
    /// Optional for MapGuide.
    ///
    /// \return
    /// An MgPoint that defines a point in the region.
    ///
    virtual MgPoint* GetPointInRegion();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCoordinateIterator GetCoordinates();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCoordinateIterator GetCoordinates();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCoordinateIterator GetCoordinates();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates() = 0;  /// __get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_Region;
};
/// \}

#endif //_MGREGION_H_
