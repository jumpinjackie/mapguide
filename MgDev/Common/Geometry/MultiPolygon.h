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

#ifndef _MGMULTIPOLYGON_H_
#define _MGMULTIPOLYGON_H_

class MgMultiPolygon;
template class MG_GEOMETRY_API Ptr<MgMultiPolygon>;

/// \defgroup MgMultiPolygon MgMultiPolygon
/// \ingroup Geometry_Module_classes
/// \{

//////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgMultiPolygon is a two-dimensional aggregate geometry
/// whose elements are MgPolygon geometries.
///
/// \remarks
/// The polygons are not
/// connected or ordered. An instance of this class is
/// constructed by calling a non-static
/// MgGeometryFactory::CreateMultiPolygon() method and, once
/// constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of an
/// MgMultiPolygon geometry. Review the MgPolygon example code.
///
/// \code
/// // A helper class additional to those created in the
/// // MgPolygon example code is needed.
/// $polygonCollection = new MgPolygonCollection();
/// // After each MgPolygon geometry is constructed,
/// // it is added to an MgPolygonCollection.
/// $index = $polygonCollection->Add($polygon);
/// echo "A polygon is added to a polygon collection at index: $index\n";
///
/// // construct the MgMultiPolygon geometry
/// $multiPolygon = $geometryFactory->
/// CreateMultiPolygon($polygonCollection);
///
/// // print out the Agf Text string for the geometry
/// $multiPolygonAgfText =
/// $wktReaderWriter->Write($multiPolygon);
/// echo "AGF Text representation of MultiPolygon: $multiPolygonAgfText\n";
/// \endcode
///
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The CreateAMultiPolygonXY method calls the CreateAPolygonXY method.
/// The code for the CreateAPolygonXY method is in the MgPolygon example code.
/// \code
/// using OSGeo.MapGuide;
///
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgMultiPolygon mpnPn0010110100Pn0010110100;
/// // this array contains the data for 2 polygons whose values are identical
/// private double[][][,] mpnPn0010110100Pn0010110100Data;
/// // the following array is used for the exterior ring of a polygon
/// // the points are in counterclockwise order
/// private double[,] da0010110100 = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 0 } };
/// private String geometryAgfText;
///
/// public MgMultiPolygon CreateAMultiPolygonXY(double[][][,] polygonData)
/// {
///     MgPolygonCollection polygons = new MgPolygonCollection(); ;
///     for (int i = 0; i < polygonData.GetLength(0); i++)
///     {
///         polygons.Add(CreateAPolygonXY(polygonData[i]));
///     }
///     return geometryFactory.CreateMultiPolygon(polygons);
/// }
///
/// geometryFactory = new MgGeometryFactory();
/// pnEr0010110100Data = new double[1][,];
/// pnEr0010110100Data[0] = da0010110100;
/// mpnPn0010110100Pn0010110100Data = new double[2][][,];
/// mpnPn0010110100Pn0010110100Data[0] = pnEr0010110100Data;
/// mpnPn0010110100Pn0010110100Data[1] = pnEr0010110100Data;
/// mpnPn0010110100Pn0010110100 = CreateAMultiPolygonXY(mpnPn0010110100Pn0010110100Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(mpnPn0010110100Pn0010110100);
/// // geometryAgfText now contains:
/// // "MULTIPOLYGON XY ((( 0 0, 1 0, 1 1, 0 1, 0 0 )), (( 0 0, 1 0, 1 1, 0 1, 0 0 )))"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgMultiPolygon : public MgAggregateGeometry
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMultiPolygon)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
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
    /// Gets the polygon at the specified index. The indexing starts
    /// at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPolygon GetPolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPolygon GetPolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPolygon GetPolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the
    /// polygon in the collection.
    ///
    /// \return
    /// The MgPolygon at the specified index
    ///
    virtual MgPolygon* GetPolygon(INT32 index);

    /////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////
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
    virtual MgCoordinateIterator* GetCoordinates();  /// __get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgMultiPolygon object
    ///
    MgMultiPolygon(MgPolygonCollection* polygons);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgMultiPolygon object for deserialization
    ///
    MgMultiPolygon();

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
    static const INT32 m_cls_id = Geometry_MultiPolygon;

private:
    Ptr<MgPolygonCollection>   m_polygons;
};
/// \}

#endif //_MGMULTIPOLYGON_H_
