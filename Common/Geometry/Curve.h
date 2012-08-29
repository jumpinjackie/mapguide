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

#ifndef _MGCURVE_H_
#define _MGCURVE_H_

class MgCurve;
template class MG_GEOMETRY_API Ptr<MgCurve>;

/// \defgroup MgCurve MgCurve
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////
/// \brief
/// MgCurve is an abstract base class. MgCurveString and
/// MgLineString are concrete classes derived from MgCurve.
///
class MG_GEOMETRY_API MgCurve : public MgGeometry
{
PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the starting coordinate of the curve.
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
    /// Gets the ending coordinate of the curve.
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
    virtual INT32 GetClassId();

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
    static const INT32 m_cls_id = Geometry_Curve;
};
/// \}

#endif //_MGCURVE_H_
