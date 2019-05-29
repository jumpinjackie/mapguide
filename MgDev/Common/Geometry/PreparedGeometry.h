//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef _MGPREPAREDGEOMETRY_H_
#define _MGPREPAREDGEOMETRY_H_

#include <memory>

class MgGeometry;

////////////////////////////////////////////////////////////////
/// \brief
/// MgPreparedGeometry is an prepared form of MgGeometry optimized for the case of 
/// repeated evaluation of spatial predicates against it and any other geometry
///
class MG_GEOMETRY_API MgPreparedGeometry : public MgGuardDisposable
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// This is a convenience method. Given 2 geometries a and b,
    /// a.Contains(b) is true if and only if b.MgGeometry::Within(a)
    /// is true.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if the other geometry is within this geometry, false
    /// otherwise.
    ///
    virtual bool Contains(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Crosses(b) is true if and only
    /// if the dimension of the intersection of the interior of a and
    /// the interior of b is less than the greater of the dimension
    /// of the interior of a and the dimension of the interior of b
    /// and the intersection of a and b is neither a nor b.
    ///
    /// \remarks
    /// A Point cannot cross another geometry because the
    /// intersection of the Point with the other geometry is the
    /// Point.
    /// \n
    /// Two MultiPoint geometries cannot cross one another because
    /// the dimension of the intersection of their interiors, namely
    /// the 0-dimension, is not less than the greater of the
    /// dimensions of their interiors, namely the 0-dimension.
    /// \n
    /// [\link OGC99049 OGC99-049 \endlink] implicitly excludes a Crosses
    /// relationship between 2 polygons. According to the definition,
    /// the possibility of such a relationship would require that the
    /// intersection of the interior of one polygon with that of
    /// another be a Point or Line.
    /// <h2>Illustration</h2>
    /// d, e and f are MultiPoint geometries.
    /// \n
    /// \image html crosses.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry spatially crosses the other geometry,
    /// false otherwise.
    ///
    virtual bool Crosses(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Disjoint(b)is true if and only
    /// if the intersection of a and b is empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry is spatially disjoint from the other
    /// geometry, false otherwise.
    ///
    virtual bool Disjoint(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// This is a convenience method. Given 2 geometries a and b,
    /// a.Intersects(b) is true if and only if a.\link MgGeometry::Disjoint Disjoint \endlink(b)
    /// is false.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry is not disjoint with respect to the
    /// other geometry, false otherwise.
    ///
    virtual bool Intersects(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Overlaps(b) is true if and only
    /// if the dimension of the interior of a equals the dimension of
    /// the interior of b equals the dimension of the intersection of
    /// the interior of a and the interior of b and the intersection
    /// of a and b is neither a nor b.
    ///
    /// \remarks
    /// A Point cannot overlap a Point, and a MultiPoint cannot
    /// overlap a Point but a MultiPoint can overlap a MultiPoint.
    /// <h2>Illustration</h2>
    /// c and d are MultiPoint geometries.
    /// \image html overlaps.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry spatially overlaps the other geometry,
    /// false otherwise.
    ///
    virtual bool Overlaps(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Touches(b) is true if and only
    /// if the intersection of the interior of a and the interior of
    /// b is empty and the intersection of a and b is not empty.
    ///
    /// \remarks
    /// A Point cannot touch a Point because a Point has no boundary
    /// and so the intersection of the interiors of the two
    /// geometries is not empty.
    /// \n
    /// A Point can touch a non-closed Line at one of the end points
    /// of the Line.
    /// \n
    /// A Point cannot touch a closed Line because all of the points
    /// in the Line are interior to it.
    /// <h2>Illustration</h2>
    /// e are MultiPoint geometries and f is a LineString.
    /// \image html touches.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry spatially touches the other geometry,
    /// false otherwise.
    ///
    virtual bool Touches(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Within(b) is true if and only
    /// if the intersection of a and b is a and the intersection of
    /// the interior of a and the interior of b is not empty.
    ///
    /// \remarks
    /// If the entire point-set of a geometry intersects the boundary
    /// of another geometry, the former is not within the latter.
    /// <h2>Illustration</h2>
    /// The end point of d and the end point of e intersect. a, b, i,
    /// j, k, and m are MultiPoints. The concentric circles represent
    /// intersecting points. The polygon n1n2n3n4 is within the
    /// polygon p1p2p3p4 and vice versa. The LineString q1q2 is
    /// within the LineString r1r2 and vice versa. The MultiPoint j
    /// is within the MultiPoint k and vice versa. The Point f is
    /// within the point g and vice versa.
    /// \n
    /// \image html within.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry is spatially within the other geometry,
    /// false otherwise.
    ///
    virtual bool Within(MgGeometry* other);

INTERNAL_API:
    static MgPreparedGeometry* Create(MgGeometry* geom);
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

private:
    class PreparedGeometryImpl;
    std::unique_ptr<PreparedGeometryImpl> d_ptr;
    MgPreparedGeometry(PreparedGeometryImpl* impl);

protected:
    virtual ~MgPreparedGeometry();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_PreparedGeometry;
};

#endif
