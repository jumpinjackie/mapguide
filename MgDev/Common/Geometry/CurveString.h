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

#ifndef _MGCURVESTRING_H_
#define _MGCURVESTRING_H_

class MgCurveString;
template class MG_GEOMETRY_API Ptr<MgCurveString>;

/// \defgroup MgCurveString MgCurveString
/// \ingroup Geometry_Module_classes
/// \{

//////////////////////////////////////////////////////////////////
/// \brief
/// An MgCurveString is composed of one or more curve segments.
///
/// \remarks
/// The curve segments are connected to one another so that the
/// end point of each segment is the start point of the next
/// segment in the segment list. The end point of the last
/// segment in the ring is not the start point of the first
/// segment. That is the curve string is not closed.
/// \n
/// An object of this type is constructed by calling the
/// non-static MgGeometryFactory::CreateCurvePolygon() method
/// and, once constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of a curve string,
/// which has a circular arc and a linear segment
///
/// \code
/// $geometryFactory = new MgGeometryFactory();
/// $coordinateCollection = new MgCoordinateCollection();
/// $curveSegmentCollection = new MgCurveSegmentCollection();
/// $wktReaderWriter = new MgWktReaderWriter();
///
/// // create the circular arc
/// $startCoordinate = $geometryFactory->CreateCoordinateXY(0,2);
/// $controlCoordinate = $geometryFactory->CreateCoordinateXY(2,4);
/// $endCoordinate = $geometryFactory->CreateCoordinateXY(4,2);
/// $arcSegment = $geometryFactory->CreateArcSegment($startCoordinate,
/// $endCoordinate, $controlCoordinate);
/// $index = $curveSegmentCollection->Add($arcSegment);
/// echo "Arc segment added to curve segment collection at index: $index\n";
///
/// // create linear segment
/// // note that the first coordinate is the same as the last
/// // coordinate of the circular arc
/// $coordinate = $geometryFactory->CreateCoordinateXY(4,2);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
/// $coordinate = $geometryFactory->CreateCoordinateXY(2,0);
/// $index = $coordinateCollection->Add($coordinate);
/// echo "Coordinate added to coordinate collection at index: $index\n";
/// $linearSegment = $geometryFactory->CreateLinearSegment($coordinateCollection);
/// $index = $curveSegmentCollection->Add($linearSegment);
/// echo "Linear segment added to curve segment collection at index: $index\n";
///
/// // create curve string
/// $curveString = $geometryFactory->CreateCurveString($curveSegmentCollection);
///
/// // print out the Agf Text string for the geometry
/// $curveStringAgfText = $wktReaderWriter->Write($curveString);
/// echo "AGF Text representation of curve string: $curveStringAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The following code shows the construction of a curve string,
/// which has a circular arc and a linear segment
/// \code
/// using OSGeo.MapGuide;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgCurveString csAs001120Ls203132;
/// // the following arrays are used for segments of a curve string
/// // the first coordinate in each array is actually a flag
/// // 0 indicates an arc segment and 1 indicates a linear segment
/// private double[,] as001120 = { { 0, 0 }, { 0, 0 }, { 1, 1 }, { 2, 0 } };
/// private double[,] ls203132 = { { 1, 1 }, { 2, 0 }, { 3, 1 }, { 3, 2 } };
/// private double[][,] csAs001120Ls203132Data;
/// private String geometryAgfText;
///
/// // The following BNF is for the construction of a string representation
/// // of the geometry.
/// // The <StartPoint> for every segment except the first is equal
/// // to the end point of the previous segment.
/// // So the <StartPoint> must be explicit for the first segment
/// // and implicit for every segment thereafter.
/// // When using the API to construct the geometry, however,
/// // you must make the start point explicit in the data.
/// // So in the data the end point from the previous segment and
/// // the start point from the next segment are identical.
/// // <CurveString> ::= '(' <StartPoint> '(' <CurveSegmentCollection> ')' ')'
/// // See CreateACurveSegmentCollectionXY() for the rest of the BNF.
/// public MgCurveString CreateACurveStringXY(double[][,] curveStringData)
/// {
///     MgCurveSegmentCollection segments = CreateACurveSegmentCollectionXY(curveStringData);
///     return geometryFactory.CreateCurveString(segments);
/// }
///
/// // <CurveSegmentCollection> ::= <CurveSegment>
/// //                              | <CurveSegmentCollection> ',' <CurveSegment>
/// // <CurveSegment> ::= CIRCULARARCSEGMENT '(' <2ndPoint> ',' <EndAndStartPointNextSeg> ')'
/// //                    | LINESTRINGSEGMENT '(' <LineSegmentPointCollection> ')'
/// // <LineSegmentPointCollection> ::= <EndAndStartPointNextSeg>
/// //                    | <2ndPoint>, <EndAndStartPointNextSeg>
/// //                    | <2ndPoint>, <PointCollection>, <EndAndStartPointNextSeg>
/// public MgCurveSegmentCollection CreateACurveSegmentCollectionXY(double[][,] curveStringData)
/// {
///     MgCurveSegmentCollection segments = new MgCurveSegmentCollection();
///     MgCoordinateCollection coords = new MgCoordinateCollection();
///     double[,] segmentData;
///     MgCoordinate start;
///     MgCoordinate control;
///     MgCoordinate end;
///     for (int i = 0; i < curveStringData.GetLength(0); i++)
///     {
///         segmentData = curveStringData[i];
///         // 0 indicates that rest of data in array is for an arc segment
///         if (segmentData[0, 0] == 0)
///         {
///             start = geometryFactory.CreateCoordinateXY(segmentData[1, 0],
///                 segmentData[1, 1]);
///             control = geometryFactory.CreateCoordinateXY(segmentData[2, 0],
///                 segmentData[2, 1]);
///             end = geometryFactory.CreateCoordinateXY(segmentData[3, 0],
///                 segmentData[3, 1]);
///             segments.Add(geometryFactory.CreateArcSegment(start, end, control));
///         }
///         else if (segmentData[0, 0] == 1)
///         {
///             for (int j = 1; j < segmentData.GetLength(0); j++)
///             {
///                 coords.Add(geometryFactory.CreateCoordinateXY(segmentData[j, 0],
///                     segmentData[j, 1]));
///             }
///             segments.Add(geometryFactory.CreateLinearSegment(coords));
///             coords.Clear();
///         }
///         else
///         {
///             // should throw exception
///             return null;
///         }
///     }
///     return segments;
/// }
///
/// geometryFactory = new MgGeometryFactory();
/// csAs001120Ls203132Data = new double[2][,];
/// csAs001120Ls203132Data[0] = as001120;
/// csAs001120Ls203132Data[1] = ls203132;
/// csAs001120Ls203132 = CreateACurveStringXY(csAs001120Ls203132Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(csAs001120Ls203132);
/// // geometryAgfText now contains:
/// // "CURVESTRING XY (0 0(CIRCULARARCSEGMENT(1 1, 2 0), LINESTRINGSEGMENT(3 1, 3 2)))"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgCurveString : public MgCurve
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgCurveString)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of segments in this curve string.
    ///
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
    /// \return
    /// The number of curve segments.
    ///
    virtual INT32 GetCount();  /// __get

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the curve segment at the specified index. The indexing
    /// starts at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveSegment GetSegment(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveSegment GetSegment(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveSegment GetSegment(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the curve
    /// segment in the collection.
    ///
    /// \return
    /// The MgCurveSegment at the specified index.
    ///
    virtual MgCurveSegment* GetSegment(INT32 index);

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
    virtual MgCoordinate* GetStartCoordinate();  /// __get, __inherited

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
    virtual MgCoordinate* GetEndCoordinate();  /// __get, __inherited

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
    /// Construct a MgCurveString object
    ///
    MgCurveString(MgCurveSegmentCollection* curveSegments);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgCurveString object for deserialization
    ///
    MgCurveString();

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

    /// \internal
    /// Return curve segment collection INTERNAL USE ONLY
    MgCurveSegmentCollection* GetCurveSegments();

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
    /// Class Identifider.
    ///
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_CurveString;

private:
    Ptr<MgCurveSegmentCollection>   m_segments;
};
/// \}

#endif //_MGCURVESTRING_H_
