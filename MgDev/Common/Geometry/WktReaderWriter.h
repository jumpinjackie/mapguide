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

#ifndef _MGWKTREADERWRITER_H_
#define _MGWKTREADERWRITER_H_

class MgWktReaderWriter;
template class MG_GEOMETRY_API Ptr<MgWktReaderWriter>;

/// \defgroup MgWktReaderWriter MgWktReaderWriter
/// \ingroup Geometry_Module_classes
/// \{

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Translates a string in AGF (Autodesk Geometry Format) Text
/// format into an MgGeometry object and vice versa. See \link AgfText the AGF Text topic. \endlink
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// $wktReaderWriter = new MgWktReaderWriter();
/// $agfReaderWriter = new MgAgfReaderWriter();
/// // In the string to geometry direction:
/// $agfTextPoint = "POINT XY (0 0)";
/// $pointGeometryFromWkt = $wktReaderWriter->Read($agfTextPoint);
/// $byteReader = $agfReaderWriter->Write($pointGeometryFromAgfText);
/// // In the geometry to string direction:
/// $pointGeometryFromBytes = $agfReaderWriter->Read($byteReader);
/// $agfTextPoint = $wktReaderWriter->Write($pointGeometryFromBytes);
/// // prints "POINT XY (0 0)" to screen
/// echo "$agfTextPoint\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// \code
/// private MgAgfReaderWriter agfReaderWriter;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgPoint pt11FromText;
/// private MgPoint pt11;
/// private String pt11TextSpec = "POINT XY ( 1 1 )";
/// private MgByteReader byteReader;
/// private String geometryAgfText;
///
/// agfReaderWriter = new MgAgfReaderWriter();
/// wktReaderWriter = new MgWktReaderWriter();
/// // In the string to geometry direction:
/// pt11FromText = wktReaderWriter.Read(pt11TextSpec) as MgPoint;
/// byteReader = agfReaderWriter.Write(pt11FromText);
/// // In the geometry to string direction:
/// pt11 = agfReaderWriter.Read(byteReader);
/// geometryAgfText = wktReaderWriter.Write(pt11);
/// // geometryAgfText now contains the text representation of the geometry
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgWktReaderWriter : public MgGuardDisposable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgWktReaderWriter)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgWktReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgWktReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgWktReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgWktReaderWriter();

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a geometry object from an AGF Text format.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Read(string wkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Read(String wkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Read(string wkt);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param wkt (String/string)
    /// A string that supplies the AGF Text
    /// representation.
    ///
    /// \return
    /// An instance of MgGeometry that corresponds to the specified
    /// AGF Text.
    ///
    virtual MgGeometry* Read(CREFSTRING wkt);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a geometry object from a string in WKT format applying
    /// the specified transform during the read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Read(string wkt, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Read(String wkt, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Read(string wkt, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param wkt (String/string)
    /// A string that supplies the WKT representation.
    /// \param transform (MgTransform)
    /// An MgTransform that is used to transform each coordinate read from the
    /// WKT representation.
    ///
    /// \return
    /// An instance of MgGeometry that corresponds to the specified WKT with
    /// the specified transform applied.
    ///
    virtual MgGeometry* Read(CREFSTRING wkt, MgTransform* transform);

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a geometry object to a string in AGF Text format.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometry (MgGeometry)
    /// An MgGeometry object to write to AGF Text.
    ///
    /// \return
    /// A string that contains the AGF Text representation.
    ///
    virtual STRING Write(MgGeometry* geometry);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a geometry object to a string in WKT format applying
    /// the specified transform during the write.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometry (MgGeometry)
    /// An MgGeometry object to write to WKT.
    /// \param transform (MgTransform)
    /// An MgTransform that is used to transform each coordinate written to the
    /// WKT representation.
    ///
    /// \return
    /// A string that contains the transformed WKT representation.
    ///
    virtual STRING Write(MgGeometry* geometry, MgTransform* transform);

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
    static const INT32 m_cls_id = Geometry_WktReaderWriter;
};
/// \}

#endif //_MGWKTREADERWRITER_H_
