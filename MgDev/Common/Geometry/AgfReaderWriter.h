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

#ifndef _MGAGFREADERWRITER_H_
#define _MGAGFREADERWRITER_H_

class MgAgfReaderWriter;
template class MG_GEOMETRY_API Ptr<MgAgfReaderWriter>;

/// \defgroup MgAgfReaderWriter MgAgfReaderWriter
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// The MgAgfReaderWriter class is used to translate an
/// MgGeometry object into into an MgByteReader object and to do
/// the reverse translation.
///
/// \remarks
/// The MgGeometry object to be translated is obtained from an
/// MgWktReaderWriter or MgGeometryFactory object. The resulting
/// MgByteReader object is then used to create an
/// MgGeometryProperty object for an \link MgInsertFeatures insert features \endlink
/// operation.
/// \n
/// In the reverse direction an MgByteReader object from an
/// MgFeatureReader object is translated into an MgGeometry
/// object. The MgFeatureReader object is obtained from a \link MgFeatureService::SelectFeatures select features \endlink
/// operation, and the MgByteReader object is returned by a call
/// to the GetGeometry() method on the MgFeatureReader object.
/// \n
/// The stream of bytes contained in this MgByteReader object is
/// in Autodesk Geometry Format (AGF).
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// $wktReaderWriter = new MgWktReaderWriter();
/// $agfReaderWriter = new MgAgfReaderWriter();
/// $geometryFactory = new MgGeometryFactory();
///
/// # In the MgGeometry to MgGeometryProperty direction:
/// # get the geometry from MgWktReaderWriter
/// $agfTextPoint = "POINT XY (0 0)";
/// $pointGeometryFromWkt = $wktReaderWriter->Read($agfTextPoint);
/// # or get the geometry from MgGeometryFactory
/// $coordinateXY = $geometryFactory->CreateCoordinateXY(0, 0);
/// $pointGeometryFromFactory = $geometryFactory->CreatePoint($coordinateXY);
///
/// # translate the MgGeometry object into an MgByteReader object
/// $byteReader = $agfReaderWriter->Write($pointGeometryFromWkt);
/// $byteReader = $agfReaderWriter->Write($pointGeometryFromFactory);
///
/// # create an MgGeometryProperty object from the byte reader
/// $geometryProperty = new MgGeometryProperty("geomPropName", $byteReader);
///
/// # In the MgFeatureReader to MgGeometry direction:
/// $byteReader = $featureReader->GetGeometry("geomPropName");
/// $pointGeometry = $agfReaderWriter->Read($byteReader);
/// $agfTextPoint = $wktReaderWriter->Write($pointGeometry);
/// # print "POINT XY (0 0)" to screen
/// echo "$agfTextPoint\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// \code
/// using OSGeo.MapGuide;
/// private MgAgfReaderWriter agfReaderWriter;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgPoint pt11;
/// private MgPoint pt11FromText;
/// private String pt11TextSpec = "POINT XY ( 1 1 )";
/// private MgByteReader byteReader;
/// private MgGeometryProperty point11Prop;
/// private MgFeatureQueryOptions containsPoint11QryOpt;
/// private MgFeatureService featureService;
/// private MgResourceIdentifier resourceId;
/// private String className;
/// private MgFeatureReader featureReader;
/// private MgGeometry geometry;
/// private String geometryText;
///
/// public MgByteReader WktTextToMgByteReader(String wktText)
/// {
///     MgByteReader byteReader = null;
///     MgGeometry geometry = wktReaderWriter.Read(wktText);
///     byteReader = agfReaderWriter.Write(geometry);
///     return byteReader;
/// }
///
/// public MgPoint CreateAPointXY(double x, double y)
/// {
///     MgCoordinate coord = geometryFactory.CreateCoordinateXY(x, y);
///     return geometryFactory.CreatePoint(coord);
/// }
///
/// className = "featureClassName";
/// agfReaderWriter = new MgAgfReaderWriter();
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryFactory = new MgGeometryFactory();
/// // In the MgGeometry to MgGeometryProperty direction:
/// // get the geometry from the MgWktReaderWriter
/// pt11FromText = wktReaderWriter.Read(pt11TextSpec) as MgPoint;
/// // or get the geometry from the geometry factory
/// pt11 = CreateAPointXY(1, 1);
///
/// // translate the MgGeometry object into an MgByteReader object
/// byteReader = agfReaderWriter.Write(pt11);
/// byteReader = agfReaderWriter.Write(pt11FromText);
///
/// // create an MgGeometryProperty object from the byte reader
/// point11Prop = new MgGeometryProperty("theFeatureGeometry", byteReader);
///
/// // In the MgFeatureReader to MgGeometry direction:
/// containsPoint11QryOpt = new MgFeatureQueryOptions();
/// containsPoint11QryOpt.SetFilter("theFeatureGeometry CONTAINS GEOMFROMTEXT( 'POINT(1 1)' )");
/// // the MgResourceIdentifier object, resourceId, is the feature source being queried
/// featureReader = featureService.SelectFeatures(resourceId, className, containsPoint11QryOpt);
/// while (featureReader.ReadNext())
/// {
///     byteReader = featureReader.GetGeometry("theFeatureGeometry");
///     geometry = agfReaderWriter.Read(byteReader);
///     geometryText = wktReaderWriter.Write(geometry);
///     // geometryText now contains the text representation of the geometry
/// }
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgAgfReaderWriter : public MgGuardDisposable
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
