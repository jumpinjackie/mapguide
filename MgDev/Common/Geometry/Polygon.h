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

#ifndef _MGPOLYGON_H_
#define _MGPOLYGON_H_

class MgPolygon;
template class MG_GEOMETRY_API Ptr<MgPolygon>;

/// \defgroup MgPolygon MgPolygon
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// An MgPolygon is a region, defined by 1 exterior boundary and
/// 0 or more interior boundaries.
///
/// \remarks
/// Each interior boundary defines
/// a hole in the polygon. The interior and exterior boundaries
/// are defined with linear interpolation between points. Each
/// consecutive pair of points defines a line segment. An object
/// of this type is constructed by calling a non-static
/// MgGeometryFactory::CreatePolygon() method and, once
/// constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following example code creates a square polygon, whose
/// side is 3 units in length, and which contains a "hole" at its
/// center. The "hole" is also a square, whose side is 1 unit in
/// length.
///
/// \code
/// $geometryFactory = new MgGeometryFactory();
/// $coordinateCollection = new MgCoordinateCollection();
/// $interiorRingCollection = new MgLinearRingCollection();
/// $wktReaderWriter = new MgWktReaderWriter();
/// // note that the coordinates for the external boundary
/// // are added to the coordinate collection in a
/// // counterclockwise order
/// // create first coordinate for external boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(0,0);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create second coordinate for external boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(3,0);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create third coordinate for external boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(3,3);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create fourth coordinate for external boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(0,3);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create fifth coordinate for external boundary that is the same as
/// // the first to close the boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(0,0);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create LinearRing that defines the external boundary
/// $exteriorRing = $geometryFactory->CreateLinearRing($coordinateCollection);
/// // clear the collection before reusing for internal boundary
/// $coordinateCollection->Clear();
///
/// // note that the coordinates for the internal boundary
/// // are added to the coordinate collection in a
/// // clockwise order
/// // create first coordinate for internal boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(1,1);
///
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create second coordinate for internal boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(1,2);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create third coordinate for internal boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(2,2);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create fourth coordinate for internal boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(2,1);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create fifth coordinate for internal boundary that is the same as
/// // the first to close the boundary
/// $coordinate = $geometryFactory->CreateCoordinateXY(1,1);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
///
/// // create LinearRing that defines the internal boundary
/// $interiorRing =
/// $geometryFactory->CreateLinearRing($coordinateCollection);
///
/// // add the interior ring to the interior ring collection
/// $index = $interiorRingCollection->Add($interiorRing);
///
/// // create the MgPolygon geometry
/// $polygon = $geometryFactory->CreatePolygon($exteriorRing, $interiorRingCollection);
///
/// // print out the Agf Text string for the geometry
/// $polygonAgfText = $wktReaderWriter->Write($polygon);
/// echo "AGF Text representation of line string: $polygonAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The following example code creates a square polygon, whose
/// side is 3 units in length, and which contains a "hole" at its
/// center. The "hole" is also a square, whose side is 1 unit in
/// length.
/// \code
/// using OSGeo.MapGuide;
///
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// // this polygon has one exterior ring and one interior ring
/// private MgPolygon pner1141441411ir2223333222;
/// // the following array is used for the exterior ring of a polygon
/// // the points are in counter-clockwise order
/// private double[,] da1141441411 = { { 1, 1 }, { 4, 1 }, { 4, 4 }, { 1, 4 }, { 1, 1 } };
/// // the following array is used for the interior ring of a polygon
/// // the points are in clockwise order
/// private double[,] da2223333222 = { { 2, 2 }, { 2, 3 }, { 3, 3 }, { 3, 2 }, { 2, 2 } };
/// // this array contains an array that will be used for an exterior ring
/// // and one that will be used for an interior ring
/// private double[][,] pnEr1141441411Ir2223333222Data;
/// private String geometryAgfText;
///
/// public MgLinearRing CreateALinearRingXY(double[,] ringData)
/// {
///     MgCoordinateCollection coords = new MgCoordinateCollection();
///     for (int i = 0; i < ringData.GetLength(0); i++)
///     {
///         coords.Add(geometryFactory.CreateCoordinateXY(ringData[i, 0], ringData[i, 1]));
///     }
///     return geometryFactory.CreateLinearRing(coords);
/// }
///
/// public MgPolygon CreateAPolygonXY(double[][,] ringsData)
/// {
///     MgLinearRing exteriorRing = CreateALinearRingXY(ringsData[0]);
///     MgLinearRingCollection interiorRings = new MgLinearRingCollection();
///     for (int i = 1; i < ringsData.GetLength(0); i++)
///     {
///         interiorRings.Add(CreateALinearRingXY(ringsData[i]));
///     }
///     return geometryFactory.CreatePolygon(exteriorRing, interiorRings);
/// }
///
/// geometryFactory = new MgGeometryFactory();
/// pnEr1141441411Ir2223333222Data = new double[2][,];
/// pnEr1141441411Ir2223333222Data[0] = da1141441411;
/// pnEr1141441411Ir2223333222Data[1] = da2223333222;
/// // create a polygon using the geometry factory
/// pner1141441411ir2223333222 = CreateAPolygonXY(pnEr1141441411Ir2223333222Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(pner1141441411ir2223333222);
/// // geometryAgfText now contains:
/// // "POLYGON XY ((1 1, 4 1, 4 4, 1 4, 1 1), (2 2, 2 3, 3 3, 3 2, 2 2))"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgPolygon : public MgRegion
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgPolygon)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the exterior ring of the polygon.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLinearRing GetExteriorRing();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLinearRing GetExteriorRing();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLinearRing GetExteriorRing();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgLinearRing that defines the exterior ring of the polygon.
    ///
    virtual MgLinearRing* GetExteriorRing();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of interior rings in this polygon.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetInteriorRingCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetInteriorRingCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetInteriorRingCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The number of interior rings.
    ///
    virtual INT32 GetInteriorRingCount();  /// __get

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the interior ring at the specified index. The indexing
    /// starts at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLinearRing GetInteriorRing(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLinearRing GetInteriorRing(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLinearRing GetInteriorRing(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the
    /// interior ring in the collection.
    ///
    /// \return
    /// The MgLinearRing at the specified index.
    ///
    virtual MgLinearRing* GetInteriorRing(INT32 index);

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
    virtual MgCoordinateIterator* GetCoordinates();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgPolygon object
    ///
    MgPolygon(MgLinearRing* outerRing, MgLinearRingCollection* innerRings);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgPolygon object for deserialization
    ///
    MgPolygon();

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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to XML representation of it-self
    ///
    virtual void ToXml(std::string& str);

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculate the envelope for this geometry
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_Polygon;

private:
    Ptr<MgLinearRing> m_outerRing;
    Ptr<MgLinearRingCollection> m_innerRings;
};
/// \}

#endif //_MGPOLYGON_H_
