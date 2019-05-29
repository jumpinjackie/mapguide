//
//  Copyright (C) 2004-2015 by Autodesk, Inc.
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

#ifndef _MGGEOMETRYSIMPLIFIER_H_
#define _MGGEOMETRYSIMPLIFIER_H_

/// \defgroup MgGeometrySimplifier MgGeometrySimplifier
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// The MgGeometrySimplifier class provides the ability to simplify MgGeometry instances.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// $wktRw = new MgWktReaderWriter();
/// $geom = $wktRw->Read("LINESTRING (0 5, 1 5, 2 5, 5 5)");
/// $simp = new MgGeometrySimplifier();
/// $simplified = $simp->Simplify($geom, 10.0, MgGeometrySimplificationAlgorithmType::DouglasPeucker);
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// \code
/// using OSGeo.MapGuide;
/// ...
/// MgWktReaderWriter wktRw = new MgWktReaderWriter();
/// MgGeometry geom = wktRw.Read("LINESTRING (0 5, 1 5, 2 5, 5 5)");
/// MgGeometrySimplifier simp = new MgGeometrySimplifier();
/// MgGeometry simplified = simp.Simplify(geom, 10.0, MgGeometrySimplificationAlgorithmType.DouglasPeucker);
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (Java) -->
/// \htmlinclude JavaExampleTop.html
/// \code
/// import org.osgeo.mapguide;
/// ...
/// MgWktReaderWriter wktRw = new MgWktReaderWriter();
/// MgGeometry geom = wktRw.Read("LINESTRING (0 5, 1 5, 2 5, 5 5)");
/// MgGeometrySimplifier simp = new MgGeometrySimplifier();
/// MgGeometry simplified = simp.Simplify(geom, 10.0, MgGeometrySimplificationAlgorithmType.DouglasPeucker);
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// \since 4.0
class MG_GEOMETRY_API MgGeometrySimplifier : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgGeometrySimplifier)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgGeometrySimplifier object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeometrySimplifier();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeometrySimplifier();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeometrySimplifier();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgGeometrySimplifier();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Simplifies the given geometry using the specified algorithm and tolerance
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Simplify(MgGeometry geom, double tolerance, int algorithm);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Simplify(MgGeometry geom, double tolerance, int algorithm);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Simplify(MgGeometry geom, double tolerance, int algorithm);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geom (MgGeometry)
    /// The geometry instance to be simplified
    ///
    /// \param tolerance (double)
    /// The tolerance factor to simplify by
    ///
    /// \param algorithm (int)
    /// The simplification algorithm to use. Use any value from MgGeometrySimplificationAlgorithmType
    ///
    /// \return
    /// A simplified MgGeometry instance or null if simpification results in an empty geometry
    ///
    MgGeometry* Simplify(MgGeometry* geom, double tolerance, INT32 algorithm);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_GeometrySimplifier;
};
/// \}

#endif //_MGGEOMETRYSIMPLIFIER_H_
