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

#ifndef _MGMULTICURVEPOLYGON_H_
#define _MGMULTICURVEPOLYGON_H_

class MgMultiCurvePolygon;
template class MG_GEOMETRY_API Ptr<MgMultiCurvePolygon>;

/// \defgroup MgMultiCurvePolygon MgMultiCurvePolygon
/// \ingroup Geometry_Module_classes
/// \{

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgMultiCurvePolygon is a two-dimensional aggregate
/// geometry whose elements are MgCurvePolygon geometries.
///
/// \remarks
/// The
/// polygons are not connected or ordered. An instance of this
/// class is constructed by calling a non-static
/// MgGeometryFactory::CreateMultiCurvePolygon() method and, once
/// constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of an
/// MgMultiCurvePolygon geometry. Review the MgCurvePolygon example code.
///
/// \code
/// // A helper class additional to those created in the
/// // MgCurvePolygon example code is needed.
/// $curvePolygonCollection = new MgCurvePolygonCollection();
///
/// // After each MgCurvePolygon geometry is constructed,
/// // it is added to an MgCurvePolygonCollection.
/// $index = $curvePolygonCollection->Add($curvePolygon);
/// echo "A curve polygon is added to a curve polygon  at index: $index\n";
///
/// // construct the MgMultiCurvePolygon geometry
/// $multiCurvePolygon = $geometryFactory-> CreateMultiCurvePolygon($curvePolygonCollection);
///
/// // print out the Agf Text string for the geometry
/// $multiCurvePolygonAgfText = $wktReaderWriter->Write($multiCurvePolygon);
/// echo "AGF Text representation of MultiCurvePolygon: $multiCurvePolygonAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
///
/// The following code shows the construction of an MgMultiCurvePolygon object,
/// which consists of two identical circles.
/// The CreateAMultiCurvePolygonXY method calls the CreateACurvePolygonXY method.
/// The code for the CreateACurvePolygonXY method is in the MgCurvePolygon example code.
///
/// \code
/// using OSGeo.MapGuide;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgMultiCurvePolygon mcpnEr0224422002Ir12233212Er0224422002Ir12233212;
/// private double[][][][,] mcpnEr0224422002Ir12233212Er0224422002Ir12233212Data;
/// private String geometryAgfText;
///
/// public MgMultiCurvePolygon CreateAMultiCurvePolygonXY(double[][][][,] polygonData)
/// {
///     MgCurvePolygonCollection polygons = new MgCurvePolygonCollection();
///     for (int i = 0; i < polygonData.GetLength(0); i++)
///     {
///         polygons.Add(CreateACurvePolygonXY(polygonData[i]));
///     }
///     return geometryFactory.CreateMultiCurvePolygon(polygons);
/// }
///
/// geometryFactory = new MgGeometryFactory();
/// mcpnEr0224422002Ir12233212Er0224422002Ir12233212Data = new double[2][][][,];
/// // the construction of cpEr0220422402Ir122332112Data is shown in the MgCurvePolygon example code.
/// mcpnEr0224422002Ir12233212Er0224422002Ir12233212Data[0] = cpEr0220422402Ir122332112Data;
/// mcpnEr0224422002Ir12233212Er0224422002Ir12233212Data[1] = cpEr0220422402Ir122332112Data;
/// mcpnEr0224422002Ir12233212Er0224422002Ir12233212 = CreateAMultiCurvePolygonXY(mcpnEr0224422002Ir12233212Er0224422002Ir12233212Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(mcpnEr0224422002Ir12233212Er0224422002Ir12233212);
/// // geometryAgfText now contains:
/// // "MULTICURVEPOLYGON XY ((( 0 2 (CIRCULARARCSEGMENT( 2 0, 4 2), CIRCULARARCSEGMENT( 2 4, 0 2))), ( 1 2 ( CIRCULARARCSEGMENT( 2 3, 3 2), CIRCULARARCSEGMENT( 2 1, 1 2)))),
/// //  (( 0 2 (CIRCULARARCSEGMENT( 2 0, 4 2), CIRCULARARCSEGMENT( 2 4, 0 2))), ( 1 2 ( CIRCULARARCSEGMENT( 2 3, 3 2), CIRCULARARCSEGMENT( 2 1, 1 2)))))"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgMultiCurvePolygon : public MgAggregateGeometry
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMultiCurvePolygon)

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

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the polygon at the specified index. The indexing starts
    /// at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurvePolygon GetCurvePolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurvePolygon GetCurvePolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurvePolygon GetCurvePolygon(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the curve
    /// polygon in the collection.
    ///
    /// \return
    /// The MgCurvePolygon at the specified index.
    ///
    virtual MgCurvePolygon* GetCurvePolygon(INT32 index);

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
    /// Construct a MgMultiCurvePolygon object
    ///
    MgMultiCurvePolygon(MgCurvePolygonCollection* polygons);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgMultiCurvePolygon object for deserialization
    ///
    MgMultiCurvePolygon();

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
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_MultiCurvePolygon;

private:
    Ptr<MgCurvePolygonCollection>   m_polygons;
};
/// \}

#endif //_MGMULTICURVEPOLYGON_H_
