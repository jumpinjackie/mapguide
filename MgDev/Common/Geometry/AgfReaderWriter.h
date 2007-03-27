//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MGAGFREADERWRITER_H_
#define _MGAGFREADERWRITER_H_

/// \defgroup MgAgfReaderWriter MgAgfReaderWriter
/// \ingroup Geometry_Module_classes
/// \{

class MgAgfReaderWriter;
template class MG_GEOMETRY_API Ptr<MgAgfReaderWriter>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// The MgAgfReaderWriter class is used to translate an
/// MgGeometry object into into an MgByteReader object and to do
/// the reverse translation.
///
/// The MgGeometry object to be translated is obtained from an
/// MgWktReaderWriter or MgGeometryFactory object. The resulting
/// MgByteReader object is then used to create an
/// MgGeometryProperty object for an \link MgInsertFeatures insert features \endlink
/// operation.
///
/// In the reverse direction an MgByteReader object from an
/// MgFeatureReader object is translated into an MgGeometry
/// object. The MgFeatureReader object is obtained from a \link MgFeatureService::SelectFeatures select features \endlink
/// operation, and the MgByteReader object is returned by a call
/// to the GetGeometry() method on the MgFeatureReader object.
///
/// The stream of bytes contained in this MgByteReader object is
/// in Autodesk Geometry Format (AGF).
/// PHP
/// $wktReaderWriter = new MgWktReaderWriter();
/// $agfReaderWriter = new MgAgfReaderWriter();
/// $geometryFactory = new MgGeometryFactory();
///
/// In the MgGeometry to MgGeometryProperty direction:
/// // get the geometry from MgWktReaderWriter
/// $agfTextPoint = "POINT XY (0 0)";
/// $pointGeometryFromWkt = $wktReaderWriter->Read($agfTextPoint);
/// // or get the geometry from MgGeometryFactory
/// // $coordinateXY = $geometryFactory->CreateCoordinateXY(0, 0);
/// // $pointGeometryFromFactory = $geometryFactory->CreatePoint($coordinateXY);
/// // translate the MgGeometry object into an MgByteReader object
/// $byteReader = $agfReaderWriter->Write($pointGeometryFromWkt);
/// // $byteReader = $agfReaderWriter->Write($pointGeometryFromFactory);
/// // create an MgGeometryProperty object from the byte reader
/// $geometryProperty = new MgGeometryProperty("geomPropName", $byteReader);
///
/// In the MgFeatureReader to MgGeometry direction:
/// $byteReader = $featureReader->GetGeometry("geomPropName");
/// $pointGeometry = $agfReaderWriter->Read($byteReader);
/// $agfTextPoint = $wktReaderWriter->Write($pointGeometry);
/// echo "$agfTextPointn"; // prints to screen "POINT XY (0 0)"

class MG_GEOMETRY_API MgAgfReaderWriter : public MgDisposable
{
    DECLARE_CLASSNAME(MgAgfReaderWriter)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgAgfReaderWriter object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgAgfReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgAgfReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgAgfReaderWriter();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgAgfReaderWriter();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Translates an MgByteReader object into an MgGeometry object.
    /// The contents of the MgByteReader is in Autodesk Geometry
    /// Format (AGF).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param agf (MgByteReader)
    /// An MgByteReader that supplies the AGF byte
    /// stream.
    ///
    /// \return
    /// An instance of MgGeometry that corresponds to the specified
    /// AGF.
    ///
    virtual MgGeometry* Read(MgByteReader* agf);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a geometry object from a stream of bytes in AGF format applying
    /// the specified transform during the read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Read(MgByteReader agf, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param agf (MgByteReader)
    /// An MgByteReader that supplies the AGF byte stream.
    /// \param transform (MgTransform)
    /// An MgTransform that is used to transform each coordinate read from the
    /// AGF representation.
    ///
    /// \return
    /// An instance of MgGeometry that corresponds to the specified AGF with
    /// the specified transform applied.
    ///
    virtual MgGeometry* Read(MgByteReader* agf, MgTransform* transform);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Translates an MgGeometry object into a stream of bytes in AGF
    /// (Autodesk Geometry Format).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometry (MgGeometry)
    /// An MgGeometry object to write to AGF.
    ///
    /// \return
    /// An MgByteReader which contains the AGF representation of the
    /// MgGeometry object.
    ///
    virtual MgByteReader* Write(MgGeometry* geometry);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a geometry object to a stream of bytes in AGF format applying
    /// the specified transform during the write.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader Write(MgGeometry geometry, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geometry (MgGeometry)
    /// An MgGeometry object to write to AGF.
    /// \param transform (MgTransform)
    /// An MgTransform that is used to transform each coordinate written to the
    /// AGF representation.
    ///
    /// \return
    /// An MgByteReader that contains the transformed AGF representation.
    ///
    virtual MgByteReader* Write(MgGeometry* geometry, MgTransform* transform);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    ///
    virtual INT32 GetClassId();

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_AgfReaderWriter;
};
/// \}

#endif //_MGAGFREADERWRITER_H_
