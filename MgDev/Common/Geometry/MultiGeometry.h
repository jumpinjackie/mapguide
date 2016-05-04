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

#ifndef _MGMULTIGEOMETRY_H_
#define _MGMULTIGEOMETRY_H_

class MgMultiGeometry;
template class MG_GEOMETRY_API Ptr<MgMultiGeometry>;

/// \defgroup MgMultiGeometry MgMultiGeometry
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgMultiGeometry is a heterogeneous aggregate of one or
/// more MgGeometry objects.
///
/// \remarks
/// Both "single" and "multiple"
/// MgGeometry objects, for example, MgPoint and MgMultiPoint
/// objects, may be included. The geometries are not connected
/// or ordered. An instance of this class is constructed by
/// calling a non-static MgGeometryFactory::CreateMultiGeometry()
/// method and, once constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of an
/// MgMultiGeoemtry geometry. Review the example code for the
/// construction of the possible constituent geometries:
/// \li MgPoint and MgMultiPoint
/// \li MgLineString and MgMultiLineString
/// \li MgCurveString and MgMultiCurveString
/// \li MgPolygon and MgMultiPolygon
/// \li MgCurvePolygon and MgMultiCurvePolygon
///
/// \code
/// $geometryFactory = new MgGeometryFactory();
/// $geometryCollection = new MgGeometryCollection();
///
/// // After each MgGeometry is constructed,
/// // it is added to the MgGeometryCollection.
/// $index = $geometryCollection->Add($point);
/// $index = $geometryCollection->Add($multiPoint);
/// $index = $geometryCollection->Add($lineString);
/// $index = $geometryCollection->Add($multiLineString);
/// $index = $geometryCollection->Add($curveString);
/// $index = $geometryCollection->Add($multiCurveString);
/// $index = $geometryCollection->Add($polygon);
/// $index = $geometryCollection->Add($multiPolygon);
/// $index = $geometryCollection->Add($curvePolygon);
/// $index = $geometryCollection->Add($multiCurvePolygon);
///
/// // construct the MgMultiGeometry geometry
/// $multiGeometry = $geometryFactory->CreateMultiGeometry($geometryCollection);
///
/// // print out the Agf Text string for the geometry
/// $multiGeometryAgfText = $wktReaderWriter->Write($multiGeometry);
/// echo "AGF Text representation of MultiGeometry: $multiGeometryAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The following code shows the construction of an MgMultiGeometry object,
/// which consists of a point and a line.
///
/// \code
/// using OSGeo.MapGuide;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgMultiGeometry multiGeom;
/// private MgGeometryCollection geometries;
/// private MgPoint pt11;
/// private MgLineString ls1121;
/// private String geometryAgfText;
///
/// geometries = new MgGeometryCollection();
/// geometryFactory = new MgGeometryFactory();
/// // Review the MgPoint example code for the construction of the point geometry.
/// geometries.Add(pt11 as MgGeometry);
/// // Review the MgLineString example code for the construction of the line string geometry.
/// geometries.Add(ls1121 as MgGeometry);
/// multiGeom = geometryFactory.CreateMultiGeometry(geometries);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(multiGeom);
/// // geometryAgfText now contains:
/// // "GEOMETRYCOLLECTION ( POINT XY ( 1 1 ), LINESTRING XY ( 1 1, 2 1 ) )"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgMultiGeometry : public MgAggregateGeometry
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMultiGeometry)

PUBLISHED_API:
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
    virtual INT32 GetCount();  /// __get, __inherited

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry at the specified index. The indexing starts
    /// at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the
    /// geometry in the collection.
    ///
    /// \return
    /// The MgGeometry at the specified index.
    ///
    virtual MgGeometry* GetGeometry(INT32 index);

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual INT32 GetGeometryType();  /// __get, __inherited

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual INT32 GetDimension();  /// __get, __inherited

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual bool IsEmpty();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual bool IsClosed();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual MgGeometricEntity* Copy();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a transformed copy of this geometric entity.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param transform (MgTransform)
    /// The MgTransform to use in constructing a copy of this geometric entity.
    ///
    /// \return
    /// An MgGeometricEntity that is a copy of this one with each coordinate
    /// transformed.
    ///
    virtual MgGeometricEntity* Transform(MgTransform* transform);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgMultiGeometry object
    ///
    MgMultiGeometry(MgGeometryCollection* geometries);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgMultiGeometry object for deserialization
    ///
    MgMultiGeometry();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

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
    virtual MgGeometry* GetGeometryAt(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual STRING ToAwkt(bool is2dOnly);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculate the envelope for this geometry
    ///
    /// \return
    /// MgEnvelope.
    ///
    ///
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_MultiGeometry;

private:
    Ptr<MgGeometryCollection>   m_geometries;
};
/// \}

#endif //_MGMULTIGEOMETRY_H_
